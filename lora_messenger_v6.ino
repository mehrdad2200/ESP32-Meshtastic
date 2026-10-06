// ==========================================================================
//  LoRa Messenger v6  -  ESP32 + RA-02 (SX1278)
// --------------------------------------------------------------------------
//  Phone chat over the ESP32's own WiFi  ->  http://192.168.4.1
//
//  Security  : AES-128-GCM (8-byte tag) on every packet, key from a shared
//              passphrase (set in the web UI), replay protection,
//              long-press button wipe.
//  Messaging : delivery ACK + retry, read receipts, reply-to, edit/delete,
//              quick replies, long messages (fragmentation), compact
//              Persian text encoding, contacts + presence beacons,
//              optional repeater mode (TTL flooding).
//  Location  : send / live-share the phone's GPS, distance + bearing,
//              map links, GPX track export, SOS.
//  Radio     : frequency selection, channel noise scan, packet statistics.
//  Meshtastic: optional interoperability mode - speaks the real Meshtastic LoRa protocol
//              (text, DMs + ACK, node info, positions, rebroadcast) so this node joins an
//              existing Meshtastic mesh. Enable it in Settings > Meshtastic.
//  UI        : minimal dark/light web app, installable (PWA).
//
//  Libraries (Library Manager):  "LoRa" by Sandeep Mistry
//  Everything else is built into the ESP32 Arduino core.
//
//  Wiring
//    RA-02  3.3V->3V3  GND->GND  SCK->18  MISO->19  MOSI->23
//           NSS->5     RST->14   DIO0->26
//    Button: GPIO4 <-> GND
//            short press = PING      hold 5 s = wipe key/settings/chat
//
//  ALL nodes must run this same version and use the same passphrase and
//  frequency.  (Private chats and channels need every node on v6; public text stays compatible with v3-v5.)
// ==========================================================================
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <mbedtls/gcm.h>
#include <mbedtls/sha256.h>
#include <time.h>
#include "assets.h"
#include "mt_core.h"      // Meshtastic wire protocol (pure C++, no Arduino dependencies)

// ------------------------------ Pins ---------------------------------------
#define LORA_SS     5
#define LORA_RST    14
#define LORA_DIO0   26
#define LED_PIN     2
#define BTN_PIN     4
#define USE_BUTTON  1

// ------------------------------ Radio --------------------------------------
#define LORA_SF          10
#define LORA_BW          125E3
#define LORA_CR          5
#define LORA_PREAMBLE    8
#define LORA_TX_POWER    17
#define FREQ_DEFAULT_KHZ 433175UL
#define FREQ_MIN_KHZ     433050UL
#define FREQ_MAX_KHZ     434790UL

// ------------------------------ Limits -------------------------------------
#define HDR_LEN        9       // flags/ttl(1) + uid(4) + counter(4)
#define TAG_LEN        8
#define MAX_RAW        255
#define MAX_WIRE       190
#define FRAG_DATA      100     // compressed bytes per fragment
#define MAX_FRAGS      6
#define MAX_TEXT_BYTES 500
#define MAX_TRIES      4
#define SOS_TRIES      8
#define DEFAULT_TTL    3
#define BEACON_MS      120000UL
#define MAX_CONTACTS   8
#define TRACK_MAX      100
#define MAX_MSGS       60
#define MAX_QUEUE      14
#define MAX_DEFER      10
#define MAX_FWD        4
#define MAX_ASM        3
#define MAX_READQ      12
#define MAX_CH         3       // extra channels with their own passphrase
#define MAX_ACKBY      7

enum { K_TEXT = 0, K_QUICK = 1, K_LOC = 2, K_SOS = 3, K_EDIT = 4, K_DEL = 5 };

// ------------------------------ Types --------------------------------------
// (kept above every function so the Arduino auto-prototype step can see them)
struct TrackPt { int32_t lat, lon; uint32_t t; };

struct Contact {
  bool     used;
  uint32_t uid;
  char     name[40];
  uint32_t lastCtr;
  uint32_t lastSeen;     // millis(), 0 = never
  int      rssi;
  int      snr;
  uint32_t rxCount;
  bool     relayOn;
  bool     hasLoc;
  int32_t  lat, lon;
  uint16_t acc;
  uint32_t locAt;
  uint16_t trkCount, trkHead;
  TrackPt  trk[TRACK_MAX];
};

struct Msg {
  uint32_t seq;           // 0 = empty slot
  uint32_t rev;
  char     dir;           // 'I', 'O', 'S'
  uint8_t  kind;
  uint8_t  status;        // outgoing: 0 sending, 1 delivered, 2 failed, 3 read
  uint8_t  acks, reads, ch;
  uint32_t dst;           // 0 = public, else private message to this uid
  uint32_t ackBy[MAX_ACKBY], readBy[MAX_ACKBY];
  uint8_t  partsTotal, partsMask;
  bool     edited, deleted, readSent, hasReply, hasLoc;
  uint32_t uid;
  uint16_t id;
  uint32_t pid;           // Meshtastic packet id (0 = native message)
  uint32_t rUid;
  uint16_t rId;
  int      rssi, snr;
  uint32_t t;
  int32_t  lat, lon;
  uint16_t acc;
  String   text;
};

struct OutItem {
  uint8_t  kind;
  uint16_t id;
  uint8_t  fragIdx, fragTotal, flags;
  uint32_t refUid;
  uint16_t refId;
  uint8_t  dlen;
  uint8_t  data[110];
  uint16_t logId;
  bool     prio;
  uint8_t  ch;
  uint32_t dst;
};

struct Pending { bool active; OutItem it; uint8_t tries, maxTries; uint32_t sentAt, timeoutMs; };

struct PingState { bool active; uint16_t id; uint32_t start, timeoutMs; uint8_t replies; };

struct Deferred { bool used; uint32_t due; uint8_t len; uint8_t buf[64]; uint8_t ch; };

struct Fwd { bool used; uint32_t due; int len; uint8_t buf[MAX_RAW]; uint32_t uid, ctr; };

struct RdEntry { uint32_t uid; uint16_t id; };

struct Assembly {
  bool used; uint32_t uid; uint16_t baseId; uint8_t total, mask;
  String part[MAX_FRAGS];
  bool hasReply; uint32_t rUid; uint16_t rId;
  uint32_t started; int rssi, snr;
};

WebServer server(80);
Preferences prefs;
mbedtls_gcm_context gcm;
DNSServer dnsServer;
mbedtls_gcm_context gcmCh[MAX_CH];
bool     chUsed[MAX_CH];
char     chName[MAX_CH][25];
uint8_t  chKey[MAX_CH][16];
uint8_t  txCh = 0, rxCh = 0;       // channel used for the next transmit / of the packet being handled
uint32_t curDst = 0;               // destination of the message being handled
int      preset = 0, txPower = LORA_TX_POWER;
bool     captiveOn = true;
uint32_t cSupp = 0;
const uint8_t PRESET_SF[4] = {10, 12, 7, 7};
const long    PRESET_BW[4] = {125000L, 125000L, 125000L, 250000L};

// ------------------------------ Meshtastic mode ----------------------------
bool     meshMode = false;          // true = speak the Meshtastic protocol instead of LoRa Link
int      mtPreset = 0;              // Meshtastic ModemPreset enum (0 = LongFast)
String   mtChName;                  // primary channel name ("" = preset name, like Meshtastic)
uint8_t  mtKey[32];                 // expanded channel key
int      mtKeyLen = 16;             // 0 = no encryption, 16 = AES-128, 32 = AES-256
uint8_t  mtHash = 8;                // channel hash carried in every packet
uint8_t  mtSlot = 0;                // 0 = auto (hash of channel name), else 1-based frequency slot
uint8_t  mtHop = 3;                 // hop limit for packets we send
uint32_t mtFreqOvrKhz = 0;          // 0 = computed from preset/channel, else manual frequency
long     curBW = 125000L;           // active modem settings (used for airtime maths)
int      curSF = 10, curCR = 5, curPre = 8;
// defined in meshtastic.h (needed earlier in the file)
uint32_t mtCurFreqHz();
bool     mtDutyOk();
void     mtSendNodeInfo(uint32_t to, bool wantResp);

// ------------------------------ Settings / identity ------------------------
uint32_t myUid = 0;
String   myName, apSsid, apPass;
bool     wifiDefault = true;
uint32_t freqKhz = FREQ_DEFAULT_KHZ;
bool     relayOn = false;
uint8_t  aesKey[16];
bool     keyIsDefault = true;
uint16_t epochNo = 0, seq16 = 0;
uint16_t msgCounter = 1;
bool     timeKnown = false;
uint32_t timeBase = 0;
uint32_t restartAt = 0, wipeAt = 0;
uint32_t ledUntil = 0;
bool     ledWarn = false;
int      unread = 0;

// ------------------------------ Counters -----------------------------------
uint32_t cTx = 0, cRx = 0, cOk = 0, cAuth = 0, cReplay = 0, cDup = 0;
uint32_t cRetry = 0, cFail = 0, cRelay = 0, cForeign = 0;
float    avgRssi = -100;
uint32_t airBucket[60];
uint32_t airLastMin = 0;

// ------------------------------ Small helpers ------------------------------
static inline void putU16(uint8_t* p, uint16_t v) { p[0] = v >> 8; p[1] = v; }
static inline uint16_t getU16(const uint8_t* p) { return ((uint16_t)p[0] << 8) | p[1]; }
static inline void putU32(uint8_t* p, uint32_t v) { p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = v; }
static inline uint32_t getU32(const uint8_t* p) {
  return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}

String hex8(uint32_t v) { char b[9]; snprintf(b, sizeof(b), "%08X", (unsigned)v); return String(b); }

String jstr(const String& s) {
  String o;
  o.reserve(s.length() + 8);
  o += '"';
  for (unsigned i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '"') o += "\\\"";
    else if (c == '\\') o += "\\\\";
    else if ((uint8_t)c < 0x20) { char b[8]; snprintf(b, sizeof(b), "\\u%04x", (unsigned)(uint8_t)c); o += b; }
    else o += c;
  }
  o += '"';
  return o;
}

String sanitize(String s) {
  s.replace("\r", " ");
  s.replace("\n", " ");
  s.replace("\t", " ");
  s.trim();
  return s;
}

int utf8Len(uint8_t lead) {
  if (lead < 0x80) return 1;
  if ((lead & 0xE0) == 0xC0) return 2;
  if ((lead & 0xF0) == 0xE0) return 3;
  if ((lead & 0xF8) == 0xF0) return 4;
  return 1;
}

uint32_t utf8Decode(const uint8_t* p, int len) {
  if (len == 1) return p[0];
  if (len == 2) return ((uint32_t)(p[0] & 0x1F) << 6) | (p[1] & 0x3F);
  if (len == 3) return ((uint32_t)(p[0] & 0x0F) << 12) | ((uint32_t)(p[1] & 0x3F) << 6) | (p[2] & 0x3F);
  return ((uint32_t)(p[0] & 0x07) << 18) | ((uint32_t)(p[1] & 0x3F) << 12) | ((uint32_t)(p[2] & 0x3F) << 6) | (p[3] & 0x3F);
}

void appendUtf8(String& s, uint32_t cp) {
  if (cp < 0x80) s += (char)cp;
  else if (cp < 0x800) { s += (char)(0xC0 | (cp >> 6)); s += (char)(0x80 | (cp & 0x3F)); }
  else if (cp < 0x10000) { s += (char)(0xE0 | (cp >> 12)); s += (char)(0x80 | ((cp >> 6) & 0x3F)); s += (char)(0x80 | (cp & 0x3F)); }
  else { s += (char)(0xF0 | (cp >> 18)); s += (char)(0x80 | ((cp >> 12) & 0x3F)); s += (char)(0x80 | ((cp >> 6) & 0x3F)); s += (char)(0x80 | (cp & 0x3F)); }
}

// cut to maxBytes without splitting a UTF-8 character
String clipUtf8(String s, int maxBytes) {
  if ((int)s.length() <= maxBytes) return s;
  int n = maxBytes;
  while (n > 0 && (((uint8_t)s[n]) & 0xC0) == 0x80) n--;
  return s.substring(0, n);
}

String xmlEsc(const String& s) {
  String o;
  for (unsigned i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '&') o += "&amp;"; else if (c == '<') o += "&lt;"; else if (c == '>') o += "&gt;"; else o += c;
  }
  return o;
}

// ------------------------------ Compact Persian text -----------------------
// ASCII stays 1 byte, the 57 most common Persian/Arabic letters and signs
// become 1 byte (0x80..0xBF), anything else is escaped (0xFE + raw UTF-8).
const uint16_t PCODES[64] PROGMEM = {
  0x0627, 0x0622, 0x0628, 0x067E, 0x062A, 0x062B, 0x062C, 0x0686,
  0x062D, 0x062E, 0x062F, 0x0630, 0x0631, 0x0632, 0x0698, 0x0633,
  0x0634, 0x0635, 0x0636, 0x0637, 0x0638, 0x0639, 0x063A, 0x0641,
  0x0642, 0x06A9, 0x06AF, 0x0644, 0x0645, 0x0646, 0x0648, 0x0647,
  0x06CC, 0x0626, 0x0621, 0x0624, 0x0623, 0x0625, 0x0643, 0x064A,
  0x0629, 0x200C, 0x060C, 0x061F, 0x061B, 0x06F0, 0x06F1, 0x06F2,
  0x06F3, 0x06F4, 0x06F5, 0x06F6, 0x06F7, 0x06F8, 0x06F9, 0x00AB,
  0x00BB, 0x0640, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

int findCode(uint32_t cp) {
  if (!cp) return -1;
  for (int i = 0; i < 64; i++) if (pgm_read_word(&PCODES[i]) == cp) return i;
  return -1;
}

// Split text into self-contained compressed chunks (token boundaries only).
// Returns number of chunks, or -1 if it needs more than maxChunks.
int buildChunks(const String& s, uint8_t chunks[][FRAG_DATA + 8], uint8_t lens[], int maxChunks) {
  int n = 0, cur = 0, i = 0, L = s.length();
  const uint8_t* p = (const uint8_t*)s.c_str();
  while (i < L) {
    int cl = utf8Len(p[i]);
    if (i + cl > L) cl = L - i;
    uint8_t tok[8];
    int tl = 0;
    if (cl == 1) {
      if (p[i] >= 0x20 && p[i] < 0x80) { tok[0] = p[i]; tl = 1; }
    } else {
      uint32_t cp = utf8Decode(p + i, cl);
      int idx = findCode(cp);
      if (idx >= 0) { tok[0] = 0x80 + idx; tl = 1; }
      else { tok[0] = 0xFE; memcpy(tok + 1, p + i, cl); tl = 1 + cl; }
    }
    i += cl;
    if (!tl) continue;
    if (cur + tl > FRAG_DATA) {
      if (n + 1 >= maxChunks) return -1;
      lens[n] = cur;
      n++;
      cur = 0;
    }
    memcpy(chunks[n] + cur, tok, tl);
    cur += tl;
  }
  lens[n] = cur;
  return n + 1;
}

String decodeBytes(const uint8_t* d, int n) {
  String out;
  int i = 0;
  while (i < n) {
    uint8_t b = d[i++];
    if (b < 0x80) out += (char)b;
    else if (b == 0xFE) {
      if (i >= n) break;
      int l = utf8Len(d[i]);
      for (int k = 0; k < l && i < n; k++) out += (char)d[i++];
    } else if (b <= 0xBF) {
      uint16_t cp = pgm_read_word(&PCODES[b - 0x80]);
      if (cp) appendUtf8(out, cp);
    }
  }
  return out;
}

// ------------------------------ Time ---------------------------------------
uint32_t nowEpoch() { return timeKnown ? timeBase + millis() / 1000UL : 0; }

String qualityFa(int rssi) {
  if (rssi > -80) return "Excellent";
  if (rssi > -100) return "Good";
  if (rssi > -115) return "Weak";
  return "Very weak";
}

// ------------------------------ Airtime ------------------------------------
float airtimeMs(int bytes) {
  int sf = curSF;
  float tsym = (float)(1UL << sf) / (float)curBW * 1000.0f;
  int de = (tsym > 16.0f) ? 1 : 0;
  float num = 8.0f * bytes - 4.0f * sf + 28.0f + 16.0f;
  float den = 4.0f * (sf - 2 * de);
  int nPay = 8 + max((int)ceil(num / den) * curCR, 0);
  return ((curPre + 4.25f) + nPay) * tsym;
}

void airRoll() {
  uint32_t m = millis() / 60000UL;
  if (airLastMin > m) airLastMin = m;
  while (airLastMin < m) {
    airLastMin++;
    airBucket[airLastMin % 60] = 0;
    if (m - airLastMin > 120) airLastMin = m;
  }
}
void airAdd(float ms) { airRoll(); airBucket[airLastMin % 60] += (uint32_t)ms; }
uint32_t airHour() { airRoll(); uint32_t s = 0; for (int i = 0; i < 60; i++) s += airBucket[i]; return s; }

// ------------------------------ Crypto -------------------------------------
void deriveKey(const String& pass, uint8_t out[16]) {
  const char* salt = "LoRaLink-v3";
  const int SL = 11;
  String p = pass;
  if (p.length() > 64) p = p.substring(0, 64);
  int pl = p.length();
  uint8_t first[80], buf[112], h[32];
  memcpy(first, salt, SL);
  memcpy(first + SL, p.c_str(), pl);
  mbedtls_sha256(first, SL + pl, h, 0);
  for (int i = 0; i < 4000; i++) {
    memcpy(buf, h, 32);
    memcpy(buf + 32, p.c_str(), pl);
    memcpy(buf + 32 + pl, salt, SL);
    mbedtls_sha256(buf, 32 + pl + SL, h, 0);
  }
  memcpy(out, h, 16);
}

void applyKey(const uint8_t k[16]) {
  memcpy(aesKey, k, 16);
  mbedtls_gcm_free(&gcm);
  mbedtls_gcm_init(&gcm);
  mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, aesKey, 128);
}

String keyFp() {
  uint8_t h[32];
  mbedtls_sha256(aesKey, 16, h, 0);
  char b[8];
  snprintf(b, sizeof(b), "%02X%02X", h[0], h[1]);
  return String(b);
}

mbedtls_gcm_context* ctxFor(uint8_t k) {
  if (k == 0) return &gcm;
  if (k <= MAX_CH && chUsed[k - 1]) return &gcmCh[k - 1];
  return nullptr;
}

String chanFp(int i) {
  uint8_t h[32];
  mbedtls_sha256(chKey[i], 16, h, 0);
  char b[8];
  snprintf(b, sizeof(b), "%02X%02X", h[0], h[1]);
  return String(b);
}

void setChannel(int i, const String& name, const uint8_t k[16]) {
  memcpy(chKey[i], k, 16);
  strncpy(chName[i], name.c_str(), sizeof(chName[i]) - 1);
  chName[i][sizeof(chName[i]) - 1] = 0;
  mbedtls_gcm_free(&gcmCh[i]);
  mbedtls_gcm_init(&gcmCh[i]);
  mbedtls_gcm_setkey(&gcmCh[i], MBEDTLS_CIPHER_ID_AES, chKey[i], 128);
  chUsed[i] = true;
}

void loadChannels() {
  for (int i = 0; i < MAX_CH; i++) {
    chUsed[i] = false;
    mbedtls_gcm_init(&gcmCh[i]);
    String kn = "chk" + String(i), nn = "chn" + String(i);
    uint8_t k[16];
    if (prefs.isKey(kn.c_str()) && prefs.getBytes(kn.c_str(), k, 16) == 16)
      setChannel(i, prefs.getString(nn.c_str(), "Channel"), k);
  }
}

long curFreqHz() { return meshMode ? (long)mtCurFreqHz() : (long)freqKhz * 1000L; }

// (Re)configures the whole radio for the active mode.
void applyRadio() {
  if (meshMode) {
    const MtPresetDef& d = MT_PRESETS[mtPreset];
    curSF = d.sf; curBW = (long)d.bwHz; curCR = d.cr; curPre = MT_PREAMBLE;
    LoRa.setSyncWord(MT_SYNC_WORD);
  } else {
    curSF = PRESET_SF[preset]; curBW = PRESET_BW[preset]; curCR = LORA_CR; curPre = LORA_PREAMBLE;
    LoRa.setSyncWord(0x12);
  }
  LoRa.setFrequency(curFreqHz());
  LoRa.setSpreadingFactor(curSF);
  LoRa.setSignalBandwidth(curBW);
  LoRa.setCodingRate4(curCR);
  LoRa.setPreambleLength(curPre);
  LoRa.enableCrc();
  int pw = txPower;
  if (meshMode && pw > MT_MAX_TX_DBM) pw = MT_MAX_TX_DBM;     // EU_433 limit used by Meshtastic
  LoRa.setTxPower(pw, PA_OUTPUT_PA_BOOST_PIN);
}

uint32_t nextCtr() {
  if (seq16 == 0xFFFF) { epochNo++; prefs.putUShort("epoch", epochNo); seq16 = 0; }
  seq16++;
  return ((uint32_t)epochNo << 16) | seq16;
}

void txRaw(const uint8_t* buf, int len) {
  LoRa.beginPacket();
  LoRa.write(buf, len);
  LoRa.endPacket();
  cTx++;
  airAdd(airtimeMs(len));
  ledUntil = millis() + 40;
}

bool sendSecure(const uint8_t* plain, int plen, uint8_t ttl) {
  if (meshMode) return false;                       // native packets are never sent in Meshtastic mode
  if (plen + HDR_LEN + TAG_LEN > MAX_WIRE) return false;
  uint8_t wire[MAX_RAW];
  uint32_t ctr = nextCtr();
  wire[0] = 0xA0 | (ttl & 0x0F);
  putU32(wire + 1, myUid);
  putU32(wire + 5, ctr);
  uint8_t iv[12];
  memset(iv, 0, sizeof(iv));
  putU32(iv, myUid);
  putU32(iv + 4, ctr);
  uint8_t tag[TAG_LEN];
  mbedtls_gcm_context* g = ctxFor(txCh);
  if (!g) g = &gcm;
  int rc = mbedtls_gcm_crypt_and_tag(g, MBEDTLS_GCM_ENCRYPT, plen, iv, 12, wire + 1, 8,
                                     plain, wire + HDR_LEN, TAG_LEN, tag);
  if (rc != 0) return false;
  memcpy(wire + HDR_LEN + plen, tag, TAG_LEN);
  txRaw(wire, HDR_LEN + plen + TAG_LEN);
  return true;
}

// ------------------------------ Contacts -----------------------------------
Contact contacts[MAX_CONTACTS];     // [0] = this device
uint32_t nextBeaconAt = 0, lastBeaconAt = 0;

Contact* findContact(uint32_t uid) {
  for (int i = 0; i < MAX_CONTACTS; i++) if (contacts[i].used && contacts[i].uid == uid) return &contacts[i];
  return nullptr;
}

void initContact(Contact* c, uint32_t uid) {
  c->used = true;
  c->uid = uid;
  snprintf(c->name, sizeof(c->name), "Node-%04X", (unsigned)(uid & 0xFFFF));
  c->lastCtr = 0; c->lastSeen = 0; c->rssi = -120; c->snr = 0; c->rxCount = 0;
  c->relayOn = false; c->hasLoc = false; c->lat = 0; c->lon = 0; c->acc = 0; c->locAt = 0;
  c->trkCount = 0; c->trkHead = 0;
}

Contact* addContact(uint32_t uid) {
  int slot = -1;
  for (int i = 1; i < MAX_CONTACTS; i++) if (!contacts[i].used) { slot = i; break; }
  if (slot < 0) {
    slot = 1;
    for (int i = 2; i < MAX_CONTACTS; i++) if (contacts[i].lastSeen < contacts[slot].lastSeen) slot = i;
  }
  initContact(&contacts[slot], uid);
  if (!meshMode && millis() - lastBeaconAt > 20000UL) nextBeaconAt = millis() + random(1500, 4000);   // introduce ourselves
  return &contacts[slot];
}

void setMyName(const String& n) {
  myName = n;
  strncpy(contacts[0].name, n.c_str(), sizeof(contacts[0].name) - 1);
  contacts[0].name[sizeof(contacts[0].name) - 1] = 0;
  prefs.putString("name", n);
}

void addTrackPoint(Contact* c, int32_t lat, int32_t lon) {
  if (c->trkCount) {
    int last = (c->trkHead + TRACK_MAX - 1) % TRACK_MAX;
    if (c->trk[last].lat == lat && c->trk[last].lon == lon) return;
  }
  TrackPt& p = c->trk[c->trkHead];
  p.lat = lat; p.lon = lon; p.t = nowEpoch();
  c->trkHead = (c->trkHead + 1) % TRACK_MAX;
  if (c->trkCount < TRACK_MAX) c->trkCount++;
}

void setContactLoc(Contact* c, int32_t lat, int32_t lon, uint16_t acc) {
  c->hasLoc = true; c->lat = lat; c->lon = lon; c->acc = acc; c->locAt = millis();
  addTrackPoint(c, lat, lon);
}

// ------------------------------ Message log --------------------------------
Msg      msgLog[MAX_MSGS];
int      logHead = 0;
uint32_t nextSeq = 1, revCounter = 1, clearCount = 0;

Msg* addMsg(char dir, uint8_t kind, uint32_t uid, uint16_t id) {
  Msg* m = &msgLog[logHead];
  logHead = (logHead + 1) % MAX_MSGS;
  *m = Msg();
  m->seq = nextSeq++;
  m->rev = ++revCounter;
  m->dir = dir; m->kind = kind; m->uid = uid; m->id = id;
  m->t = nowEpoch();
  return m;
}
void touch(Msg* m) { m->rev = ++revCounter; }

Msg* findMsg(uint32_t uid, uint16_t id, char dir) {
  for (int i = 0; i < MAX_MSGS; i++)
    if (msgLog[i].seq && msgLog[i].dir == dir && msgLog[i].uid == uid && msgLog[i].id == id) return &msgLog[i];
  return nullptr;
}

void sysLine(const String& text) {
  Msg* m = addMsg('S', 0, myUid, 0);
  m->text = text;
}

uint16_t newIds(int n) {
  if (msgCounter == 0) msgCounter = 1;
  uint16_t base = msgCounter;
  msgCounter += n;
  return base;
}

// ------------------------------ Outgoing engine ----------------------------
OutItem outQ[MAX_QUEUE];
int     qCount = 0;

Pending pend;

PingState pingPend;

Deferred defQ[MAX_DEFER];

Fwd fwdQ[MAX_FWD];

RdEntry rdQ[MAX_READQ];
int      rdCount = 0;
uint32_t lastReadAt = 0;

bool enqueue(const OutItem& it) {
  if (qCount >= MAX_QUEUE) return false;
  if (it.prio) {
    for (int i = qCount; i > 0; i--) outQ[i] = outQ[i - 1];
    outQ[0] = it;
  } else {
    outQ[qCount] = it;
  }
  qCount++;
  return true;
}

bool deferSend(uint32_t delayMs, const uint8_t* plain, uint8_t len, uint8_t ch) {
  if (len > sizeof(defQ[0].buf)) return false;
  for (int i = 0; i < MAX_DEFER; i++) {
    if (!defQ[i].used) {
      defQ[i].used = true;
      defQ[i].due = millis() + delayMs;
      defQ[i].len = len;
      defQ[i].ch = ch;
      memcpy(defQ[i].buf, plain, len);
      return true;
    }
  }
  return false;
}

uint32_t ackTimeoutMs() {
  uint32_t t = (uint32_t)airtimeMs(HDR_LEN + 7 + TAG_LEN) + 600 + 350 + 500;
  for (int i = 1; i < MAX_CONTACTS; i++) if (contacts[i].used && contacts[i].relayOn) { t += 1500; break; }
  return t + random(0, 250);
}

void transmitPending() {
  OutItem& it = pend.it;
  uint8_t b[200];
  int n = 0;
  b[n++] = 'M';
  b[n++] = it.kind;
  putU16(b + n, it.id); n += 2;
  b[n++] = it.fragIdx;
  b[n++] = it.fragTotal;
  b[n++] = it.flags;
  if (it.flags & 1) { putU32(b + n, it.refUid); n += 4; putU16(b + n, it.refId); n += 2; }
  if (it.flags & 2) { putU32(b + n, it.dst); n += 4; }
  memcpy(b + n, it.data, it.dlen);
  n += it.dlen;
  txCh = it.ch;
  sendSecure(b, n, DEFAULT_TTL);
  txCh = 0;
  if (pend.tries > 0) cRetry++;
  pend.tries++;
  pend.sentAt = millis();
  pend.timeoutMs = ackTimeoutMs();
}

void failPending() {
  cFail++;
  uint16_t logId = pend.it.logId;
  pend.active = false;
  if (logId) {
    Msg* m = findMsg(myUid, logId, 'O');
    if (m && m->dir == 'O') { m->status = 2; touch(m); }
    int w = 0;
    for (int i = 0; i < qCount; i++) if (outQ[i].logId != logId) outQ[w++] = outQ[i];
    qCount = w;
  }
}

void onAck(uint16_t id, uint32_t from) {
  for (int i = 0; i < MAX_MSGS; i++) {
    Msg& m = msgLog[i];
    if (!m.seq || m.dir != 'O') continue;
    uint16_t d = (uint16_t)(id - m.id);
    uint8_t total = m.partsTotal ? m.partsTotal : 1;
    if (d >= total) continue;
    if (d == total - 1) {
      bool known = false;
      for (int k = 0; k < MAX_ACKBY; k++) if (m.ackBy[k] == from) known = true;
      if (!known) {
        for (int k = 0; k < MAX_ACKBY; k++) if (!m.ackBy[k]) { m.ackBy[k] = from; break; }
        if (m.acks < 250) m.acks++;
      }
    }
    m.partsMask |= (uint8_t)(1u << d);
    uint8_t full = (uint8_t)((1u << total) - 1);
    if ((m.partsMask & full) == full && m.status != 3) m.status = 1;
    touch(&m);
    break;
  }
  if (pend.active && pend.it.id == id) pend.active = false;
}

void serviceOutgoing() {
  if (!pend.active) {
    if (qCount > 0) {
      pend.it = outQ[0];
      for (int i = 1; i < qCount; i++) outQ[i - 1] = outQ[i];
      qCount--;
      pend.active = true;
      pend.tries = 0;
      pend.maxTries = (pend.it.kind == K_SOS) ? SOS_TRIES : MAX_TRIES;
      transmitPending();
    }
    return;
  }
  if (millis() - pend.sentAt >= pend.timeoutMs) {
    if (pend.tries < pend.maxTries) transmitPending();
    else failPending();
  }
}

void serviceDeferred() {
  for (int i = 0; i < MAX_DEFER; i++) {
    if (defQ[i].used && (int32_t)(millis() - defQ[i].due) >= 0) {
      defQ[i].used = false;
      txCh = defQ[i].ch;
      sendSecure(defQ[i].buf, defQ[i].len, DEFAULT_TTL);
      txCh = 0;
      return;                       // one transmission per loop pass
    }
  }
}

void serviceForward() {
  for (int i = 0; i < MAX_FWD; i++) {
    if (fwdQ[i].used && (int32_t)(millis() - fwdQ[i].due) >= 0) {
      fwdQ[i].used = false;
      if (meshMode && !mtDutyOk()) return;          // out of airtime budget: skip the rebroadcast
      txRaw(fwdQ[i].buf, fwdQ[i].len);
      cRelay++;
      return;
    }
  }
}

void scheduleForward(const uint8_t* w, int n, uint8_t newTtl) {
  for (int i = 0; i < MAX_FWD; i++) {
    if (!fwdQ[i].used) {
      fwdQ[i].used = true;
      fwdQ[i].uid = getU32(w + 1);
      fwdQ[i].ctr = getU32(w + 5);
      fwdQ[i].due = millis() + random(250, 800);
      fwdQ[i].len = n;
      memcpy(fwdQ[i].buf, w, n);
      fwdQ[i].buf[0] = 0xA0 | (newTtl & 0x0F);
      return;
    }
  }
}

void cancelForward(uint32_t uid, uint32_t ctr) {
  for (int i = 0; i < MAX_FWD; i++)
    if (fwdQ[i].used && fwdQ[i].uid == uid && fwdQ[i].ctr == ctr) { fwdQ[i].used = false; cSupp++; }
}

void markVisible() {
  unread = 0;
  for (int i = 0; i < MAX_MSGS; i++) {
    Msg& m = msgLog[i];
    if (!m.seq || m.dir != 'I' || m.readSent || m.deleted || m.kind > K_SOS || m.ch) continue;
    m.readSent = true;
    if (!meshMode && rdCount < MAX_READQ) { rdQ[rdCount].uid = m.uid; rdQ[rdCount].id = m.id; rdCount++; }
  }
}

void serviceReads() {
  if (!rdCount || millis() - lastReadAt < 3000) return;
  uint8_t b[2 + 5 * 6];
  int n = (rdCount > 5) ? 5 : rdCount;
  b[0] = 'D';
  b[1] = n;
  for (int i = 0; i < n; i++) { putU32(b + 2 + i * 6, rdQ[i].uid); putU16(b + 6 + i * 6, rdQ[i].id); }
  for (int i = n; i < rdCount; i++) rdQ[i - n] = rdQ[i];
  rdCount -= n;
  deferSend(random(300, 900), b, 2 + n * 6, 0);
  lastReadAt = millis();
}

// ------------------------------ Ping ---------------------------------------
void startPing() {
  if (pingPend.active) return;
  if (meshMode) {                                   // Meshtastic: ask nearby nodes to announce themselves
    pingPend.id = 0;
    pingPend.active = true;
    pingPend.start = millis();
    pingPend.replies = 0;
    pingPend.timeoutMs = 20000;
    mtSendNodeInfo(MT_BROADCAST, true);
    sysLine("Ping sent: asking Meshtastic nodes in range to announce themselves...");
    return;
  }
  pingPend.id = newIds(1);
  pingPend.active = true;
  pingPend.start = millis();
  pingPend.replies = 0;
  pingPend.timeoutMs = (uint32_t)(airtimeMs(HDR_LEN + 3 + TAG_LEN) + airtimeMs(HDR_LEN + 10 + TAG_LEN) + 2500);
  uint8_t b[3];
  b[0] = 'P';
  putU16(b + 1, pingPend.id);
  sendSecure(b, 3, DEFAULT_TTL);
  sysLine("Ping sent, waiting for replies...");
}

void servicePing() {
  if (pingPend.active && millis() - pingPend.start > pingPend.timeoutMs) {
    pingPend.active = false;
    if (pingPend.replies == 0) sysLine("Ping got no reply (peer out of range, powered off, or on a different frequency/key)");
  }
}

// ------------------------------ Beacon -------------------------------------
void sendBeacon() {
  if (meshMode) {                                   // Meshtastic: periodic NodeInfo announcement
    mtSendNodeInfo(MT_BROADCAST, false);
    lastBeaconAt = millis();
    nextBeaconAt = millis() + 1800000UL + random(0, 60000);
    return;
  }
  uint8_t chunks[1][FRAG_DATA + 8];
  uint8_t lens[1];
  String nm = clipUtf8(myName, 20);
  if (buildChunks(nm, chunks, lens, 1) != 1) { nm = "Node"; buildChunks(nm, chunks, lens, 1); }
  uint8_t b[48];
  int n = 0;
  b[n++] = 'B';
  b[n++] = relayOn ? 1 : 0;
  b[n++] = lens[0];
  memcpy(b + n, chunks[0], lens[0]);
  n += lens[0];
  for (uint8_t k = 0; k <= MAX_CH; k++) {
    if (!ctxFor(k)) continue;
    txCh = k;
    sendSecure(b, n, DEFAULT_TTL);
  }
  txCh = 0;
  lastBeaconAt = millis();
  nextBeaconAt = millis() + BEACON_MS + random(0, 15000);
}

// ------------------------------ Incoming messages --------------------------
uint64_t seenKeys[48];
int      seenHead = 0;
bool seenBefore(uint32_t uid, uint16_t id) {
  uint64_t k = ((uint64_t)uid << 16) | id;
  for (int i = 0; i < 48; i++) if (seenKeys[i] == k) return true;
  seenKeys[seenHead] = k;
  seenHead = (seenHead + 1) % 48;
  return false;
}

Assembly asmb[MAX_ASM];

Assembly* getAsm(uint32_t uid, uint16_t base, uint8_t total) {
  for (int i = 0; i < MAX_ASM; i++) if (asmb[i].used && asmb[i].uid == uid && asmb[i].baseId == base) return &asmb[i];
  int slot = -1;
  for (int i = 0; i < MAX_ASM; i++) if (!asmb[i].used) { slot = i; break; }
  if (slot < 0) {
    slot = 0;
    for (int i = 1; i < MAX_ASM; i++) if (asmb[i].started < asmb[slot].started) slot = i;
  }
  Assembly& a = asmb[slot];
  a.used = true; a.uid = uid; a.baseId = base; a.total = total; a.mask = 0;
  for (int k = 0; k < MAX_FRAGS; k++) a.part[k] = "";
  a.hasReply = false; a.rUid = 0; a.rId = 0; a.started = millis();
  return &a;
}

void serviceAssemblies() {
  for (int i = 0; i < MAX_ASM; i++)
    if (asmb[i].used && millis() - asmb[i].started > 120000UL) asmb[i].used = false;
}

Msg* newIncoming(Contact* c, uint8_t kind, uint16_t id, int rssi, int snr) {
  Msg* m = addMsg('I', kind, c->uid, id);
  m->rssi = rssi;
  m->snr = snr;
  m->dst = curDst;
  m->ch = rxCh;
  unread++;
  return m;
}

void queueAck(uint32_t uid, uint16_t id, uint8_t ch) {
  uint8_t b[7];
  b[0] = 'A';
  putU32(b + 1, uid);
  putU16(b + 5, id);
  deferSend(random(250, 600), b, 7, ch);
}

void handleM(Contact* c, const uint8_t* p, int len, int rssi, int snr) {
  if (len < 6) return;
  uint8_t kind = p[0];
  uint16_t id = getU16(p + 1);
  uint8_t fi = p[3], ft = p[4], flags = p[5];
  int off = 6;
  uint32_t rUid = 0;
  uint16_t rId = 0;
  bool hasReply = (flags & 1) != 0;
  if (hasReply) {
    if (len < off + 6) return;
    rUid = getU32(p + off);
    rId = getU16(p + off + 4);
    off += 6;
  }
  uint32_t dst = 0;
  if (flags & 2) {
    if (len < off + 4) return;
    dst = getU32(p + off);
    off += 4;
  }
  if (dst && dst != myUid) return;            // private message for another node: ignore, do not ack
  curDst = dst;
  const uint8_t* data = p + off;
  int dlen = len - off;

  queueAck(c->uid, id, rxCh);                       // always acknowledge (retries too)
  if (seenBefore(c->uid, id)) { cDup++; return; }

  if (kind == K_TEXT) {
    String part = decodeBytes(data, dlen);
    if (ft <= 1) {
      Msg* m = newIncoming(c, K_TEXT, id, rssi, snr);
      m->text = part;
      if (hasReply) { m->hasReply = true; m->rUid = rUid; m->rId = rId; }
      ledUntil = millis() + 120;
      return;
    }
    if (ft > MAX_FRAGS || fi >= ft) return;
    uint16_t base = (uint16_t)(id - fi);
    Assembly* a = getAsm(c->uid, base, ft);
    a->part[fi] = part;
    a->mask |= (uint8_t)(1u << fi);
    a->rssi = rssi; a->snr = snr;
    if (hasReply) { a->hasReply = true; a->rUid = rUid; a->rId = rId; }
    uint8_t full = (uint8_t)((1u << a->total) - 1);
    if ((a->mask & full) == full) {
      String all;
      for (int k = 0; k < a->total; k++) all += a->part[k];
      Msg* m = newIncoming(c, K_TEXT, base, a->rssi, a->snr);
      m->text = all;
      if (a->hasReply) { m->hasReply = true; m->rUid = a->rUid; m->rId = a->rId; }
      a->used = false;
      ledUntil = millis() + 120;
    }
  } else if (kind == K_QUICK) {
    if (dlen < 1) return;
    Msg* m = newIncoming(c, K_QUICK, id, rssi, snr);
    m->text = String((int)data[0]);
    if (hasReply) { m->hasReply = true; m->rUid = rUid; m->rId = rId; }
    ledUntil = millis() + 120;
  } else if (kind == K_LOC) {
    if (dlen < 10) return;
    int32_t la = (int32_t)getU32(data), lo = (int32_t)getU32(data + 4);
    uint16_t ac = getU16(data + 8);
    Msg* m = newIncoming(c, K_LOC, id, rssi, snr);
    m->hasLoc = true; m->lat = la; m->lon = lo; m->acc = ac;
    setContactLoc(c, la, lo, ac);
    ledUntil = millis() + 120;
  } else if (kind == K_SOS) {
    if (dlen < 11) return;
    Msg* m = newIncoming(c, K_SOS, id, rssi, snr);
    if (data[0] & 1) {
      int32_t la = (int32_t)getU32(data + 1), lo = (int32_t)getU32(data + 5);
      uint16_t ac = getU16(data + 9);
      m->hasLoc = true; m->lat = la; m->lon = lo; m->acc = ac;
      setContactLoc(c, la, lo, ac);
    }
  } else if (kind == K_EDIT) {
    if (dlen < 2) return;
    Msg* m = findMsg(c->uid, getU16(data), 'I');
    if (m && m->dir == 'I' && m->kind == K_TEXT && !m->deleted) {
      m->text = decodeBytes(data + 2, dlen - 2);
      m->edited = true;
      touch(m);
    }
  } else if (kind == K_DEL) {
    if (dlen < 2) return;
    Msg* m = findMsg(c->uid, getU16(data), 'I');
    if (m && m->dir == 'I') { m->deleted = true; m->text = ""; touch(m); }
  }
}

void handleAck(Contact* c, const uint8_t* p, int len) {
  if (len < 6) return;
  if (getU32(p) != myUid) return;
  onAck(getU16(p + 4), c->uid);
}

void handleRead(Contact* c, const uint8_t* p, int len) {
  if (len < 1) return;
  int n = p[0];
  for (int i = 0; i < n && 1 + (i + 1) * 6 <= len; i++) {
    uint32_t uid = getU32(p + 1 + i * 6);
    uint16_t id = getU16(p + 5 + i * 6);
    if (uid != myUid) continue;
    Msg* m = findMsg(myUid, id, 'O');
    if (m && m->dir == 'O') {
      bool known = false;
      for (int k = 0; k < MAX_ACKBY; k++) if (m->readBy[k] == c->uid) known = true;
      if (!known) {
        for (int k = 0; k < MAX_ACKBY; k++) if (!m->readBy[k]) { m->readBy[k] = c->uid; break; }
        if (m->reads < 250) m->reads++;
      }
      m->status = 3;
      touch(m);
    }
  }
}

void handlePing(Contact* c, const uint8_t* p, int len, int rssi, int snr) {
  if (len < 2) return;
  uint8_t b[10];
  b[0] = 'R';
  putU32(b + 1, c->uid);
  b[5] = p[0]; b[6] = p[1];
  putU16(b + 7, (uint16_t)(int16_t)rssi);
  b[9] = (uint8_t)(int8_t)snr;
  deferSend(random(250, 650), b, 10, 0);
  sysLine("Ping from " + String(c->name) + " received (" + String(rssi) + " dBm, " + qualityFa(rssi) + ")");
}

void handlePong(Contact* c, const uint8_t* p, int len, int rssi, int snr) {
  if (len < 9) return;
  if (getU32(p) != myUid) return;
  uint16_t id = getU16(p + 4);
  if (!(pingPend.active && id == pingPend.id)) return;
  pingPend.replies++;
  int remRssi = (int16_t)getU16(p + 6);
  int remSnr = (int8_t)p[8];
  sysLine("Reply from " + String(c->name) + " | out: " + String(remRssi) + " dBm, SNR " + String(remSnr) +
          " (" + qualityFa(remRssi) + ") | back: " + String(rssi) + " dBm, SNR " + String(snr) +
          " (" + qualityFa(rssi) + ")");
}

void handleLive(Contact* c, const uint8_t* p, int len) {
  if (len < 10) return;
  setContactLoc(c, (int32_t)getU32(p), (int32_t)getU32(p + 4), getU16(p + 8));
}

void handleBeacon(Contact* c, const uint8_t* p, int len) {
  if (len < 2) return;
  c->relayOn = (p[0] & 1) != 0;
  int nl = p[1];
  if (nl > len - 2) nl = len - 2;
  String nm = clipUtf8(decodeBytes(p + 2, nl), 39);
  if (nm.length()) { strncpy(c->name, nm.c_str(), sizeof(c->name) - 1); c->name[sizeof(c->name) - 1] = 0; }
}

// ------------------------------ Meshtastic ---------------------------------
#include "meshtastic.h"

// ------------------------------ Radio receive ------------------------------
void handleWire(uint8_t* w, int n, int rssi, float snrF) {
  cRx++;
  if (n < HDR_LEN + 1 + TAG_LEN || (w[0] & 0xF0) != 0xA0) { cForeign++; return; }
  uint8_t ttl = w[0] & 0x0F;
  uint32_t uid = getU32(w + 1), ctr = getU32(w + 5);
  if (uid == myUid) return;                         // our own packet echoed by a repeater
  int clen = n - HDR_LEN - TAG_LEN;
  uint8_t plain[MAX_RAW];
  uint8_t iv[12];
  memset(iv, 0, sizeof(iv));
  putU32(iv, uid);
  putU32(iv + 4, ctr);
  int rc = -1;
  rxCh = 0;
  for (uint8_t k = 0; k <= MAX_CH && rc != 0; k++) {
    mbedtls_gcm_context* g = ctxFor(k);
    if (!g) continue;
    rc = mbedtls_gcm_auth_decrypt(g, clen, iv, 12, w + 1, 8, w + HDR_LEN + clen, TAG_LEN,
                                  w + HDR_LEN, plain);
    if (rc == 0) rxCh = k;
  }
  if (rc != 0) { cAuth++; return; }                 // wrong key or tampered

  Contact* c = findContact(uid);
  if (!c) c = addContact(uid);
  if (ctr <= c->lastCtr) {                          // replayed / duplicate copy
    cReplay++;
    if (rssi > -100) cancelForward(uid, ctr);       // a neighbour already repeated it loudly: skip ours
    return;
  }     // replayed / duplicate copy
  c->lastCtr = ctr;
  cOk++;
  int snr = (int)lroundf(snrF);
  c->lastSeen = millis() ? millis() : 1;
  c->rssi = rssi;
  c->snr = snr;
  c->rxCount++;
  avgRssi = (avgRssi * 7.0f + rssi) / 8.0f;

  const uint8_t* p = plain + 1;
  int plen = clen - 1;
  switch (plain[0]) {
    case 'M': handleM(c, p, plen, rssi, snr); break;
    case 'A': handleAck(c, p, plen); break;
    case 'D': handleRead(c, p, plen); break;
    case 'P': handlePing(c, p, plen, rssi, snr); break;
    case 'R': handlePong(c, p, plen, rssi, snr); break;
    case 'L': handleLive(c, p, plen); break;
    case 'B': handleBeacon(c, p, plen); break;
    default: break;
  }
  if (relayOn && ttl > 1) scheduleForward(w, n, ttl - 1);
}

void pollRadio() {
  int sz = LoRa.parsePacket();
  if (sz <= 0) return;
  uint8_t buf[MAX_RAW];
  int n = 0;
  while (LoRa.available() && n < MAX_RAW) buf[n++] = (uint8_t)LoRa.read();
  if (meshMode) mtHandle(buf, n, LoRa.packetRssi(), LoRa.packetSnr());
  else handleWire(buf, n, LoRa.packetRssi(), LoRa.packetSnr());
}

// ==========================================================================
//  Web server
// ==========================================================================

void jerr(int code, const String& msg) {
  server.send(code, "application/json", "{\"err\":" + jstr(msg) + "}");
}
void jokx(const String& extra) {
  server.send(200, "application/json", "{\"ok\":1" + extra + "}");
}
void jok() { jokx(""); }

String meJson() {
  String o = "{\"u\":\"" + hex8(myUid) + "\",\"n\":" + jstr(myName);
  o += ",\"fp\":\"" + keyFp() + "\"";
  o += ",\"kd\":"; o += keyIsDefault ? "1" : "0";
  o += ",\"wd\":"; o += wifiDefault ? "1" : "0";
  o += ",\"ssid\":" + jstr(apSsid);
  o += ",\"fq\":" + String(freqKhz);
  o += ",\"rl\":"; o += relayOn ? "1" : "0";
  o += ",\"tk\":"; o += timeKnown ? "1" : "0";
  o += ",\"pr\":" + String(preset) + ",\"pw\":" + String(txPower);
  o += ",\"cp\":"; o += captiveOn ? "1" : "0";
  o += ",\"mt\":"; o += meshMode ? "1" : "0";
  o += ",\"mp\":" + String(mtPreset) + ",\"mn\":" + jstr(mtEffName()) + ",\"mc\":" + jstr(mtChName);
  o += ",\"mk\":" + jstr(mtKeyKind()) + ",\"mh\":" + String(mtHop) + ",\"ms\":" + String(mtSlot);
  o += ",\"mo\":" + String(mtFreqOvrKhz) + ",\"mf\":" + String(mtCurFreqHz()) + ",\"mx\":" + String(mtHash);
  o += ",\"chs\":[";
  bool f1 = true;
  for (int i = 0; i < MAX_CH; i++) {
    if (!chUsed[i] || meshMode) continue;
    if (!f1) o += ",";
    f1 = false;
    o += "{\"i\":" + String(i + 1) + ",\"n\":" + jstr(String(chName[i])) + ",\"fp\":\"" + chanFp(i) + "\"}";
  }
  o += "]";
  o += ",\"q\":" + String(qCount + (pend.active ? 1 : 0) + mtPending());
  o += "}";
  return o;
}

String contactJson(const Contact& c, bool me) {
  String o = "{\"u\":\"" + hex8(c.uid) + "\",\"n\":" + jstr(String(c.name));
  o += ",\"me\":"; o += me ? "1" : "0";
  long ls = me ? 0 : (c.lastSeen ? (long)((millis() - c.lastSeen) / 1000UL) : -1);
  o += ",\"ls\":" + String(ls);
  o += ",\"rs\":" + String(c.rssi);
  o += ",\"sn\":" + String(c.snr);
  o += ",\"rx\":" + String(c.rxCount);
  o += ",\"rl\":"; o += (me ? relayOn : c.relayOn) ? "1" : "0";
  o += ",\"hl\":"; o += c.hasLoc ? "1" : "0";
  o += ",\"la\":" + String(c.lat);
  o += ",\"lo\":" + String(c.lon);
  o += ",\"ac\":" + String(c.acc);
  o += ",\"lt\":" + String(c.hasLoc ? (long)((millis() - c.locAt) / 1000UL) : 0L);
  o += ",\"tp\":" + String(c.trkCount);
  o += "}";
  return o;
}

String msgJson(const Msg& m) {
  String name;
  if (m.dir == 'O') name = myName;
  else { Contact* c = findContact(m.uid); name = c ? String(c->name) : String("?"); }
  String o;
  o.reserve(220 + m.text.length());
  o += "{\"q\":" + String(m.seq);
  o += ",\"r\":" + String(m.rev);
  o += ",\"d\":\""; o += m.dir; o += "\"";
  o += ",\"k\":" + String(m.kind);
  o += ",\"u\":\"" + hex8(m.uid) + "\"";
  o += ",\"n\":" + jstr(name);
  o += ",\"i\":" + String(m.id);
  o += ",\"s\":" + String(m.status);
  o += ",\"a\":" + String(m.acks);
  o += ",\"rd\":" + String(m.reads);
  o += ",\"pt\":" + String(m.partsTotal);
  o += ",\"pm\":" + String(m.partsMask);
  o += ",\"rs\":" + String(m.rssi);
  o += ",\"sn\":" + String(m.snr);
  o += ",\"t\":" + String(m.t);
  o += ",\"e\":"; o += m.edited ? "1" : "0";
  o += ",\"x\":"; o += m.deleted ? "1" : "0";
  o += ",\"hl\":"; o += m.hasLoc ? "1" : "0";
  o += ",\"la\":" + String(m.lat);
  o += ",\"lo\":" + String(m.lon);
  o += ",\"ac\":" + String(m.acc);
  o += ",\"tx\":" + jstr(m.text);
  o += ",\"to\":\"" + hex8(m.dst) + "\",\"ch\":" + String(m.ch);
  if (m.dir == 'O') {
    o += ",\"ab\":[";
    bool fa = true;
    for (int k = 0; k < MAX_ACKBY; k++) if (m.ackBy[k]) { if (!fa) o += ","; fa = false; o += "\"" + hex8(m.ackBy[k]) + "\""; }
    o += "],\"rb\":[";
    fa = true;
    for (int k = 0; k < MAX_ACKBY; k++) if (m.readBy[k]) { if (!fa) o += ","; fa = false; o += "\"" + hex8(m.readBy[k]) + "\""; }
    o += "]";
  }
  if (m.hasReply) o += ",\"rp\":{\"u\":\"" + hex8(m.rUid) + "\",\"i\":" + String(m.rId) + "}";
  o += "}";
  return o;
}

void handleRoot() { server.send_P(200, "text/html; charset=UTF-8", PAGE_HTML); }
void handleManifest() { server.send_P(200, "application/manifest+json", MANIFEST_JSON); }
void handleSw() { server.send_P(200, "application/javascript", SW_JS); }
void handleIcon192() { server.send_P(200, "image/png", (const char*)ICON192, ICON192_LEN); }
void handleIcon512() { server.send_P(200, "image/png", (const char*)ICON512, ICON512_LEN); }

void handlePoll() {
  uint32_t cRev = (uint32_t)server.arg("rev").toInt();
  uint32_t cClr = (uint32_t)server.arg("clr").toInt();
  if (server.hasArg("t")) {
    uint32_t t = (uint32_t)strtoul(server.arg("t").c_str(), nullptr, 10);
    if (t > 1600000000UL) { timeBase = t - millis() / 1000UL; timeKnown = true; }
  }
  if (server.arg("vis") == "1") markVisible();
  if (cClr != clearCount) cRev = 0;

  String o;
  o.reserve(2048);
  o += "{\"rev\":" + String(revCounter) + ",\"clr\":" + String(clearCount);
  o += ",\"me\":" + meJson();
  o += ",\"msgs\":[";
  bool first = true;
  for (int i = 0; i < MAX_MSGS; i++) {
    const Msg& m = msgLog[(logHead + i) % MAX_MSGS];
    if (!m.seq || m.rev <= cRev) continue;
    if (!first) o += ",";
    first = false;
    o += msgJson(m);
  }
  o += "],\"contacts\":[";
  first = true;
  for (int i = 0; i < MAX_CONTACTS; i++) {
    if (!contacts[i].used) continue;
    if (!first) o += ",";
    first = false;
    o += contactJson(contacts[i], i == 0);
  }
  o += "]}";
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", o);
}

bool readTarget(uint8_t* ch, uint32_t* dst) {
  *ch = 0;
  *dst = 0;
  if (server.hasArg("ch") && server.arg("ch").toInt() > 0) {
    int c = server.arg("ch").toInt();
    if (c > MAX_CH || !chUsed[c - 1]) { jerr(404, "Channel not found"); return false; }
    *ch = (uint8_t)c;
  } else if (server.hasArg("to") && server.arg("to").length()) {
    uint32_t u = (uint32_t)strtoul(server.arg("to").c_str(), nullptr, 16);
    if (u == myUid || !findContact(u)) { jerr(404, "Node not found"); return false; }
    *dst = u;
  }
  return true;
}

void handleSend() {
  String t = clipUtf8(sanitize(server.arg("text")), MAX_TEXT_BYTES);
  if (!t.length()) { jerr(400, "Message is empty"); return; }
  if (meshMode) {
    uint8_t tch;
    uint32_t tdst;
    if (!readTarget(&tch, &tdst)) return;
    if (tch) { jerr(400, "Private channels are not available in Meshtastic mode"); return; }
    bool hr = server.hasArg("ru") && server.hasArg("ri");
    uint32_t ru = hr ? (uint32_t)strtoul(server.arg("ru").c_str(), nullptr, 16) : 0;
    uint16_t ri = hr ? (uint16_t)server.arg("ri").toInt() : 0;
    String err;
    uint16_t id = 0;
    int rc = mtSendMsg(K_TEXT, t, t, tdst, hr, ru, ri, false, err, id);
    if (rc) { jerr(rc, err); return; }
    jokx(",\"id\":" + String(id));
    return;
  }
  uint8_t chunks[MAX_FRAGS][FRAG_DATA + 8];
  uint8_t lens[MAX_FRAGS];
  int n = buildChunks(t, chunks, lens, MAX_FRAGS);
  if (n <= 0) { jerr(413, "Message is too long"); return; }
  if (qCount + n > MAX_QUEUE) { jerr(429, "Send queue is full, wait a moment"); return; }
  uint8_t tch;
  uint32_t tdst;
  if (!readTarget(&tch, &tdst)) return;
  uint16_t base = newIds(n);
  Msg* m = addMsg('O', K_TEXT, myUid, base);
  m->ch = tch;
  m->dst = tdst;
  m->text = t;
  m->partsTotal = n;
  if (server.hasArg("ru") && server.hasArg("ri")) {
    m->hasReply = true;
    m->rUid = (uint32_t)strtoul(server.arg("ru").c_str(), nullptr, 16);
    m->rId = (uint16_t)server.arg("ri").toInt();
  }
  for (int i = 0; i < n; i++) {
    OutItem it;
    memset(&it, 0, sizeof(it));
    it.kind = K_TEXT;
    it.id = (uint16_t)(base + i);
    it.fragIdx = i;
    it.fragTotal = n;
    it.logId = base;
    if (i == 0 && m->hasReply) { it.flags = 1; it.refUid = m->rUid; it.refId = m->rId; }
    it.ch = tch;
    if (tdst) { it.flags |= 2; it.dst = tdst; }
    memcpy(it.data, chunks[i], lens[i]);
    it.dlen = lens[i];
    enqueue(it);
  }
  jokx(",\"id\":" + String(base));
}

void handleQuick() {
  int code = server.arg("code").toInt();
  if (code < 0 || code > 63) { jerr(400, "bad code"); return; }
  if (meshMode) {
    uint8_t tch;
    uint32_t tdst;
    if (!readTarget(&tch, &tdst)) return;
    if (tch) { jerr(400, "Private channels are not available in Meshtastic mode"); return; }
    if (code > 7) { jerr(400, "Quick reply not available in Meshtastic mode"); return; }
    String err;
    uint16_t id = 0;
    int rc = mtSendMsg(K_QUICK, String(code), String(MT_QUICK[code]), tdst, false, 0, 0, false, err, id);
    if (rc) { jerr(rc, err); return; }
    jok();
    return;
  }
  if (qCount + 1 > MAX_QUEUE) { jerr(429, "Send queue is full"); return; }
  uint8_t tch;
  uint32_t tdst;
  if (!readTarget(&tch, &tdst)) return;
  uint16_t id = newIds(1);
  Msg* m = addMsg('O', K_QUICK, myUid, id);
  m->ch = tch;
  m->dst = tdst;
  m->text = String(code);
  m->partsTotal = 1;
  OutItem it;
  memset(&it, 0, sizeof(it));
  it.kind = K_QUICK; it.id = id; it.fragTotal = 1; it.logId = id;
  it.data[0] = (uint8_t)code; it.dlen = 1;
  it.ch = tch;
  if (tdst) { it.flags |= 2; it.dst = tdst; }
  enqueue(it);
  jok();
}

void handleEdit() {
  if (meshMode) { jerr(400, "Editing is not supported in Meshtastic mode"); return; }
  uint16_t id = (uint16_t)server.arg("id").toInt();
  String t = clipUtf8(sanitize(server.arg("text")), MAX_TEXT_BYTES);
  Msg* m = findMsg(myUid, id, 'O');
  if (!m || m->dir != 'O' || m->kind != K_TEXT || m->deleted) { jerr(404, "Message not found"); return; }
  if (!t.length()) { jerr(400, "Text is empty"); return; }
  uint8_t chunks[MAX_FRAGS][FRAG_DATA + 8];
  uint8_t lens[MAX_FRAGS];
  if (buildChunks(t, chunks, lens, 1) != 1) { jerr(413, "Only short single-packet text can be edited"); return; }
  if (qCount + 1 > MAX_QUEUE) { jerr(429, "Send queue is full"); return; }
  OutItem it;
  memset(&it, 0, sizeof(it));
  it.kind = K_EDIT; it.id = newIds(1); it.fragTotal = 1;
  it.ch = m->ch;
  if (m->dst) { it.flags |= 2; it.dst = m->dst; }
  putU16(it.data, id);
  memcpy(it.data + 2, chunks[0], lens[0]);
  it.dlen = 2 + lens[0];
  enqueue(it);
  m->text = t; m->edited = true; touch(m);
  jok();
}

void handleDel() {
  if (meshMode) { jerr(400, "Deleting is not supported in Meshtastic mode"); return; }
  uint16_t id = (uint16_t)server.arg("id").toInt();
  Msg* m = findMsg(myUid, id, 'O');
  if (!m || m->dir != 'O' || m->deleted) { jerr(404, "Message not found"); return; }
  if (qCount + 1 > MAX_QUEUE) { jerr(429, "Send queue is full"); return; }
  OutItem it;
  memset(&it, 0, sizeof(it));
  it.kind = K_DEL; it.id = newIds(1); it.fragTotal = 1;
  it.ch = m->ch;
  if (m->dst) { it.flags |= 2; it.dst = m->dst; }
  putU16(it.data, id);
  it.dlen = 2;
  enqueue(it);
  m->deleted = true; m->text = ""; touch(m);
  jok();
}

bool readCoords(int32_t* la, int32_t* lo, uint16_t* acc) {
  if (!server.hasArg("lat") || !server.hasArg("lon")) return false;
  double lat = server.arg("lat").toDouble(), lon = server.arg("lon").toDouble();
  if (fabs(lat) > 90.0 || fabs(lon) > 180.0) return false;
  long a = server.arg("acc").toInt();
  if (a < 0) a = 0;
  if (a > 65535) a = 65535;
  *la = (int32_t)lround(lat * 1e6);
  *lo = (int32_t)lround(lon * 1e6);
  *acc = (uint16_t)a;
  return true;
}

void handleLoc() {
  int32_t la, lo;
  uint16_t acc;
  if (!readCoords(&la, &lo, &acc)) { jerr(400, "Invalid coordinates"); return; }
  setContactLoc(&contacts[0], la, lo, acc);
  if (meshMode) {
    if (server.arg("mode") == "live") {               // keep airtime low: one position per minute
      static uint32_t lastMtLive = 0;
      if ((lastMtLive == 0 || millis() - lastMtLive >= 60000UL) && mtFree() > 0) {
        mtSendPosition(la, lo, MT_BROADCAST, 0, 0);
        lastMtLive = millis();
      }
      jok();
      return;
    }
    uint8_t tch;
    uint32_t tdst;
    if (!readTarget(&tch, &tdst)) return;
    if (tch) { jerr(400, "Private channels are not available in Meshtastic mode"); return; }
    if (mtFree() < 1) { jerr(429, "Send queue is full"); return; }
    uint32_t pid = mtNewIds(1);
    Msg* m = addMsg('O', K_LOC, myUid, (uint16_t)pid);
    m->pid = pid; m->dst = tdst; m->partsTotal = 1;
    m->hasLoc = true; m->lat = la; m->lon = lo; m->acc = acc;
    mtSendPosition(la, lo, tdst ? tdst : MT_BROADCAST, pid, (uint16_t)pid);
    jok();
    return;
  }
  if (server.arg("mode") == "live") {
    static uint32_t lastLive = 0;
    if (millis() - lastLive >= 5000UL || lastLive == 0) {
      uint8_t b[11];
      b[0] = 'L';
      putU32(b + 1, (uint32_t)la);
      putU32(b + 5, (uint32_t)lo);
      putU16(b + 9, acc);
      sendSecure(b, 11, DEFAULT_TTL);
      lastLive = millis();
    }
    jok();
    return;
  }
  if (qCount + 1 > MAX_QUEUE) { jerr(429, "Send queue is full"); return; }
  uint8_t tch;
  uint32_t tdst;
  if (!readTarget(&tch, &tdst)) return;
  uint16_t id = newIds(1);
  Msg* m = addMsg('O', K_LOC, myUid, id);
  m->ch = tch;
  m->dst = tdst;
  m->partsTotal = 1; m->hasLoc = true; m->lat = la; m->lon = lo; m->acc = acc;
  OutItem it;
  memset(&it, 0, sizeof(it));
  it.kind = K_LOC; it.id = id; it.fragTotal = 1; it.logId = id;
  putU32(it.data, (uint32_t)la);
  putU32(it.data + 4, (uint32_t)lo);
  putU16(it.data + 8, acc);
  it.dlen = 10;
  it.ch = tch;
  if (tdst) { it.flags |= 2; it.dst = tdst; }
  enqueue(it);
  jok();
}

void handleSos() {
  int32_t la = 0, lo = 0;
  uint16_t acc = 0;
  bool has = readCoords(&la, &lo, &acc);
  if (meshMode) {                                    // Meshtastic has no SOS packet: broadcast text + position
    String err;
    uint16_t sid = 0;
    String w = has ? "SOS! I need help - my position follows" : "SOS! I need help";
    int rc = mtSendMsg(K_SOS, "", w, 0, false, 0, 0, true, err, sid);
    if (rc) { jerr(rc, err); return; }
    if (has) {
      Msg* sm = findMsg(myUid, sid, 'O');
      if (sm) { sm->hasLoc = true; sm->lat = la; sm->lon = lo; sm->acc = acc; }
      setContactLoc(&contacts[0], la, lo, acc);
      if (mtFree() > 0) mtSendPosition(la, lo, MT_BROADCAST, 0, 0);
    }
    jok();
    return;
  }
  if (qCount + 1 > MAX_QUEUE) { jerr(429, "Send queue is full"); return; }
  uint16_t id = newIds(1);
  Msg* m = addMsg('O', K_SOS, myUid, id);
  m->partsTotal = 1;
  if (has) { m->hasLoc = true; m->lat = la; m->lon = lo; m->acc = acc; setContactLoc(&contacts[0], la, lo, acc); }
  OutItem it;
  memset(&it, 0, sizeof(it));
  it.kind = K_SOS; it.id = id; it.fragTotal = 1; it.logId = id; it.prio = true;
  it.data[0] = has ? 1 : 0;
  putU32(it.data + 1, (uint32_t)la);
  putU32(it.data + 5, (uint32_t)lo);
  putU16(it.data + 9, acc);
  it.dlen = 11;
  enqueue(it);
  jok();
}

void handleApiPing() { startPing(); jok(); }

void handleCfg() {
  bool restart = false;
  if (server.hasArg("name")) {
    String n = clipUtf8(sanitize(server.arg("name")), 24);
    if (!n.length()) { jerr(400, "Name is empty"); return; }
    setMyName(n);
  }
  if (server.hasArg("pass") && server.arg("pass").length()) {
    String p = server.arg("pass");
    if (p.length() < 6) { jerr(400, "Passphrase must be at least 6 characters"); return; }
    uint8_t k[16];
    deriveKey(p, k);
    applyKey(k);
    prefs.putBytes("key", k, 16);
    keyIsDefault = false;
    for (int i = 1; i < MAX_CONTACTS; i++) contacts[i].lastCtr = 0;
  }
  if (server.hasArg("ssid") || server.hasArg("wpass")) {
    String s = server.hasArg("ssid") ? sanitize(server.arg("ssid")) : apSsid;
    String w = server.hasArg("wpass") ? server.arg("wpass") : apPass;
    if (s.length() < 1 || s.length() > 31) { jerr(400, "Network name must be 1-31 characters"); return; }
    if (w.length() < 8 || w.length() > 63) { jerr(400, "Wi-Fi password must be 8-63 characters"); return; }
    prefs.putString("ssid", s);
    prefs.putString("wpass", w);
    restart = true;
  }
  if (server.hasArg("freq")) {
    uint32_t f = (uint32_t)strtoul(server.arg("freq").c_str(), nullptr, 10);
    if (f < FREQ_MIN_KHZ || f > FREQ_MAX_KHZ) { jerr(400, "Frequency out of range"); return; }
    freqKhz = f;
    prefs.putULong("freq", f);
    applyRadio();
  }
  if (server.hasArg("relay")) {
    relayOn = server.arg("relay") == "1";
    prefs.putBool("relay", relayOn);
    nextBeaconAt = millis() + 1500;
  }
  if (server.hasArg("preset")) {
    int p = server.arg("preset").toInt();
    if (p < 0 || p > 3) { jerr(400, "Bad radio preset"); return; }
    preset = p;
    prefs.putInt("preset", p);
    applyRadio();
  }
  if (server.hasArg("txp")) {
    int p = server.arg("txp").toInt();
    if (p < 2 || p > 20) { jerr(400, "TX power must be 2-20 dBm"); return; }
    txPower = p;
    prefs.putInt("txp", p);
    applyRadio();
  }
  if (server.hasArg("cap")) {
    captiveOn = server.arg("cap") == "1";
    prefs.putBool("cap", captiveOn);
    if (captiveOn) dnsServer.start(53, "*", WiFi.softAPIP()); else dnsServer.stop();
  }
  // ---- Meshtastic settings
  bool mtChanged = false;
  if (server.hasArg("mtpre")) {
    int p = server.arg("mtpre").toInt();
    if (!mtPresetValid(p)) { jerr(400, "Bad Meshtastic preset"); return; }
    mtPreset = p; mtChanged = true;
  }
  if (server.hasArg("mtname")) {
    String n = sanitize(server.arg("mtname"));
    if (n.length() > 11) { jerr(400, "Channel name: 11 bytes at most"); return; }
    mtChName = n; mtChanged = true;
  }
  if (server.hasArg("mtpsk")) {
    if (!mtSetPsk(server.arg("mtpsk"))) { jerr(400, "Key must be base64 (16 or 32 bytes), 'default' or 'none'"); return; }
    mtChanged = true;
  }
  if (server.hasArg("mturl") && server.arg("mturl").length()) {
    if (!mtApplyUrl(server.arg("mturl"))) { jerr(400, "Not a valid Meshtastic channel link"); return; }
    mtChanged = true;
  }
  if (server.hasArg("mtslot")) {
    int sl = server.arg("mtslot").toInt();
    if (sl < 0 || sl > 99) { jerr(400, "Frequency slot must be 0 (auto) to 99"); return; }
    mtSlot = (uint8_t)sl; mtChanged = true;
  }
  if (server.hasArg("mthop")) {
    int hp = server.arg("mthop").toInt();
    if (hp < 1 || hp > 7) { jerr(400, "Hop limit must be 1-7"); return; }
    mtHop = (uint8_t)hp; mtChanged = true;
  }
  if (server.hasArg("mtfq")) {
    uint32_t f = (uint32_t)strtoul(server.arg("mtfq").c_str(), nullptr, 10);
    if (f && (f < FREQ_MIN_KHZ || f > FREQ_MAX_KHZ)) { jerr(400, "Frequency out of range"); return; }
    mtFreqOvrKhz = f; mtChanged = true;
  }
  if (server.hasArg("mode")) {
    bool mm = server.arg("mode") == "1";
    if (mm != meshMode) {
      meshMode = mm;
      mtResetState();
      nextBeaconAt = millis() + random(4000, 9000);
      mtChanged = true;
      mtRecalc();
      if (meshMode) sysLine("Meshtastic mode on: " + mtEffName() + ", " + String(mtCurFreqHz() / 1e6, 3) + " MHz. LoRa Link nodes cannot hear this node until you switch back.");
      else sysLine("LoRa Link mode on.");
    }
  }
  if (mtChanged) { mtRecalc(); mtSave(); applyRadio(); }
  jokx(String(",\"restart\":") + (restart ? "1" : "0"));
}

void handleChan() {
  int i = server.arg("slot").toInt() - 1;
  if (i < 0 || i >= MAX_CH) { jerr(400, "Bad channel slot"); return; }
  String kn = "chk" + String(i), nn = "chn" + String(i);
  if (server.arg("del") == "1") {
    chUsed[i] = false;
    prefs.remove(kn.c_str());
    prefs.remove(nn.c_str());
    for (int k = 0; k < MAX_MSGS; k++) if (msgLog[k].seq && msgLog[k].ch == i + 1) msgLog[k].seq = 0;
    clearCount++;
    revCounter++;
    jok();
    return;
  }
  String n = clipUtf8(sanitize(server.arg("name")), 20);
  String p = server.arg("pass");
  if (!n.length()) { jerr(400, "Channel name is empty"); return; }
  if (p.length() < 6) { jerr(400, "Passphrase must be at least 6 characters"); return; }
  uint8_t k[16];
  deriveKey(p, k);
  setChannel(i, n, k);
  prefs.putBytes(kn.c_str(), k, 16);
  prefs.putString(nn.c_str(), n);
  jok();
}

void handleNotFound() {
  if (captiveOn && server.method() == HTTP_GET && server.uri().indexOf("/api") != 0) {
    server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString() + "/", true);
    server.send(302, "text/plain", "");
    return;
  }
  server.send(404, "text/plain", "Not found");
}

void handleStats() {
  String o = "{";
  o += "\"up\":" + String(millis() / 1000UL);
  o += ",\"tx\":" + String(cTx) + ",\"rx\":" + String(cRx) + ",\"ok\":" + String(cOk);
  o += ",\"auth\":" + String(cAuth) + ",\"replay\":" + String(cReplay) + ",\"dup\":" + String(cDup);
  o += ",\"retry\":" + String(cRetry) + ",\"fail\":" + String(cFail) + ",\"relay\":" + String(cRelay) + ",\"supp\":" + String(cSupp);
  o += ",\"avg\":" + String((int)lroundf(avgRssi));
  o += ",\"air\":" + String(airHour());
  o += ",\"q\":" + String(qCount + (pend.active ? 1 : 0) + mtPending());
  o += ",\"heap\":" + String((uint32_t)ESP.getFreeHeap());
  o += "}";
  server.send(200, "application/json", o);
}

void handleScan() {
  String o = "{\"ch\":[";
  bool first = true;
  for (uint32_t k = 433050UL; k <= 434750UL; k += 100UL) {
    LoRa.setFrequency((long)k * 1000L);
    LoRa.receive();
    delay(25);
    int mx = -200;
    for (int s = 0; s < 8; s++) { int r = LoRa.rssi(); if (r > mx) mx = r; delay(4); }
    if (!first) o += ",";
    first = false;
    o += "{\"f\":" + String(k) + ",\"r\":" + String(mx) + "}";
  }
  LoRa.setFrequency(curFreqHz());
  LoRa.idle();
  o += "]}";
  server.send(200, "application/json", o);
}

void handleGpx() {
  String u = server.arg("u");
  Contact* c = (u == "me") ? &contacts[0] : findContact((uint32_t)strtoul(u.c_str(), nullptr, 16));
  if (!c || c->trkCount == 0) { server.send(404, "text/plain", "no track"); return; }
  String o;
  o.reserve(c->trkCount * 100 + 300);
  o += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  o += "<gpx version=\"1.1\" creator=\"LoRaLink\" xmlns=\"http://www.topografix.com/GPX/1/1\">\n";
  o += "<trk><name>" + xmlEsc(String(c->name)) + "</name><trkseg>\n";
  int start = (c->trkCount < TRACK_MAX) ? 0 : c->trkHead;
  for (int i = 0; i < c->trkCount; i++) {
    const TrackPt& p = c->trk[(start + i) % TRACK_MAX];
    char b[96];
    snprintf(b, sizeof(b), "<trkpt lat=\"%.6f\" lon=\"%.6f\">", p.lat / 1e6, p.lon / 1e6);
    o += b;
    if (p.t) {
      time_t tt = (time_t)p.t;
      struct tm tmv;
      gmtime_r(&tt, &tmv);
      char tb[32];
      strftime(tb, sizeof(tb), "%Y-%m-%dT%H:%M:%SZ", &tmv);
      o += "<time>"; o += tb; o += "</time>";
    }
    o += "</trkpt>\n";
  }
  o += "</trkseg></trk></gpx>\n";
  server.sendHeader("Content-Disposition", "attachment; filename=\"track-" + hex8(c->uid) + ".gpx\"");
  server.send(200, "application/gpx+xml", o);
}

void handleClear() {
  for (int i = 0; i < MAX_MSGS; i++) msgLog[i].seq = 0;
  logHead = 0;
  clearCount++;
  revCounter++;
  unread = 0;
  jok();
}

void handleResetCtr() {
  for (int i = 1; i < MAX_CONTACTS; i++) contacts[i].lastCtr = 0;
  jok();
}

void handleRestart() { jok(); restartAt = millis() + 800; if (!restartAt) restartAt = 1; }
void handleWipe() { jok(); wipeAt = millis() + 600; if (!wipeAt) wipeAt = 1; }

// ==========================================================================
//  Hardware button : short = ping, hold 5 s = wipe
// ==========================================================================
void serviceButton() {
#if USE_BUTTON
  static bool lastState = HIGH;
  static uint32_t pressedAt = 0, lastChange = 0;
  static bool wiped = false;
  bool s = digitalRead(BTN_PIN);
  uint32_t now = millis();
  if (s != lastState && now - lastChange > 40) {
    lastChange = now;
    lastState = s;
    if (s == LOW) { pressedAt = now; wiped = false; }
    else {
      ledWarn = false;
      uint32_t held = now - pressedAt;
      if (!wiped && held < 1500) startPing();
    }
  }
  if (lastState == LOW && !wiped) {
    uint32_t held = now - pressedAt;
    ledWarn = (held > 2000);
    if (held > 5000) {
      wiped = true;
      ledWarn = false;
      wipeAt = now + 300;
      if (!wipeAt) wipeAt = 1;
    }
  }
#endif
}

// ==========================================================================
//  Setup / loop
// ==========================================================================
void loadSettings() {
  myUid = (uint32_t)(ESP.getEfuseMac() >> 16);
  if (myUid < 4 || myUid == 0xFFFFFFFFUL) myUid ^= 0x5A5A5A5AUL;     // 0-3 and 0xFFFFFFFF are reserved by Meshtastic
  String defName = String("Node-") + hex8(myUid).substring(4);
  myName = prefs.getString("name", defName);
  apSsid = prefs.getString("ssid", String("LoRaLink-") + hex8(myUid).substring(4));
  wifiDefault = !prefs.isKey("wpass");
  apPass = prefs.getString("wpass", "12345678");
  freqKhz = prefs.getULong("freq", FREQ_DEFAULT_KHZ);
  if (freqKhz < FREQ_MIN_KHZ || freqKhz > FREQ_MAX_KHZ) freqKhz = FREQ_DEFAULT_KHZ;
  relayOn = prefs.getBool("relay", false);
  preset = prefs.getInt("preset", 0);
  if (preset < 0 || preset > 3) preset = 0;
  txPower = prefs.getInt("txp", LORA_TX_POWER);
  if (txPower < 2 || txPower > 20) txPower = LORA_TX_POWER;
  captiveOn = prefs.getBool("cap", true);
  epochNo = prefs.getUShort("epoch", 0) + 1;
  prefs.putUShort("epoch", epochNo);

  uint8_t k[16];
  if (prefs.isKey("key") && prefs.getBytes("key", k, 16) == 16) {
    keyIsDefault = false;
  } else {
    deriveKey("changeme", k);
    keyIsDefault = true;
  }
  applyKey(k);
  mtLoad();

  for (int i = 0; i < MAX_CONTACTS; i++) contacts[i].used = false;
  initContact(&contacts[0], myUid);
  strncpy(contacts[0].name, myName.c_str(), sizeof(contacts[0].name) - 1);
  contacts[0].name[sizeof(contacts[0].name) - 1] = 0;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LORA_SS, OUTPUT);
  digitalWrite(LORA_SS, HIGH);
#if USE_BUTTON
  pinMode(BTN_PIN, INPUT_PULLUP);
#endif
  prefs.begin("lora3", false);
  mbedtls_gcm_init(&gcm);
  loadSettings();
  loadChannels();
  msgCounter = (uint16_t)(esp_random() % 60000) + 1;

  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSsid.c_str(), apPass.c_str(), 1, 0, 4);
  Serial.printf("WiFi: %s  ->  http://%s\n", apSsid.c_str(), WiFi.softAPIP().toString().c_str());

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(curFreqHz())) {
    Serial.println("LoRa init failed. Check wiring.");
    while (true) { digitalWrite(LED_PIN, !digitalRead(LED_PIN)); delay(120); }
  }
  applyRadio();                       // modem settings for the active mode (LoRa Link or Meshtastic)

  server.on("/", HTTP_GET, handleRoot);
  server.on("/manifest.webmanifest", HTTP_GET, handleManifest);
  server.on("/sw.js", HTTP_GET, handleSw);
  server.on("/icon-192.png", HTTP_GET, handleIcon192);
  server.on("/icon-512.png", HTTP_GET, handleIcon512);
  server.on("/gpx", HTTP_GET, handleGpx);
  server.on("/api/poll", HTTP_GET, handlePoll);
  server.on("/api/stats", HTTP_GET, handleStats);
  server.on("/api/send", HTTP_POST, handleSend);
  server.on("/api/quick", HTTP_POST, handleQuick);
  server.on("/api/edit", HTTP_POST, handleEdit);
  server.on("/api/del", HTTP_POST, handleDel);
  server.on("/api/loc", HTTP_POST, handleLoc);
  server.on("/api/sos", HTTP_POST, handleSos);
  server.on("/api/ping", HTTP_POST, handleApiPing);
  server.on("/api/cfg", HTTP_POST, handleCfg);
  server.on("/api/scan", HTTP_POST, handleScan);
  server.on("/api/clear", HTTP_POST, handleClear);
  server.on("/api/resetctr", HTTP_POST, handleResetCtr);
  server.on("/api/chan", HTTP_POST, handleChan);
  server.on("/api/restart", HTTP_POST, handleRestart);
  server.on("/api/wipe", HTTP_POST, handleWipe);
  server.onNotFound(handleNotFound);
  server.begin();
  if (captiveOn) dnsServer.start(53, "*", WiFi.softAPIP());

  nextBeaconAt = millis() + random(3000, 8000);
  if (meshMode) Serial.printf("Meshtastic mode: %s  %.3f MHz\n", mtEffName().c_str(), mtCurFreqHz() / 1e6);
}

void loop() {
  if (captiveOn) dnsServer.processNextRequest();
  server.handleClient();
  pollRadio();
  serviceOutgoing();
  serviceDeferred();
  serviceForward();
  mtService();
  serviceReads();
  servicePing();
  serviceAssemblies();
  serviceButton();

  if ((int32_t)(millis() - nextBeaconAt) >= 0) sendBeacon();

  if (restartAt && (int32_t)(millis() - restartAt) >= 0) ESP.restart();
  if (wipeAt && (int32_t)(millis() - wipeAt) >= 0) {
    prefs.clear();
    delay(100);
    ESP.restart();
  }

  // LED: short pulse on traffic, slow blink while there are unread messages,
  // fast blink while the wipe button is held
  bool on = ((int32_t)(ledUntil - millis()) > 0) || (unread > 0 && ((millis() / 400) & 1)) ||
            (ledWarn && ((millis() / 100) & 1));
  digitalWrite(LED_PIN, on);

  delay(1);
}
