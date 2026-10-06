// assets.h - embedded web app, service worker, manifest and PWA icons.
// Keep this file in the same folder as lora_messenger_v6.ino
#pragma once
#include <Arduino.h>

const char PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en" dir="ltr">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover,interactive-widget=resizes-content">
<meta name="theme-color" content="#070914">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="mobile-web-app-capable" content="yes">
<link rel="manifest" href="/manifest.webmanifest">
<link rel="icon" href="/icon-192.png">
<link rel="apple-touch-icon" href="/icon-192.png">
<title>LoRa Link</title>
<style>
:root{--bg:#070914;--tx:#eef1ff;--mu:#8a93b8;--a:#00c8ff;--p:#7a5cff;--g:#28ff78;--r:#ff4d5e;--y:#ffd040;--gl:rgba(22,28,72,.55);--gl2:rgba(255,255,255,.06);--bd:rgba(255,255,255,.11);--inp:rgba(8,11,36,.55);--sh:0 10px 34px rgba(0,0,0,.38);--grad:linear-gradient(135deg,#00c8ff,#7a5cff)}
:root{--bgo:rgba(7,9,20,.94)}[data-theme=light]{--bg:#e8ecfa;--tx:#141a3d;--mu:#5d6794;--gl:rgba(255,255,255,.74);--gl2:rgba(30,40,110,.07);--bd:rgba(60,72,150,.16);--inp:rgba(255,255,255,.92);--sh:0 8px 26px rgba(50,62,140,.14);--bgo:rgba(232,236,250,.95)}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
html,body{margin:0;height:100%}
body{font-family:Inter,system-ui,-apple-system,"Segoe UI",Roboto,sans-serif;background:var(--bg);color:var(--tx);font-size:15px;overflow:hidden;-webkit-font-smoothing:antialiased}
.orb{position:fixed;border-radius:50%;pointer-events:none;will-change:transform;animation:fl 16s ease-in-out infinite alternate}
.o1{width:520px;height:520px;top:-200px;left:-170px;background:radial-gradient(circle,rgba(58,76,255,.55),transparent 65%)}
.o2{width:480px;height:480px;bottom:-190px;right:-150px;background:radial-gradient(circle,rgba(150,60,255,.5),transparent 65%);animation-delay:-6s}
.o3{width:340px;height:340px;top:40%;left:50%;background:radial-gradient(circle,rgba(0,200,255,.28),transparent 65%);animation-delay:-11s}
[data-theme=light] .orb{opacity:.6}
@keyframes fl{to{transform:translate3d(46px,64px,0) scale(1.18)}}
svg.i{width:20px;height:20px;fill:none;stroke:currentColor;stroke-width:1.9;stroke-linecap:round;stroke-linejoin:round;flex:none}
#app{position:relative;z-index:1;display:flex;flex-direction:column;height:100dvh;max-width:760px;margin:0 auto;padding:env(safe-area-inset-top,0px) 10px 0}
.card,header,nav,#bar,#panel,#replyBar,#quick,#sheet,#toast{background:var(--gl);-webkit-backdrop-filter:blur(18px) saturate(150%);backdrop-filter:blur(18px) saturate(150%);border:1px solid var(--bd);box-shadow:var(--sh),inset 0 1px 0 rgba(255,255,255,.08)}
header{display:flex;align-items:center;gap:12px;padding:10px 12px;margin-top:8px;border-radius:22px}
.logo{width:40px;height:40px;border-radius:13px;background:var(--grad);display:grid;place-items:center;color:#fff;box-shadow:0 0 26px rgba(0,200,255,.4);flex:none}
.ht{flex:1;min-width:0}
#hname{font-weight:700;font-size:16px;letter-spacing:.2px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
#hsub{font-size:12px;color:var(--mu);margin-top:1px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
#dot{width:10px;height:10px;border-radius:50%;background:var(--r);flex:none;transition:.3s}
#dot.on{background:var(--g);box-shadow:0 0 0 0 rgba(40,255,120,.6);animation:pulse 2s infinite}
@keyframes pulse{70%{box-shadow:0 0 0 9px rgba(40,255,120,0)}100%{box-shadow:0 0 0 0 rgba(40,255,120,0)}}
#banner{display:flex;flex-direction:column;gap:6px;margin-top:8px}
#banner:empty{display:none}
.bn{padding:9px 14px;font-size:12.5px;border-radius:14px;background:rgba(255,120,60,.16);border:1px solid rgba(255,138,61,.45);color:#ffd9bf;animation:fade .3s}
.bn.info{background:rgba(0,200,255,.14);border-color:rgba(0,200,255,.4);color:#c9f3ff}
[data-theme=light] .bn{color:#7a3300}[data-theme=light] .bn.info{color:#00506b}
main{flex:1;min-height:0;position:relative;margin-top:8px}
.view{display:none;position:absolute;inset:0;overflow-y:auto;scrollbar-width:none}
.view::-webkit-scrollbar{display:none}
.view.active{display:flex;flex-direction:column;animation:fade .3s}
@keyframes fade{from{opacity:0;transform:translateY(6px)}}
#v-chat{overflow:hidden}
#chat{flex:1;overflow-y:auto;padding:6px 2px 8px;display:flex;flex-direction:column;gap:7px;scrollbar-width:none}
#chat::-webkit-scrollbar{display:none}
.empty{margin:auto;color:var(--mu);text-align:center;white-space:pre-line;line-height:1.7}
.row{display:flex}
.row.mine{justify-content:flex-end}
.row.theirs{justify-content:flex-start}
.row.in .bub{animation:pop .38s cubic-bezier(.2,.9,.3,1.25)}
@keyframes pop{from{opacity:0;transform:translateY(14px) scale(.92)}}
.bub{max-width:82%;padding:9px 13px 6px;border-radius:20px;line-height:1.5;overflow-wrap:anywhere;box-shadow:0 6px 18px rgba(0,0,0,.25);cursor:pointer;transition:transform .15s}
.bub:active{transform:scale(.975)}
.mine .bub{background:linear-gradient(135deg,#0ea5e9,#6d4cff);color:#fff;border-bottom-right-radius:6px}
.theirs .bub{background:var(--gl);-webkit-backdrop-filter:blur(14px);backdrop-filter:blur(14px);border:1px solid var(--bd);border-bottom-left-radius:6px}
.bub.sos{background:linear-gradient(135deg,#b91c2e,#ff4d5e);color:#fff;border:1px solid #ff8a93;animation:sosp 1.4s infinite}
@keyframes sosp{50%{box-shadow:0 0 28px rgba(255,77,94,.7)}}
.who{font-size:12px;color:var(--a);font-weight:700;margin-bottom:1px}
.txt{white-space:pre-wrap;unicode-bidi:plaintext;text-align:start}
.quick{font-weight:700}
.quote{font-size:12.5px;opacity:.85;border-left:3px solid var(--a);padding:3px 9px;margin:2px 0 5px;border-radius:4px;background:rgba(255,255,255,.1);unicode-bidi:plaintext}
.meta{display:flex;gap:8px;font-size:11px;opacity:.72;margin-top:3px;justify-content:flex-end;flex-wrap:wrap;align-items:center}
.st.read{color:#7ff0ff;opacity:1;font-weight:700}
.st.fail{color:#ffd1d6;font-weight:700}
.st.pend::before{content:"";display:inline-block;width:9px;height:9px;margin-right:4px;border:2px solid currentColor;border-top-color:transparent;border-radius:50%;animation:spin .8s linear infinite;vertical-align:-1px}
@keyframes spin{to{transform:rotate(360deg)}}
.sys{align-self:center;max-width:94%;text-align:center;font-size:12px;color:var(--mu);background:var(--gl2);border:1px solid var(--bd);padding:5px 13px;border-radius:99px;animation:fade .3s}
.muted{color:var(--mu);font-size:12.5px}
.bub .muted{color:inherit;opacity:.75}
.sos-t{font-weight:800;letter-spacing:.3px}
.loc{margin-top:3px}
.loc-t{font-weight:600;font-variant-numeric:tabular-nums;direction:ltr}
.loc-i{margin:4px 0;font-size:13px;display:flex;align-items:center;gap:6px}
.loc-b{display:flex;gap:6px;margin-top:5px}
.arr{display:inline-block;font-size:19px;font-weight:700;color:var(--a);transition:transform .3s}
.mine .arr{color:#fff}
.chip{display:inline-flex;align-items:center;gap:5px;padding:5px 13px;border-radius:99px;background:var(--gl2);color:var(--tx);text-decoration:none;font-size:13px;border:1px solid var(--bd);cursor:pointer;font-family:inherit;transition:.15s}
.chip:active{transform:scale(.95)}
a.chip{color:var(--a)}
.mine a.chip{color:#fff}
#replyBar,#quick,#panel{display:none;margin-top:8px;border-radius:18px;animation:up .25s cubic-bezier(.2,.9,.3,1)}
@keyframes up{from{opacity:0;transform:translateY(14px)}}
#replyBar{align-items:center;gap:8px;padding:8px 12px;font-size:13px}
#replyBar.on{display:flex}
#replyBar .rt{flex:1;min-width:0;overflow:hidden;text-overflow:ellipsis;white-space:nowrap;border-left:3px solid var(--a);padding-left:9px;color:var(--mu)}
#quick{gap:7px;padding:9px 10px;overflow-x:auto;scrollbar-width:none}
#quick.on{display:flex}
#quick .chip{white-space:nowrap;flex:none}
#panel{padding:10px;gap:8px;flex-wrap:wrap}
#panel.on{display:flex}
.pb{flex:1 1 44%;display:flex;align-items:center;gap:10px;padding:12px;background:var(--gl2);border:1px solid var(--bd);border-radius:16px;color:var(--tx);font:inherit;font-size:14px;cursor:pointer;transition:.15s}
.pb:active{transform:scale(.97);background:rgba(122,92,255,.2)}
.pb svg{color:var(--a)}
.pb.dg{color:var(--r);border-color:rgba(255,77,94,.5)}.pb.dg svg{color:var(--r)}
.pb select{margin-left:auto;padding:3px 6px;font-size:12px}
#bar{display:flex;gap:8px;align-items:center;padding:7px;margin-top:8px;border-radius:26px}
#txt{flex:1;min-width:0;padding:11px 14px;border-radius:20px;border:1px solid transparent;background:var(--inp);color:var(--tx);font:inherit;outline:none;transition:.2s}
#txt:focus{border-color:var(--a);box-shadow:0 0 0 3px rgba(0,200,255,.18)}
.ib{width:42px;height:42px;border-radius:50%;border:1px solid var(--bd);background:var(--gl2);color:var(--tx);cursor:pointer;flex:none;display:grid;place-items:center;transition:.18s}
.ib:active{transform:scale(.88)}
.ib.go{background:var(--grad);border:0;color:#fff;box-shadow:0 0 20px rgba(0,200,255,.4)}
nav{display:flex;gap:4px;padding:6px;margin:10px 0 calc(10px + env(safe-area-inset-bottom,0px));border-radius:24px}
nav button{flex:1;background:none;border:0;color:var(--mu);padding:8px 0 6px;font:inherit;font-size:11.5px;cursor:pointer;display:flex;flex-direction:column;align-items:center;gap:3px;border-radius:18px;transition:.25s}
nav button.on{color:#fff;background:var(--grad);box-shadow:0 6px 20px rgba(0,200,255,.3);font-weight:600}
.pad{padding:2px 0 14px}
.card{border-radius:20px;padding:14px 16px;margin-bottom:10px;animation:fade .3s}
.cr{display:flex;align-items:center;gap:8px;flex-wrap:wrap}
.cn{font-weight:700;font-size:16px}
.od{width:9px;height:9px;border-radius:50%;background:var(--mu)}
.od.on{background:var(--g);box-shadow:0 0 10px var(--g)}
.tag{font-size:11px;padding:2px 9px;border-radius:99px;background:var(--gl2);border:1px solid var(--bd);color:var(--mu)}
.tag.g{color:var(--g)}.tag.y{color:var(--y)}.tag.r{color:var(--r)}
details.card{padding:0;overflow:hidden}
details.card>summary{list-style:none;display:flex;align-items:center;gap:10px;padding:14px 16px;font-weight:600;cursor:pointer}
details.card>summary::-webkit-details-marker{display:none}
details.card>summary svg:first-child{color:var(--a)}
details.card>summary .chev{margin-left:auto;color:var(--mu);transition:transform .25s}
details[open]>summary .chev{transform:rotate(180deg)}
details .body{padding:0 16px 16px;animation:fade .3s}
label{display:block;font-size:12.5px;color:var(--mu);margin:12px 0 5px}
input[type=text],input[type=password],input[type=number],select{width:100%;padding:10px 12px;border-radius:12px;border:1px solid var(--bd);background:var(--inp);color:var(--tx);font:inherit;outline:none;transition:.2s}
input:focus,select:focus{border-color:var(--a);box-shadow:0 0 0 3px rgba(0,200,255,.15)}
select option{background:#10142e;color:#eef1ff}
.btn{padding:10px 16px;border-radius:12px;border:1px solid var(--bd);background:var(--gl2);color:var(--tx);font:inherit;font-size:14px;cursor:pointer;transition:.18s}
.btn:active{transform:scale(.96)}
.btn.pr{background:var(--grad);border:0;color:#fff;font-weight:600;box-shadow:0 4px 16px rgba(0,200,255,.28)}
.btn.dg{color:var(--r);border-color:rgba(255,77,94,.5)}
.btn:disabled{opacity:.5}
.btns{display:flex;gap:8px;flex-wrap:wrap;margin-top:12px}
.sw{display:flex;align-items:center;justify-content:space-between;margin-top:12px}
input[type=checkbox]{appearance:none;-webkit-appearance:none;width:46px;height:26px;border-radius:99px;background:var(--gl2);border:1px solid var(--bd);position:relative;cursor:pointer;transition:.25s;flex:none}
input[type=checkbox]::after{content:"";position:absolute;top:2px;left:2px;width:20px;height:20px;border-radius:50%;background:#fff;transition:.25s;box-shadow:0 2px 6px rgba(0,0,0,.3)}
input[type=checkbox]:checked{background:var(--grad);border-color:transparent}
input[type=checkbox]:checked::after{transform:translateX(20px)}
.hint{font-size:12.5px;color:var(--mu);margin-top:8px;line-height:1.65}
.hint b{color:var(--tx)}
ol.steps{margin:8px 0 0;padding-left:20px;font-size:13px;line-height:1.75;color:var(--mu)}
ol.steps b{color:var(--tx)}
code{font-family:ui-monospace,Menlo,Consolas,monospace;font-size:12px;background:var(--gl2);border:1px solid var(--bd);padding:1px 6px;border-radius:6px;color:var(--a);word-break:break-all}
.kv{display:flex;align-items:center;justify-content:space-between;gap:8px;margin-top:8px;font-size:13px}
#scanbox{display:flex;align-items:flex-end;gap:3px;height:92px;margin-top:12px}
#scanbox div{flex:1;background:rgba(122,92,255,.55);border-radius:4px 4px 0 0;min-height:3px;cursor:pointer;transition:height .5s}
#scanbox div.q{background:var(--g)}
#scanlbl{display:flex;justify-content:space-between;font-size:10.5px;color:var(--mu);margin-top:3px}
table.t{width:100%;border-collapse:collapse;font-size:13px}
table.t td{padding:6px 0;border-bottom:1px solid var(--bd)}
table.t td:last-child{text-align:right;color:var(--a);font-weight:600;font-variant-numeric:tabular-nums}
#shade{display:none;position:fixed;inset:0;background:rgba(3,5,20,.55);-webkit-backdrop-filter:blur(6px);backdrop-filter:blur(6px);z-index:20;align-items:flex-end;justify-content:center;animation:fadeo .2s}
@keyframes fadeo{from{opacity:0}}
#shade.on{display:flex}
#shade.mid{align-items:center}
#sheet{width:100%;max-width:560px;border-radius:26px 26px 0 0;padding:16px 16px calc(16px + env(safe-area-inset-bottom,0px));display:flex;flex-direction:column;gap:9px;animation:up .3s cubic-bezier(.2,.9,.3,1)}
#sheet.center{border-radius:26px;margin:auto 14px;animation:pop .35s cubic-bezier(.2,.9,.3,1.25)}
#sheet h4{margin:0;font-size:17px}
#sheet p{margin:0;color:var(--mu);line-height:1.7;font-size:13.5px;white-space:pre-line}
#sheet .sb{padding:13px;border-radius:14px;border:1px solid var(--bd);background:var(--gl2);color:var(--tx);font:inherit;cursor:pointer;text-align:center;transition:.15s}
#sheet .sb:active{transform:scale(.97)}
#sheet .sb.dg{background:linear-gradient(135deg,#ff4d5e,#ff8a3d);color:#fff;border:0;font-weight:700}
#toast{position:fixed;left:50%;top:calc(14px + env(safe-area-inset-top,0px));transform:translate(-50%,-20px);padding:10px 18px;border-radius:99px;font-size:13px;opacity:0;pointer-events:none;transition:.3s;z-index:30;max-width:90%;text-align:center}
#toast.show{opacity:1;transform:translate(-50%,0)}
@media(prefers-reduced-motion:reduce){*{animation:none!important;transition:none!important}}

#app{height:var(--vh,100dvh);transform:translateY(var(--vt,0px));transition:height .2s ease-out}
html.tt *{transition:background-color .35s,color .35s,border-color .35s!important}
[data-theme=dark] .tmoon,[data-theme=light] .tsun{display:none}
[data-theme=light] .theirs .bub{background:#fff}
[data-theme=light] select option{background:#fff;color:#141a3d}
#clist .card,.sys{animation:none}
.bn{animation:none}
#chead,.citem,.ob{background:var(--gl);-webkit-backdrop-filter:blur(18px) saturate(150%);backdrop-filter:blur(18px) saturate(150%);border:1px solid var(--bd);box-shadow:var(--sh),inset 0 1px 0 rgba(255,255,255,.08)}
#v-chat{position:absolute;inset:0;z-index:6;display:flex;flex-direction:column;padding:env(safe-area-inset-top,0px) 10px 0;background:var(--bgo);-webkit-backdrop-filter:blur(22px);backdrop-filter:blur(22px);transform:translateX(104%);visibility:hidden;transition:transform .34s cubic-bezier(.22,.9,.3,1),visibility 0s .34s}
#v-chat.open{transform:none;visibility:visible;transition:transform .34s cubic-bezier(.22,.9,.3,1)}
#chead{display:flex;align-items:center;gap:10px;padding:7px 10px;margin-top:8px;border-radius:22px}
#cname{font-weight:700;font-size:16px}#csub{font-size:12px;color:var(--mu)}
#chatwrap{position:relative;flex:1;min-height:0;display:flex;margin-top:4px}
#chat{overscroll-behavior:contain}
#chat>:first-child{margin-top:auto}
#compose{position:relative;padding-bottom:calc(10px + env(safe-area-inset-bottom,0px))}
#panel,#quick{display:flex;position:absolute;left:0;right:0;bottom:calc(100% - 4px);margin:0;animation:none;opacity:0;transform:translateY(14px) scale(.96);transform-origin:bottom left;pointer-events:none;transition:opacity .2s,transform .28s cubic-bezier(.2,.9,.3,1.2);z-index:5}
#panel.on,#quick.on{opacity:1;transform:none;pointer-events:auto}
#replyBar{display:flex;max-height:0;opacity:0;margin:0;padding:0 12px;border-width:0;overflow:hidden;animation:none;box-shadow:none;transition:max-height .26s ease,opacity .2s,margin .26s,padding .26s}
#replyBar.on{max-height:56px;opacity:1;margin-top:8px;padding:8px 12px;border-width:1px}
#txt{resize:none;max-height:120px;line-height:1.4;overflow-y:auto;scrollbar-width:none;display:block;height:42px}
#btnSend,#btnQuick{transition:width .24s,opacity .2s,transform .26s cubic-bezier(.2,.9,.3,1.3),margin .24s}
#btnSend{width:0;opacity:0;transform:scale(.3);margin-left:-8px;border:0;padding:0}
#btnSend.show{width:42px;opacity:1;transform:none;margin-left:0}
#btnQuick.hide{width:0;opacity:0;transform:scale(.3);margin-left:-8px;border:0;padding:0;pointer-events:none}
.av{width:34px;height:34px;border-radius:50%;display:grid;place-items:center;color:#fff;font-weight:700;font-size:14px;flex:none;background:#7a5cff}
.av.big{width:54px;height:54px;font-size:19px}.av.g{background:var(--grad)!important}
.row.theirs{gap:7px;align-items:flex-end}.row.theirs .av{width:30px;height:30px;font-size:12px}
.dsep{align-self:center;margin:8px 0 2px;padding:3px 12px;border-radius:99px;font-size:12px;color:var(--mu);background:var(--gl2);border:1px solid var(--bd);-webkit-backdrop-filter:blur(8px);backdrop-filter:blur(8px)}
#toBottom{position:absolute;right:6px;bottom:8px;opacity:0;transform:scale(.6) translateY(12px);pointer-events:none;transition:.22s cubic-bezier(.2,.9,.3,1.3);z-index:3;background:var(--gl);-webkit-backdrop-filter:blur(14px);backdrop-filter:blur(14px);box-shadow:var(--sh);overflow:visible}
#toBottom.show{opacity:1;transform:none;pointer-events:auto}
.badge{min-width:21px;height:21px;padding:0 6px;border-radius:99px;background:var(--grad);color:#fff;font-size:11.5px;font-weight:700;display:none;place-items:center}
.badge.on{display:grid;animation:pop .3s}
#toBottom .badge{position:absolute;top:-8px;right:-4px}
.citem{display:flex;align-items:center;gap:13px;width:100%;padding:12px 14px;border-radius:22px;color:var(--tx);font:inherit;text-align:left;cursor:pointer;transition:transform .15s}
.citem:active{transform:scale(.98)}
.cm{flex:1;min-width:0}.cl{display:flex;align-items:center;justify-content:space-between;gap:8px}.cl+.cl{margin-top:3px}.cl b{font-size:16px}
.ct{font-size:12px;color:var(--mu)}.cp{font-size:13.5px;color:var(--mu);white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
#shade.out{animation:fadeout .2s forwards}@keyframes fadeout{to{opacity:0}}
#shade.out #sheet{transform:translateY(34px);opacity:0;transition:.2s}
#onb{position:fixed;inset:0;z-index:25;display:none;align-items:center;justify-content:center;padding:18px;background:var(--bgo);-webkit-backdrop-filter:blur(26px);backdrop-filter:blur(26px);overflow-y:auto}
#onb.on{display:flex;animation:fadeo .3s}
.ob{width:100%;max-width:420px;border-radius:28px;padding:26px 22px;margin:auto}
.ob .logo{margin:0 auto 14px;width:58px;height:58px;border-radius:19px}
.ob h2{margin:0 0 6px;text-align:center;font-size:21px}.ob p{margin:0 0 6px;text-align:center;color:var(--mu);font-size:13.5px;line-height:1.65}
.stp{display:none}.stp.on{display:block;animation:fade .35s}
.dots{display:flex;gap:6px;justify-content:center;margin-bottom:16px}.dots i{width:7px;height:7px;border-radius:99px;background:var(--bd);transition:.3s}.dots i.on{width:22px;background:var(--grad)}
.obskip{text-align:center;margin-top:12px}.lnk{background:none;border:0;color:var(--mu);font:inherit;font-size:13px;cursor:pointer;text-decoration:underline}

@media (min-width:640px) and (orientation:landscape){
#app{max-width:1280px;display:grid;grid-template-columns:minmax(300px,380px) 1fr;grid-template-rows:auto auto minmax(0,1fr) auto;column-gap:14px;padding:env(safe-area-inset-top,0px) 14px 0}
header,#banner,main,nav{grid-column:1}
header{grid-row:1}#banner{grid-row:2}main{grid-row:3}nav{grid-row:4}
#v-chat{grid-column:2;grid-row:1/5;position:relative;inset:auto;transform:none;visibility:visible;padding:8px 0 0;background:none;-webkit-backdrop-filter:none;backdrop-filter:none;z-index:1;transition:none;min-height:0}
#btnBack{display:none}.bub{max-width:68%}.citem.act{outline:2px solid var(--a);outline-offset:-2px}
#compose{padding-bottom:12px}
}
@media (hover:hover){.citem:hover,.chip:hover,.btn:hover,.pb:hover,#sheet .sb:hover,.ib:hover{filter:brightness(1.13)}.bub{user-select:text}}
@media (hover:none){.bub{-webkit-user-select:none;user-select:none}}
.bub{touch-action:pan-y}
#hlist{display:flex;flex-direction:column;gap:8px}
.citem .cp.dr{color:var(--r)}
.inst{display:flex;align-items:center;gap:12px;padding:12px 14px;border-radius:20px;margin-bottom:10px;background:var(--gl);border:1px solid var(--bd);box-shadow:var(--sh);-webkit-backdrop-filter:blur(18px);backdrop-filter:blur(18px)}
.inst .it{flex:1;min-width:0;font-size:12.5px;color:var(--mu)}.inst .it b{display:block;color:var(--tx);font-size:14.5px}
.inst .ico{width:40px;height:40px;border-radius:13px;background:var(--grad);display:grid;place-items:center;color:#fff;flex:none}
.inst .btn{padding:8px 14px}.inst .ib{width:34px;height:34px}
#sbar,#pinbar{display:flex;align-items:center;gap:8px;max-height:0;opacity:0;overflow:hidden;margin:0;padding:0 10px;border:0 solid var(--bd);border-radius:16px;background:var(--gl);transition:max-height .25s,opacity .2s,margin .25s,padding .25s;-webkit-backdrop-filter:blur(14px);backdrop-filter:blur(14px)}
#sbar.on,#pinbar.on{max-height:52px;opacity:1;margin-top:8px;padding:6px 10px;border-width:1px}
#sInput{flex:1;min-width:0;padding:8px 10px!important;width:auto!important}
#pinT{flex:1;min-width:0;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;font-size:13px;border-left:3px solid var(--a);padding-left:9px;cursor:pointer;color:var(--mu)}
#ment{display:flex;gap:7px;position:absolute;left:0;right:0;bottom:calc(100% - 4px);padding:8px;border-radius:16px;overflow-x:auto;opacity:0;transform:translateY(10px);pointer-events:none;transition:.2s;z-index:5;background:var(--gl);-webkit-backdrop-filter:blur(18px);backdrop-filter:blur(18px);border:1px solid var(--bd)}
#ment.on{opacity:1;transform:none;pointer-events:auto}
a.lk{color:var(--a);text-decoration:underline;word-break:break-all}.mine a.lk{color:#fff}
.mention{color:var(--a);font-weight:600}.mention.me{background:rgba(255,208,64,.25);border-radius:6px;padding:0 3px;color:var(--y)}.mine .mention{color:#fff}
.bub.ment{box-shadow:0 0 0 2px var(--y),0 6px 18px rgba(0,0,0,.25)}
@keyframes fl2{50%{box-shadow:0 0 0 3px var(--a)}}.row.flash .bub{animation:fl2 .6s 2}
.radar{color:var(--tx)}.radar svg{width:100%;max-width:340px;display:block;margin:6px auto 0}.radar text{fill:var(--tx);font-size:10px;font-family:inherit}
.cmp{display:flex;align-items:center;gap:14px;font-size:15px;justify-content:center;padding:6px 0 4px}.cmp .arr{font-size:84px;line-height:1}
.rtt{width:100%;border-collapse:collapse;font-size:12.5px;margin-top:10px}.rtt td,.rtt th{padding:5px 4px;border-bottom:1px solid var(--bd);text-align:right}.rtt td:first-child,.rtt th:first-child{text-align:left}
.pb2{height:6px;border-radius:99px;background:var(--gl2);overflow:hidden;margin-top:10px}.pb2 i{display:block;height:100%;background:var(--grad);width:0;transition:width .3s}
input[type=range]{padding:0!important;height:28px;background:none!important;border:0!important;box-shadow:none!important;accent-color:#00c8ff}
input[type=time]{color-scheme:dark}[data-theme=light] input[type=time]{color-scheme:light}
.chl{display:flex;align-items:center;gap:10px;padding:8px 0;border-bottom:1px solid var(--bd);font-size:13.5px}.chl span{flex:1}
</style>
</head>
<body>
<svg width="0" height="0" style="position:absolute" aria-hidden="true"><defs>
<symbol id="i-chat" viewBox="0 0 24 24"><path d="M21 12a8 8 0 0 1-11.5 7.2L4 21l1.8-5.2A8 8 0 1 1 21 12z"/></symbol>
<symbol id="i-users" viewBox="0 0 24 24"><circle cx="9" cy="8" r="3.5"/><path d="M2.5 20c0-3.6 2.9-6 6.5-6s6.5 2.4 6.5 6M16 4.6a3.5 3.5 0 0 1 0 6.8M18 14.3c2.2.7 3.5 2.6 3.5 5.7"/></symbol>
<symbol id="i-set" viewBox="0 0 24 24"><path d="M4 6h10M18 6h2M4 12h4M12 12h8M4 18h12M20 18h0"/><circle cx="16" cy="6" r="2"/><circle cx="10" cy="12" r="2"/><circle cx="18" cy="18" r="2"/></symbol>
<symbol id="i-radio" viewBox="0 0 24 24"><circle cx="12" cy="12" r="2"/><path d="M7.8 7.8a6 6 0 0 0 0 8.4M16.2 7.8a6 6 0 0 1 0 8.4M4.9 4.9a10 10 0 0 0 0 14.2M19.1 4.9a10 10 0 0 1 0 14.2"/></symbol>
<symbol id="i-plus" viewBox="0 0 24 24"><path d="M12 5v14M5 12h14"/></symbol>
<symbol id="i-send" viewBox="0 0 24 24"><path d="M22 2 11 13M22 2l-7 20-4-9-9-4z"/></symbol>
<symbol id="i-pin" viewBox="0 0 24 24"><path d="M12 21s7-6.2 7-11.5A7 7 0 0 0 5 9.5C5 14.8 12 21 12 21z"/><circle cx="12" cy="9.5" r="2.5"/></symbol>
<symbol id="i-bolt" viewBox="0 0 24 24"><path d="M13 2 4 14h7l-1 8 9-12h-7z"/></symbol>
<symbol id="i-pulse" viewBox="0 0 24 24"><path d="M3 12h4l3-8 4 16 3-8h4"/></symbol>
<symbol id="i-alert" viewBox="0 0 24 24"><path d="M12 3 2 20h20zM12 10v4M12 17.5v.1"/></symbol>
<symbol id="i-x" viewBox="0 0 24 24"><path d="M6 6l12 12M18 6 6 18"/></symbol>
<symbol id="i-user" viewBox="0 0 24 24"><circle cx="12" cy="8" r="4"/><path d="M4 21c0-4 3.6-6.5 8-6.5s8 2.5 8 6.5"/></symbol>
<symbol id="i-lock" viewBox="0 0 24 24"><rect x="5" y="11" width="14" height="10" rx="2.5"/><path d="M8 11V8a4 4 0 0 1 8 0v3"/></symbol>
<symbol id="i-wifi" viewBox="0 0 24 24"><path d="M2 9a15 15 0 0 1 20 0M5.5 12.5a10 10 0 0 1 13 0M9 16a5 5 0 0 1 6 0"/><circle cx="12" cy="19.5" r="1"/></symbol>
<symbol id="i-sun" viewBox="0 0 24 24"><circle cx="12" cy="12" r="4"/><path d="M12 2v2M12 20v2M2 12h2M20 12h2M4.9 4.9l1.4 1.4M17.7 17.7l1.4 1.4M4.9 19.1l1.4-1.4M17.7 6.3l1.4-1.4"/></symbol>
<symbol id="i-chart" viewBox="0 0 24 24"><path d="M4 20V10M10 20V4M16 20v-7M22 20H2"/></symbol>
<symbol id="i-trash" viewBox="0 0 24 24"><path d="M4 7h16M9 7V4h6v3M6 7l1 13h10l1-13"/></symbol>
<symbol id="i-gps" viewBox="0 0 24 24"><circle cx="12" cy="12" r="9"/><path d="M15.5 8.5 13.5 13.5 8.5 15.5 10.5 10.5z"/></symbol>
<symbol id="i-copy" viewBox="0 0 24 24"><rect x="9" y="9" width="11" height="11" rx="2"/><path d="M5 15V6a2 2 0 0 1 2-2h8"/></symbol>
<symbol id="i-moon" viewBox="0 0 24 24"><path d="M21 13.5A9 9 0 1 1 10.5 3a7 7 0 0 0 10.5 10.5z"/></symbol>
<symbol id="i-back" viewBox="0 0 24 24"><path d="M15 5l-7 7 7 7"/></symbol>
<symbol id="i-down" viewBox="0 0 24 24"><path d="M6 9l6 6 6-6M6 15l6 6 6-6" transform="translate(0 -3)"/></symbol>
<symbol id="i-search" viewBox="0 0 24 24"><circle cx="11" cy="11" r="7"/><path d="M20 20l-4-4"/></symbol>
<symbol id="i-clock" viewBox="0 0 24 24"><circle cx="12" cy="12" r="9"/><path d="M12 7v5l3 2"/></symbol>
<symbol id="i-dl" viewBox="0 0 24 24"><path d="M12 3v12M7 10l5 5 5-5M5 21h14"/></symbol>
<symbol id="i-chev" viewBox="0 0 24 24"><path d="M6 9l6 6 6-6"/></symbol>
</defs></svg>
<div class="orb o1"></div><div class="orb o2"></div><div class="orb o3"></div>
<div id="app">
  <header>
    <span class="logo"><svg class="i"><use href="#i-radio"/></svg></span>
    <div class="ht"><div id="hname">LoRa Link</div><div id="hsub">Connecting...</div></div>
    <span id="dot"></span>
    <button class="ib" id="btnTheme" title="Day / night"><svg class="i tsun"><use href="#i-sun"/></svg><svg class="i tmoon"><use href="#i-moon"/></svg></button>
  </header>
  <div id="banner"></div>
  <main>
    <section class="view active" id="v-home"><div class="pad"><div id="inst"></div><div id="hlist"></div></div></section>
    <section class="view" id="v-contacts"><div class="pad"><div id="radar"></div><div id="clist"></div></div></section>
    <section class="view" id="v-settings"><div class="pad">
<details class="card"><summary><svg class="i"><use href="#i-user"/></svg><span>Profile</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><label>Display name</label><input type="text" id="sName" maxlength="24"><div class="btns"><button class="btn pr" id="saveName">Save</button></div></div></details>
<details class="card" id="gpsCard" open><summary><svg class="i"><use href="#i-gps"/></svg><span>Location &amp; GPS</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><div class="hint">Browsers only share your phone's GPS with <b>secure (HTTPS)</b> pages. This app is served by the device over plain HTTP, so the browser blocks location until you mark this address as trusted. It is a one-time step on <b>Android Chrome</b>. iPhone/Safari has no such option, so use manual coordinates below.</div>
<div class="kv"><span>GPS status</span><span class="tag r" id="gpsState">--</span></div>
<div class="kv"><span>Current position</span><span class="muted" id="gpsMan">--</span></div>
<label>One-time setup (Android Chrome)</label>
<ol class="steps">
<li>Copy the flags address, paste it in Chrome's address bar and open it. <button class="chip" id="cpFlag"><svg class="i"><use href="#i-copy"/></svg>Copy</button></li>
<li>Find <b>Insecure origins treated as secure</b> and set it to <b>Enabled</b>.</li>
<li>In the text box type this address: <code id="gpsOrigin">http://192.168.4.1</code> <button class="chip" id="cpOrigin"><svg class="i"><use href="#i-copy"/></svg>Copy</button></li>
<li>Tap <b>Relaunch</b>, reconnect to the device Wi-Fi and reopen this page.</li>
<li>Make sure phone Location is on, then tap <b>Test GPS</b> below and choose <b>Allow</b>.</li>
</ol>
<div class="hint">Chrome does not let web pages open <code>chrome://</code> links, so the address must be pasted by hand.</div>
<label>Manual coordinates (works on any phone)</label>
<input type="text" id="gpsIn" placeholder="lat, lon  e.g. 35.6892, 51.3890" inputmode="decimal" autocomplete="off">
<div class="btns"><button class="btn pr" id="gpsTest">Test GPS</button><button class="btn" id="gpsSet">Use as my position</button><button class="btn" id="gpsSend">Send to chat</button></div>
<div class="hint">A manual position is used for distance, bearing and SOS until you reload the page.</div></div></details>
<details class="card"><summary><svg class="i"><use href="#i-lock"/></svg><span>Security</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><div class="hint">Key fingerprint: <b id="sFp">----</b><br>It must be identical on every device.</div><label>New shared passphrase</label><input type="password" id="sPass" placeholder="At least 6 characters" autocomplete="new-password"><div class="btns"><button class="btn pr" id="savePass">Set key</button><button class="btn" id="resetCtr">Reset counters</button></div><div class="hint">Messages are encrypted and authenticated with AES-128-GCM. If you wipe one device, tap Reset counters on the others.</div></div></details>
<details class="card"><summary><svg class="i"><use href="#i-radio"/></svg><span>Radio</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><label>Frequency (MHz)</label><input type="number" id="sFreq" step="0.025" min="433.05" max="434.79"><div class="sw"><span>Repeater (smart)</span><input type="checkbox" id="sRelay"></div><label>Radio preset</label><select id="sPreset"><option value="0">Balanced - SF10 / 125 kHz</option><option value="1">Long range - SF12 / 125 kHz</option><option value="2">Fast - SF7 / 125 kHz</option><option value="3">Fast wide - SF7 / 250 kHz</option></select><div class="hint" id="presetHint"></div><label>Transmit power: <b id="txpV">17</b> dBm</label><input type="range" id="sTxp" min="2" max="20" step="1"><div class="hint">Higher power drains the battery and may exceed the legal limit for the 433 MHz band where you live. Check your local rules.</div><div class="btns"><button class="btn pr" id="saveRadio">Apply</button><button class="btn" id="doScan">Scan channels</button></div><div id="scanbox"></div><div id="scanlbl"></div><div class="hint">All devices must share one frequency. Green bars are the quietest channels; tap a bar to use it. A scan keeps the radio busy for about two seconds.</div></div></details>
<details class="card" id="mtCard"><summary><svg class="i"><use href="#i-radio"/></svg><span>Meshtastic</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><div class="sw"><span>Meshtastic mode</span><input type="checkbox" id="sMtMode"></div><div class="hint" id="mtHint"></div><label>Channel link (optional)</label><input type="text" id="sMtUrl" placeholder="https://meshtastic.org/e/#..." autocomplete="off" autocapitalize="off" spellcheck="false"><div class="hint">Paste the share link of your Meshtastic channel (Meshtastic app &gt; Channels &gt; share). It sets the name, key, preset and hop limit in one go.</div><label>Modem preset</label><select id="sMtPre"><option value="0">LongFast - SF11 / 250 kHz (default)</option><option value="1">LongSlow - SF12 / 125 kHz</option><option value="7">LongModerate - SF11 / 125 kHz</option><option value="9">LongTurbo - SF11 / 500 kHz</option><option value="3">MediumSlow - SF10 / 250 kHz</option><option value="4">MediumFast - SF9 / 250 kHz</option><option value="5">ShortSlow - SF8 / 250 kHz</option><option value="6">ShortFast - SF7 / 250 kHz</option><option value="8">ShortTurbo - SF7 / 500 kHz</option></select><label>Channel name (empty = preset name)</label><input type="text" id="sMtName" maxlength="11" autocomplete="off"><label>Channel key</label><input type="text" id="sMtPsk" placeholder="default" autocomplete="off" autocapitalize="off" spellcheck="false"><div class="hint">Base64 key from the Meshtastic app, or <b>default</b> (the public key), or <b>none</b>. Leave empty to keep the current key.</div><label>Hop limit: <b id="mtHopV">3</b></label><input type="range" id="sMtHop" min="1" max="7" step="1"><label>Frequency slot (0 = automatic)</label><input type="number" id="sMtSlot" min="0" max="99" step="1"><div class="btns"><button class="btn pr" id="saveMt">Apply</button></div><div class="hint">Works with Meshtastic nodes set to region <b>EU_433</b> and the same preset, channel name and key. Transmit power is limited to 10 dBm and airtime to 10 % per hour in this mode. Not available in Meshtastic mode: edit, delete, read receipts, private channels and fragmented long messages (long texts are sent as several messages).</div></div></details>
<details class="card"><summary><svg class="i"><use href="#i-wifi"/></svg><span>Device Wi-Fi</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><label>Network name</label><input type="text" id="sSsid" maxlength="31"><label>Password (8-63 characters)</label><input type="password" id="sWpass" autocomplete="new-password"><div class="sw"><span>Open page when connecting</span><input type="checkbox" id="sCap"></div><div class="hint">Phones show a "sign in to network" prompt that opens this page automatically.</div><div class="btns"><button class="btn pr" id="saveWifi">Save and restart</button></div></div></details>
<details class="card"><summary><svg class="i"><use href="#i-sun"/></svg><span>Appearance &amp; sound</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><label>Theme</label><select id="sTheme"><option value="auto">Auto (by clock)</option><option value="light">Day</option><option value="dark">Night</option></select><div class="kv"><span>Night starts</span><input type="time" id="sNightS" style="width:auto"></div><div class="kv"><span>Night ends</span><input type="time" id="sNightE" style="width:auto"></div><div class="hint">Auto uses your phone clock and switches to Night between these times. If the clock is unavailable, Day is used.</div><label>Message sound</label><select id="sSound"><option value="chime">Chime</option><option value="soft">Soft</option><option value="alert">Alert</option><option value="off">Off</option></select><div class="sw"><span>Vibration</span><input type="checkbox" id="sVib"></div><div class="btns"><button class="btn" id="testSound">Test sound</button><button class="btn pr" id="installBtn" style="display:none">Install app</button></div></div></details>
<details class="card"><summary><svg class="i"><use href="#i-lock"/></svg><span>Channels</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><div class="hint">A channel is a private group with its own passphrase. Only nodes that add the same name and passphrase can read it (up to 3). A repeater must add the channel too to forward its messages.</div><div id="chList"></div><label>Channel name</label><input type="text" id="chName" maxlength="20" autocomplete="off"><label>Channel passphrase</label><input type="password" id="chPass" autocomplete="new-password" placeholder="At least 6 characters"><div class="btns"><button class="btn pr" id="chAdd">Add channel</button></div></div></details><details class="card"><summary><svg class="i"><use href="#i-pulse"/></svg><span>Range test</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><div class="hint">Sends a series of pings and measures how many replies come back, with signal strength in both directions. Keep this page open.</div><label>Number of pings</label><select id="rtN"><option>5</option><option selected>10</option><option>20</option></select><div class="btns"><button class="btn pr" id="rtGo">Start test</button></div><div class="pb2"><i id="rtBar"></i></div><div class="hint" id="rtP"></div><div id="rtOut"></div></div></details><details class="card"><summary><svg class="i"><use href="#i-chart"/></svg><span>Statistics</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><table class="t" id="stats"></table><div class="btns"><button class="btn" id="refStats">Refresh</button></div></div></details>
<details class="card"><summary><svg class="i"><use href="#i-trash"/></svg><span>Danger zone</span><svg class="i chev"><use href="#i-chev"/></svg></summary><div class="body"><div class="btns" style="margin-top:0"><button class="btn" id="clrChat">Clear chat</button><button class="btn dg" id="wipeAll">Wipe everything</button></div><div class="hint">Wipe removes the key, settings and chat. Holding the hardware button for 5 seconds does the same.</div></div></details>
<div class="hint" style="text-align:center">LoRa Link v6</div>
    </div></section>
  </main>
  <nav>
    <button class="on" data-v="home"><svg class="i"><use href="#i-chat"/></svg><span>Chats</span></button>
    <button data-v="contacts"><svg class="i"><use href="#i-users"/></svg><span>Nodes</span></button>
    <button data-v="settings"><svg class="i"><use href="#i-set"/></svg><span>Settings</span></button>
  </nav>
<section id="v-chat">
  <div id="chead"><button class="ib" id="btnBack"><svg class="i"><use href="#i-back"/></svg></button><span class="av g" id="cav"><svg class="i"><use href="#i-users"/></svg></span><div class="ht"><div id="cname">Public chat</div><div id="csub">&nbsp;</div></div><button class="ib" id="btnSearch"><svg class="i"><use href="#i-search"/></svg></button><button class="ib" id="btnPing" title="Ping"><svg class="i"><use href="#i-pulse"/></svg></button></div>
  <div id="sbar"><input type="text" id="sInput" placeholder="Search messages" autocomplete="off"><span class="muted" id="sCnt"></span><button class="chip" id="sX">Close</button></div>
  <div id="pinbar"><span id="pinT"></span><button class="chip" id="pinX">Unpin</button></div>
  <div id="chatwrap"><div id="chat"></div><button class="ib" id="toBottom"><svg class="i"><use href="#i-down"/></svg><span class="badge" id="tbB"></span></button></div>
  <div id="compose">
    <div id="replyBar"><div class="rt" id="replyTxt"></div><button class="chip" id="replyX"><svg class="i"><use href="#i-x"/></svg></button></div>
    <div id="ment"></div>
    <div id="quick"></div>
    <div id="panel">
      <button class="pb" id="pLoc"><svg class="i"><use href="#i-pin"/></svg>Send my location</button>
      <div class="pb" id="pLive"><svg class="i"><use href="#i-radio"/></svg>Live share <select id="liveInt"><option value="15">15 s</option><option value="30" selected>30 s</option><option value="60">1 min</option><option value="300">5 min</option></select></div>
      <button class="pb" id="pSched"><svg class="i"><use href="#i-clock"/></svg>Send later</button>
      <button class="pb" id="pQuick"><svg class="i"><use href="#i-bolt"/></svg>Quick reply</button>
      <button class="pb" id="pPing"><svg class="i"><use href="#i-pulse"/></svg>Ping</button>
      <button class="pb dg" id="pSos"><svg class="i"><use href="#i-alert"/></svg>SOS</button>
    </div>
    <div id="bar">
      <button class="ib" id="btnPlus"><svg class="i"><use href="#i-plus"/></svg></button>
      <textarea id="txt" rows="1" placeholder="Message" autocomplete="off"></textarea>
      <button class="ib" id="btnQuick"><svg class="i"><use href="#i-bolt"/></svg></button>
      <button class="ib go" id="btnSend"><svg class="i"><use href="#i-send"/></svg></button>
    </div>
  </div>
</section>
</div>
<div id="onb"><div class="ob">
 <span class="logo"><svg class="i"><use href="#i-radio"/></svg></span>
 <div class="dots"><i class="on"></i><i></i><i></i></div>
 <div class="stp on" id="ob1"><h2>Welcome to LoRa Link</h2><p>Are you the first node? Enter your name and a shared passphrase. Every node must use the same passphrase, so if you are first, pick one and share it with the others.</p>
  <label>Your name</label><input type="text" id="obName" maxlength="24" autocomplete="off">
  <label>Shared passphrase</label><input type="password" id="obPass" placeholder="At least 6 characters" autocomplete="new-password">
  <div class="btns"><button class="btn pr" id="obNext1" style="flex:1">Continue</button></div><div class="obskip"><button class="lnk" id="obSkip">Skip for now</button></div></div>
 <div class="stp" id="ob2"><h2>Device Wi-Fi</h2><p>You can reuse your name as the Wi-Fi name, but a different one is better. Choose a strong password.</p>
  <label>Wi-Fi name</label><input type="text" id="obSsid" maxlength="31" autocomplete="off">
  <label>Wi-Fi password (8-63 characters)</label><input type="password" id="obWpass" autocomplete="new-password">
  <div class="btns"><button class="btn" id="obBack2">Back</button><button class="btn pr" id="obNext2" style="flex:1">Save and finish</button></div><div class="obskip"><button class="lnk" id="obKeep">Keep current Wi-Fi settings</button></div></div>
 <div class="stp" id="ob3"><h2 id="ob3t">You are all set</h2><p id="ob3p"></p><div class="btns"><button class="btn pr" id="obDone" style="flex:1">Open public chat</button></div></div>
</div></div>
<div id="shade"><div id="sheet"></div></div>
<div id="toast"></div>
<script>
(function(){
'use strict';
var $=function(id){return document.getElementById(id);};
var LS={g:function(k,d){try{var v=localStorage.getItem(k);return v===null?d:v;}catch(e){return d;}},s:function(k,v){try{localStorage.setItem(k,v);}catch(e){}}};
var QUICK=['OK','No','Yes','On my way','Waiting','Help!','Thanks','Call me'];
var DIRS=['N','NE','E','SE','S','SW','W','NW'];
var S={conv:'pub',unr:{},noAnim:false,sh:false,hsig:'',sg:0,stick:true,unread:0,newBelow:0,chatOpen:false,rd:{},msgs:new Map(),seen:new Set(),fs:{},contacts:[],me:{},rev:0,clr:0,first:true,firstRender:true,pos:null,heading:null,reply:null,watch:null,live:null,online:false,view:'home',alarm:null,unseen:0,installEv:null,statsTimer:null};
var busy=false;

function h(t,c,x){var e=document.createElement(t);if(c)e.className=c;if(x!==undefined&&x!==null)e.textContent=x;return e;}
var tt;function toast(m){var t=$('toast');t.textContent=m;t.classList.add('show');clearTimeout(tt);tt=setTimeout(function(){t.classList.remove('show');},2800);}
function api(path,data){
  var o={method:data?'POST':'GET'};
  if(data){o.headers={'Content-Type':'application/x-www-form-urlencoded'};o.body=new URLSearchParams(data).toString();}
  return fetch(path,o).then(function(r){return r.text().then(function(t){var j=null;try{j=JSON.parse(t);}catch(e){}if(!r.ok)throw new Error((j&&j.err)||t||('HTTP '+r.status));return j;});});
}

/* ---------- theme / sound / vibration ---------- */
function tmin(v){var m=/^(\d{1,2}):(\d{2})$/.exec(v||'');return m?(+m[1])*60+(+m[2]):null;}
function autoTheme(){try{var d=new Date(),n=d.getHours()*60+d.getMinutes(),s=tmin(LS.g('nightS','19:00')),e=tmin(LS.g('nightE','06:00'));if(isNaN(n)||s===null||e===null)return 'light';var night=s>e?(n>=s||n<e):(n>=s&&n<e);return night?'dark':'light';}catch(x){return 'light';}}
function applyTheme(){var t=LS.g('theme','auto');if(t==='auto')t=autoTheme();document.documentElement.setAttribute('data-theme',t);var m=document.querySelector('meta[name=theme-color]');if(m)m.setAttribute('content',t==='light'?'#e8ecfa':'#070914');}
setInterval(applyTheme,60000);
var actx=null;
function unlockAudio(){if(!actx){try{actx=new (window.AudioContext||window.webkitAudioContext)();}catch(e){}}if(actx&&actx.state==='suspended')actx.resume();}
document.addEventListener('click',unlockAudio);document.addEventListener('touchstart',unlockAudio,{passive:true});
function tone(f,t0,d,v){if(!actx)return;var o=actx.createOscillator(),g=actx.createGain(),n=actx.currentTime;o.type='sine';o.frequency.value=f;g.gain.setValueAtTime(0.0001,n+t0);g.gain.exponentialRampToValueAtTime(v||0.2,n+t0+0.02);g.gain.exponentialRampToValueAtTime(0.0001,n+t0+d);o.connect(g);g.connect(actx.destination);o.start(n+t0);o.stop(n+t0+d+0.05);}
function siren(){if(!actx)return;var o=actx.createOscillator(),g=actx.createGain(),n=actx.currentTime;o.type='sawtooth';o.frequency.setValueAtTime(650,n);o.frequency.linearRampToValueAtTime(1050,n+.55);o.frequency.linearRampToValueAtTime(650,n+1.1);g.gain.setValueAtTime(.0001,n);g.gain.exponentialRampToValueAtTime(.3,n+.05);g.gain.setValueAtTime(.3,n+1.0);g.gain.exponentialRampToValueAtTime(.0001,n+1.15);o.connect(g);g.connect(actx.destination);o.start(n);o.stop(n+1.2);}
function playSound(kind){var s=LS.g('sound','chime');if(kind==='sos'){siren();return;}if(kind==='mention'){tone(1175,0,.12,.3);tone(1568,.14,.22,.3);return;}if(s==='off')return;if(s==='soft')tone(660,0,.2,.15);else if(s==='chime'){tone(784,0,.18,.2);tone(1047,.18,.3,.2);}else{tone(900,0,.12,.25);tone(900,.18,.12,.25);tone(900,.36,.12,.25);}}
function vib(p){if(LS.g('vib','1')==='1'&&navigator.vibrate){try{navigator.vibrate(p);}catch(e){}}}

/* ---------- sheet / modal ---------- */
function closeSheet(){var g=++S.sg;$('shade').classList.add('out');setTimeout(function(){if(g!==S.sg)return;$('shade').classList.remove('on','mid','out');$('sheet').classList.remove('center');$('sheet').textContent='';},210);}
function sheet(o){
  S.sg++;$('shade').classList.remove('out','mid');var sh=$('sheet');sh.classList.remove('center');sh.textContent='';
  if(o.title)sh.appendChild(h('h4',null,o.title));
  if(o.text)sh.appendChild(h('p',null,o.text));
  var inp=null;
  if(o.input){inp=h('input');inp.type='text';inp.value=o.input.value||'';if(o.input.ph)inp.placeholder=o.input.ph;sh.appendChild(inp);}
  (o.actions||[]).forEach(function(a){
    var b=h('button','sb'+(a.cls?' '+a.cls:''),a.label);
    b.addEventListener('click',function(){var v=inp?inp.value:null;if(!a.keep)closeSheet();if(a.fn)a.fn(v);});
    sh.appendChild(b);
  });
  if(o.center){sh.classList.add('center');$('shade').classList.add('mid');}
  $('shade').classList.add('on');
  if(inp)setTimeout(function(){inp.focus();},50);
}
$('shade').addEventListener('click',function(e){if(e.target===$('shade')&&!S.alarm)closeSheet();});

/* ---------- geo helpers ---------- */
function toRad(x){return x*Math.PI/180;}
function dist(a,b,c,d){var R=6371000,dl=toRad(c-a),dn=toRad(d-b);var x=Math.pow(Math.sin(dl/2),2)+Math.cos(toRad(a))*Math.cos(toRad(c))*Math.pow(Math.sin(dn/2),2);return 2*R*Math.asin(Math.sqrt(x));}
function bearing(a,b,c,d){var y=Math.sin(toRad(d-b))*Math.cos(toRad(c));var x=Math.cos(toRad(a))*Math.sin(toRad(c))-Math.sin(toRad(a))*Math.cos(toRad(c))*Math.cos(toRad(d-b));return (Math.atan2(y,x)*180/Math.PI+360)%360;}
function fmtDist(m){return m<1000?Math.round(m)+' m':(m/1000).toFixed(m<10000?2:1)+' km';}
function dirName(b){return DIRS[Math.round(b/45)%8];}
function geoOK(){return !!(navigator.geolocation&&window.isSecureContext);}
function setPos(p){S.pos={lat:p.coords.latitude,lon:p.coords.longitude,acc:Math.round(p.coords.accuracy||0),t:Date.now()};}
function startWatch(){
  if(!geoOK())return false;
  if(S.watch===null){
    S.watch=navigator.geolocation.watchPosition(function(p){setPos(p);updateDyn();if(S.view==='contacts')renderContacts();},function(e){toast('GPS: '+e.message);S.watch=null;},{enableHighAccuracy:true,maximumAge:5000,timeout:20000});
    startCompass();
  }
  return true;
}
var compassOn=false;
function startCompass(){
  if(compassOn)return;compassOn=true;
  var on=function(e){var hd=null;if(typeof e.webkitCompassHeading==='number')hd=e.webkitCompassHeading;else if(e.alpha!==null&&e.alpha!==undefined&&(e.absolute||e.type==='deviceorientationabsolute'))hd=(360-e.alpha)%360;if(hd!==null){S.heading=hd;updateArrows();}};
  var go=function(){window.addEventListener('deviceorientationabsolute',on,true);window.addEventListener('deviceorientation',on,true);};
  if(typeof DeviceOrientationEvent!=='undefined'&&typeof DeviceOrientationEvent.requestPermission==='function'){DeviceOrientationEvent.requestPermission().then(function(r){if(r==='granted')go();}).catch(function(){});}else go();
}
function getPos(timeout){
  return new Promise(function(res,rej){
    if(S.pos&&(S.pos.man||Date.now()-S.pos.t<20000))return res(S.pos);
    if(!geoOK())return rej(new Error('nogeo'));
    navigator.geolocation.getCurrentPosition(function(p){setPos(p);res(S.pos);},function(e){rej(e);},{enableHighAccuracy:true,timeout:timeout||15000,maximumAge:5000});
  });
}
function parseLatLon(v){if(!v)return null;var m=v.replace(/،/g,',').split(/[,\s]+/).filter(Boolean);if(m.length<2)return null;var a=parseFloat(m[0]),b=parseFloat(m[1]);if(isNaN(a)||isNaN(b)||Math.abs(a)>90||Math.abs(b)>180)return null;return {lat:a,lon:b,acc:0};}
function manualSheet(cb){sheet({title:'Enter coordinates',text:'Format: lat,lon  e.g. 35.6892,51.3890',input:{ph:'35.6892,51.3890'},actions:[{label:'Use',cls:'dg',fn:function(v){var p=parseLatLon(v);if(!p){toast('Invalid coordinates');return;}setManual(p);if(cb)cb(p);}},{label:'Cancel'}]});}
function setManual(p){S.pos={lat:p.lat,lon:p.lon,acc:0,t:Date.now(),man:true};updateDyn();if(S.view==='contacts')renderContacts();gpsUI();}
function gpsUI(){var s=$('gpsState');if(!s)return;var ok=geoOK();s.textContent=ok?'Available':'Blocked (HTTP page)';s.className='tag '+(ok?'g':'r');$('gpsMan').textContent=S.pos?(S.pos.lat.toFixed(5)+', '+S.pos.lon.toFixed(5)+(S.pos.man?' (manual)':'')):'No position yet';$('gpsOrigin').textContent=location.origin;}
function openGuide(){navTo('settings');var d=$('gpsCard');if(d){d.open=true;setTimeout(function(){d.scrollIntoView({behavior:'smooth',block:'start'});},80);}}
function geoHelp(onManual){
  sheet({title:'GPS is blocked on this address',text:'Browsers only share GPS on secure (HTTPS) pages, and this page is plain HTTP.'+'\n'+'\n'+'Fix it once in Settings > Location & GPS, or enter coordinates by hand.',actions:[{label:'Open setup guide',fn:openGuide},{label:'Enter coordinates',fn:function(){manualSheet(onManual);}},{label:'Close'}]});
}
function distInfo(el){
  el.textContent='';
  if(!S.pos){el.textContent='Your position is needed to show distance';return;}
  var lat=+el.dataset.lat,lon=+el.dataset.lon;
  var d=dist(S.pos.lat,S.pos.lon,lat,lon),b=bearing(S.pos.lat,S.pos.lon,lat,lon);
  var ar=h('span','arr','↑');ar.dataset.b=b;el.appendChild(ar);
  el.appendChild(document.createTextNode(' '+fmtDist(d)+' • '+Math.round(b)+'° '+dirName(b)));
}
function updateDyn(){var l=document.querySelectorAll('.dyn');for(var i=0;i<l.length;i++)distInfo(l[i]);updateArrows();}
function updateArrows(){var l=document.querySelectorAll('.arr');for(var i=0;i<l.length;i++){var b=+l[i].dataset.b;l[i].style.transform='rotate('+(S.heading===null?b:(b-S.heading))+'deg)';}}
function locBlock(lat,lon,acc,mine){
  var w=h('div','loc');
  w.appendChild(h('div','loc-t',''+lat.toFixed(5)+','+lon.toFixed(5)));
  if(acc)w.appendChild(h('div','muted','Accuracy ≈ '+acc+' m'));
  var info=h('div','loc-i'+(mine?'':' dyn'));info.dataset.lat=lat;info.dataset.lon=lon;w.appendChild(info);
  var b=h('div','loc-b');
  var a1=h('a','chip','Map');a1.href='geo:'+lat+','+lon+'?q='+lat+','+lon;
  var a2=h('a','chip','OSM');a2.href='https://www.openstreetmap.org/?mlat='+lat+'&mlon='+lon+'#map=17/'+lat+'/'+lon;a2.target='_blank';a2.rel='noopener';
  b.appendChild(a1);b.appendChild(a2);w.appendChild(b);
  a1.addEventListener('click',function(e){e.stopPropagation();});a2.addEventListener('click',function(e){e.stopPropagation();});
  return w;
}

/* ---------- chat ---------- */
function two(n){return (n<10?'0':'')+n;}
function fmtTime(m){var d=null;if(m.t)d=new Date(m.t*1000);else if(S.fs[m.q])d=new Date(S.fs[m.q]);return d?two(d.getHours())+':'+two(d.getMinutes()):'';}
function bits(x){var c=0;while(x){c+=x&1;x>>=1;}return c;}
function findMsg(u,i){var r=null;S.msgs.forEach(function(m){if(m.u===u&&m.i===i&&m.d!=='S')r=m;});return r;}
function snippet(m){if(!m)return '';if(m.x)return 'Deleted message';if(m.k===1)return QUICK[+m.tx]||'';if(m.k===2)return 'Location';if(m.k===3)return 'SOS';return (m.tx||'').slice(0,70);}
function statusNode(m){
  var s=h('span','st');
  if(m.s===0){s.className='st pend';s.textContent=m.pt>1?''+bits(m.pm)+'/'+m.pt:'';}
  else if(m.s===1){s.textContent='✓'+(m.a>1?' '+m.a:'');}
  else if(m.s===3){s.textContent='✓✓';s.className='st read';}
  else {s.textContent='✗ Failed';s.className='st fail';}
  return s;
}
function bubble(m){
  if(m.d==='S')return h('div','sys',m.tx);
  var mine=m.d==='O';var dm=S.conv.charAt(0)==='u';
  var row=h('div','row '+(mine?'mine':'theirs')+((!S.rd[m.q]&&!S.firstRender&&!S.noAnim)?' in':''));S.rd[m.q]=1;
  var b=h('div','bub'+(m.k===3?' sos':''));
  if(!mine&&!dm){var w=h('div','who',m.n||'?');w.style.color=colOf(m.n);b.appendChild(w);}
  if(m.x){b.appendChild(h('div','muted','This message was deleted'));}
  else{
    if(m.rp){var q=findMsg(m.rp.u,m.rp.i);b.appendChild(h('div','quote',q?snippet(q):'Earlier message'));}
    if(m.k===0){var tx=h('div','txt');renderText(tx,m.tx);b.appendChild(tx);if(!mine&&mentionsMe(m.tx))b.classList.add('ment');}
    else if(m.k===1)b.appendChild(h('div','txt quick',QUICK[+m.tx]||m.tx));
    else if(m.k===2){if(m.hl)b.appendChild(locBlock(m.la/1e6,m.lo/1e6,m.ac,mine));else b.appendChild(h('div','muted','Invalid location'));}
    else if(m.k===3){b.appendChild(h('div','sos-t','SOS: help requested'));if(m.hl)b.appendChild(locBlock(m.la/1e6,m.lo/1e6,m.ac,mine));}
  }
  var meta=h('div','meta');
  var tm=fmtTime(m);if(tm)meta.appendChild(h('span',null,tm));
  if(m.e&&!m.x)meta.appendChild(h('span',null,'edited'));
  if(!mine){meta.appendChild(h('span',null,m.rs+' dBm'));}
  else if(!m.x)meta.appendChild(statusNode(m));
  b.appendChild(meta);
  attachGestures(b,m);row.dataset.k=m.u+':'+m.i;
  if(!mine&&!dm)row.appendChild(avatar(m.n));row.appendChild(b);return row;
}
var COLS=['#ff6b6b','#ff9f43','#e0a800','#1dd1a1','#00a8e8','#7a5cff','#ee5a9b','#4d8dff'];
function colOf(n){var x=0;n=n||'?';for(var i=0;i<n.length;i++)x=(x*31+n.charCodeAt(i))>>>0;return COLS[x%COLS.length];}
function avatar(n){var a=h('span','av',((n||'?').trim().charAt(0)||'?').toUpperCase());a.style.background=colOf(n);return a;}
function dayLabel(d){var n=new Date(),y=new Date(n.getTime()-864e5);function k(x){return x.getFullYear()*400+x.getMonth()*32+x.getDate();}if(k(d)===k(n))return 'Today';if(k(d)===k(y))return 'Yesterday';return d.toLocaleDateString(undefined,{day:'numeric',month:'long',year:d.getFullYear()===n.getFullYear()?undefined:'numeric'});}
function msgDate(m){return m.t?new Date(m.t*1000):(S.fs[m.q]?new Date(S.fs[m.q]):null);}
function scrollBottom(sm){var c=$('chat');if(c.scrollTo)c.scrollTo({top:c.scrollHeight,behavior:sm?'smooth':'auto'});else c.scrollTop=c.scrollHeight;}
function updateFab(){var f=$('toBottom');f.classList.toggle('show',S.chatOpen&&!S.stick);if(S.stick)S.newBelow=0;var b=$('tbB');b.textContent=S.newBelow;b.classList.toggle('on',S.newBelow>0);}
var mqW=window.matchMedia?matchMedia('(min-width:640px) and (orientation:landscape)'):null;
function isWide(){return !!(mqW&&mqW.matches);}
function svgI(n){var s=document.createElementNS('http://www.w3.org/2000/svg','svg');s.setAttribute('class','i');var u=document.createElementNS('http://www.w3.org/2000/svg','use');u.setAttribute('href','#i-'+n);s.appendChild(u);return s;}
function convOf(m){if(m.d==='S')return 'pub';if(m.ch)return 'c'+m.ch;var to=(m.to&&m.to!=='00000000')?m.to:'';if(to)return 'u'+(m.d==='O'?to:m.u);return 'pub';}
function chanOf(n){var r=null;(S.me.chs||[]).forEach(function(c){if(c.i===n)r=c;});return r;}
function contactOf(u){var r=null;S.contacts.forEach(function(c){if(c.u===u)r=c;});return r;}
function nameOf(u){var k=contactOf(u);return k?k.n:u;}
function convName(cv){if(cv==='pub')return 'Public chat';if(cv.charAt(0)==='c'){var c=chanOf(+cv.slice(1));return c?c.n:'Channel';}var k=contactOf(cv.slice(1));return k?k.n:'Node';}
function targetOf(cv){if(cv.charAt(0)==='c')return {ch:+cv.slice(1)};if(cv.charAt(0)==='u')return {to:cv.slice(1)};return {};}
function convList(){var l=['pub'];(S.me.chs||[]).forEach(function(c){l.push('c'+c.i);});S.contacts.forEach(function(c){if(!c.me)l.push('u'+c.u);});return l;}
function lastOf(cv){var best=null;S.msgs.forEach(function(m){if(m.d!=='S'&&!m.x&&convOf(m)===cv&&(!best||m.q>best.q))best=m;});return best;}
var LINK_RE=/(https?:\/\/[^\s]+)|(-?\d{1,2}\.\d{3,},\s*-?\d{1,3}\.\d{3,})|(@[\w\-\u0600-\u06FF]{2,24})|(\+?\d[\d\s\-]{7,}\d)/g;
function myTag(){return '@'+(S.me.n||'').replace(/\s+/g,'_');}
function mentionsMe(t){return !!(t&&S.me.n&&t.toLowerCase().indexOf(myTag().toLowerCase())>=0);}
function renderText(el,t){var last=0,m;LINK_RE.lastIndex=0;t=t||'';while((m=LINK_RE.exec(t))){if(m.index>last)el.appendChild(document.createTextNode(t.slice(last,m.index)));var x=m[0],a;
  if(m[1]){a=h('a','lk',x);a.href=x;a.target='_blank';a.rel='noopener';}
  else if(m[2]){var p=parseLatLon(x);if(p){a=h('a','lk',x);a.href='geo:'+p.lat+','+p.lon+'?q='+p.lat+','+p.lon;}else a=document.createTextNode(x);}
  else if(m[3]){a=h('span','mention'+(x.toLowerCase()===myTag().toLowerCase()?' me':''),x);}
  else{a=h('a','lk',x);a.href='tel:'+x.replace(/[^\d+]/g,'');}
  el.appendChild(a);last=m.index+x.length;}
  if(last<t.length)el.appendChild(document.createTextNode(t.slice(last)));}
var lastPT='';
function attachGestures(b,m){
  var sx=0,sy=0,tm=null,drag=false,go=false;
  b.addEventListener('pointerdown',function(e){lastPT=e.pointerType;if(e.pointerType==='mouse')return;sx=e.clientX;sy=e.clientY;drag=false;go=false;clearTimeout(tm);tm=setTimeout(function(){if(!drag){vib(15);openActions(m);}},450);});
  b.addEventListener('pointermove',function(e){if(e.pointerType==='mouse')return;var x=e.clientX-sx,y=e.clientY-sy;if(!drag&&(Math.abs(x)>10||Math.abs(y)>10)){clearTimeout(tm);if(Math.abs(x)>Math.abs(y)*1.4&&x<0)drag=true;}if(drag){var d=Math.max(x,-90);b.style.transition='none';b.style.transform='translateX('+d+'px)';if(d<-60&&!go){go=true;vib(10);}else if(d>=-60)go=false;}});
  b.addEventListener('pointerup',function(){clearTimeout(tm);if(drag){b.style.transition='';b.style.transform='';if(go&&!m.x)setReply(m);}drag=false;});
  b.addEventListener('pointercancel',function(){clearTimeout(tm);b.style.transition='';b.style.transform='';drag=false;});
  b.addEventListener('contextmenu',function(e){e.preventDefault();if(lastPT==='mouse')openActions(m);});
}
function getPins(){try{return JSON.parse(LS.g('pins','{}'))||{};}catch(e){return {};}}
function isPinned(k){return (getPins()[S.conv]||[]).some(function(p){return p.k===k;});}
function togglePin(m){var all=getPins(),l=all[S.conv]||[],k=m.u+':'+m.i;var i=-1;l.forEach(function(p,j){if(p.k===k)i=j;});if(i>=0)l.splice(i,1);else{l.push({k:k,tx:snippet(m),n:m.n||''});if(l.length>5)l.shift();}all[S.conv]=l;LS.s('pins',JSON.stringify(all));renderPin();}
function renderPin(){var l=getPins()[S.conv]||[],p=l[l.length-1],bar=$('pinbar');if(!p){bar.classList.remove('on');return;}$('pinT').textContent=(p.n?p.n+': ':'')+p.tx;bar.classList.add('on');$('pinT').onclick=function(){var r=document.querySelector('#chat .row[data-k="'+p.k+'"]');if(r){r.scrollIntoView({behavior:'smooth',block:'center'});r.classList.add('flash');setTimeout(function(){r.classList.remove('flash');},1300);}else toast('That message is no longer on the device');};}
function forwardMsg(m){var a=[];convList().forEach(function(cv){a.push({label:convName(cv),fn:function(){api('/api/send',Object.assign({text:'Forwarded from '+(m.n||'?')+': '+m.tx},targetOf(cv))).then(function(){toast('Forwarded');poll();}).catch(function(e){toast(e.message);});}});});a.push({label:'Cancel'});sheet({title:'Forward to',actions:a});}
function deliveryInfo(m){var ab=m.ab||[],rb=m.rb||[];var t='Status: '+(m.s===3?'read':m.s===1?'delivered':m.s===2?'failed':'sending')+'\nDelivered to: '+(ab.length?ab.map(nameOf).join(', '):'nobody yet')+'\nRead by: '+(rb.length?rb.map(nameOf).join(', '):'nobody yet');
  var tgt=(m.to&&m.to!=='00000000')?m.to:'';var wait=S.contacts.filter(function(c){return !c.me&&ab.indexOf(c.u)<0&&(!tgt||tgt===c.u);}).map(function(c){return c.n;});
  if(wait.length&&m.s!==2&&!m.ch)t+='\nNot confirmed: '+wait.join(', ');
  sheet({title:'Delivery info',text:t,actions:[{label:'Close'}]});}
function getSched(){try{return JSON.parse(LS.g('sched','[]'))||[];}catch(e){return [];}}
function setSched(a){LS.s('sched',JSON.stringify(a));bnSig='';renderBanner();}
function schedTick(){var a=getSched(),now=Date.now(),keep=[];a.forEach(function(x){if(x.at<=now){api('/api/send',Object.assign({text:x.text},targetOf(x.cv))).then(poll).catch(function(){});}else keep.push(x);});if(keep.length!==a.length)setSched(keep);}
setInterval(schedTick,5000);
function viewSched(){var acts=[];getSched().forEach(function(x,i){acts.push({label:'Cancel: '+x.text.slice(0,30),fn:function(){var a=getSched();a.splice(i,1);setSched(a);}});});acts.push({label:'Close'});sheet({title:'Scheduled messages',actions:acts});}
function openSched(){closePanel();var t=$('txt').value.trim(),acts=[];if(t){[[1,'In 1 minute'],[5,'In 5 minutes'],[15,'In 15 minutes'],[60,'In 1 hour']].forEach(function(o){acts.push({label:o[1],fn:function(){var a=getSched();a.push({at:Date.now()+o[0]*60000,text:t,cv:S.conv});setSched(a);$('txt').value='';LS.s('dr:'+S.conv,'');grow();toast('Scheduled');}});});}
  var n=getSched().length;if(n)acts.push({label:'Scheduled messages ('+n+')',keep:true,fn:viewSched});acts.push({label:'Close'});
  sheet({title:'Send later',text:t?'The page must stay open when the time comes.':'Type a message first, then tap Send later.',actions:acts});}
function mentionSuggest(){var t=$('txt'),v=t.value.slice(0,t.selectionStart===undefined?t.value.length:t.selectionStart);var m=/(?:^|\s)@([\w\-\u0600-\u06FF]*)$/.exec(v),box=$('ment');if(!m||S.conv.charAt(0)==='u'){box.classList.remove('on');return;}
  var q=m[1].toLowerCase(),n=0;box.textContent='';
  S.contacts.forEach(function(c){if(c.me)return;var tag=c.n.replace(/\s+/g,'_');if(tag.toLowerCase().indexOf(q)!==0)return;n++;var b=h('button','chip','@'+tag);b.addEventListener('mousedown',function(e){e.preventDefault();});b.addEventListener('click',function(){var base=v.slice(0,v.length-m[1].length-1);t.value=base+'@'+tag+' '+t.value.slice(v.length);t.focus();grow();box.classList.remove('on');});box.appendChild(b);});
  box.classList.toggle('on',n>0);}
function renderChat(){
  var box=$('chat');var prev=box.scrollTop;var was=S.stick||S.firstRender;
  box.textContent='';
  var arr=[],q=(S.search||'').toLowerCase();
  S.msgs.forEach(function(m){if(convOf(m)!==S.conv)return;if(q&&(m.tx||'').toLowerCase().indexOf(q)<0&&(m.n||'').toLowerCase().indexOf(q)<0)return;arr.push(m);});
  arr.sort(function(a,b){return a.q-b.q;});
  if(!arr.length)box.appendChild(h('div','empty',q?'No matches.':(S.conv==='pub'?'No messages yet.\nSay hello to everyone.':'No messages yet.\nSay hello.')));
  var last='';
  arr.forEach(function(m){var d=msgDate(m);if(d){var k=dayLabel(d);if(k!==last){last=k;box.appendChild(h('div','dsep',k));}}box.appendChild(bubble(m));});
  if(q)$('sCnt').textContent=arr.length+' found';
  box.scrollTop=was?box.scrollHeight:prev;
  S.firstRender=false;updateDyn();updateFab();
}
function renderHome(){
  var box=$('hlist');if(!box)return;
  var l=convList(),nh=1+(S.me.chs||[]).length,nodes=l.slice(nh);
  nodes.sort(function(a,b){var x=lastOf(a),y=lastOf(b);return (y?y.q:0)-(x?x.q:0);});
  var order=l.slice(0,nh).concat(nodes),rows=[];
  order.forEach(function(cv){var m=lastOf(cv);rows.push([cv,convName(cv),m?m.q+':'+m.r:'',LS.g('dr:'+cv,''),S.unr[cv]||0,(isWide()&&S.chatOpen&&S.conv===cv)?1:0,m?fmtTime(m):'']);});
  var sig=JSON.stringify(rows);if(sig===S.hsig)return;S.hsig=sig;box.textContent='';
  order.forEach(function(cv,i){
    var m=lastOf(cv),it=h('button','citem'+(rows[i][5]?' act':'')),av;
    if(cv==='pub'||cv.charAt(0)==='c'){av=h('span','av big g');av.appendChild(svgI(cv==='pub'?'users':'lock'));}else{av=avatar(convName(cv));av.className+=' big';}
    it.appendChild(av);
    var cm=h('div','cm'),r1=h('div','cl'),r2=h('div','cl');
    r1.appendChild(h('b',null,convName(cv)));r1.appendChild(h('span','ct',rows[i][6]));
    var dr=rows[i][3];var pv=dr?('Draft: '+dr.slice(0,50)):(m?((m.d==='O'?'You: ':((cv==='pub'||cv.charAt(0)==='c')&&m.n?m.n+': ':''))+snippet(m)):'No messages yet');
    var bd=h('span','badge');bd.textContent=rows[i][4];if(rows[i][4]>0)bd.classList.add('on');
    r2.appendChild(h('span','cp'+(dr?' dr':''),pv));r2.appendChild(bd);cm.appendChild(r1);cm.appendChild(r2);it.appendChild(cm);
    it.addEventListener('click',function(){openConv(cv);});box.appendChild(it);
  });
}
function csubUpd(){
  $('cname').textContent=convName(S.conv);
  if(S.conv.charAt(0)!=='u'){var on=0;S.contacts.forEach(function(c){if(c.me||(c.ls>=0&&c.ls<300))on++;});$('csub').textContent=S.contacts.length+' nodes, '+on+' online'+(S.me.q?' · queue '+S.me.q:'');}
  else{var k=contactOf(S.conv.slice(1));$('csub').textContent=k?(k.ls>=0&&k.ls<300?'online':(k.ls<0?'not heard yet':'last seen '+ago(k.ls))):'';}
}
function saveDraft(){LS.s('dr:'+S.conv,$('txt').value.trim());}
function openConv(cv){
  if(S.chatOpen)saveDraft();
  S.conv=cv;S.chatOpen=true;S.unr[cv]=0;S.stick=true;S.search='';$('sbar').classList.remove('on');$('sInput').value='';$('sCnt').textContent='';
  var av=$('cav');av.textContent='';
  if(cv==='pub'||cv.charAt(0)==='c'){av.className='av g';av.style.background='';av.appendChild(svgI(cv==='pub'?'users':'lock'));}
  else{av.className='av';av.textContent=(convName(cv).charAt(0)||'?').toUpperCase();av.style.background=colOf(convName(cv));}
  csubUpd();$('txt').value=LS.g('dr:'+cv,'');grow();clearReply();renderPin();
  S.noAnim=true;renderChat();S.noAnim=false;S.hsig='';renderHome();
  $('v-chat').classList.add('open');
  if(!isWide()&&!(history.state&&history.state.c))history.pushState({c:1},'');
  setTimeout(function(){scrollBottom(false);updateFab();},30);
}
function closeChat(){saveDraft();S.chatOpen=false;$('txt').blur();$('panel').classList.remove('on');$('quick').classList.remove('on');$('ment').classList.remove('on');$('v-chat').classList.remove('open');S.hsig='';renderHome();updateFab();}
function grow(){var t=$('txt');t.style.height='auto';t.style.height=Math.min(t.scrollHeight,120)+'px';var has=t.value.trim().length>0;$('btnSend').classList.toggle('show',has);$('btnQuick').classList.toggle('hide',has);if(S.stick)scrollBottom(false);}
function openActions(m){
  if(m.x)return;
  var a=[];
  if(m.d==='O'&&m.s===2&&m.k===0)a.push({label:'Resend',fn:function(){sendText(m.tx);}});
  a.push({label:'Reply',fn:function(){setReply(m);}});
  if(m.k===0){a.push({label:'Copy text',fn:function(){copyText(m.tx);}});a.push({label:'Forward',fn:function(){forwardMsg(m);}});}
  a.push({label:isPinned(m.u+':'+m.i)?'Unpin':'Pin',fn:function(){togglePin(m);}});
  if(m.d==='O')a.push({label:'Delivery info',fn:function(){deliveryInfo(m);}});
  if(m.d==='O'&&m.k===0)a.push({label:'Edit',fn:function(){editMsg(m);}});
  if(m.d==='O')a.push({label:'Delete for everyone',cls:'dg',fn:function(){delMsg(m);}});
  a.push({label:'Close'});
  sheet({actions:a});
}
function copyText(t){try{if(navigator.clipboard&&window.isSecureContext){navigator.clipboard.writeText(t);toast('Copied');return;}}catch(e){}var ta=document.createElement('textarea');ta.value=t;document.body.appendChild(ta);ta.select();try{document.execCommand('copy');toast('Copied');}catch(e){}document.body.removeChild(ta);}
function setReply(m){S.reply={u:m.u,i:m.i};$('replyTxt').textContent=(m.n||'')+': '+snippet(m);$('replyBar').classList.add('on');$('txt').focus();}
function clearReply(){S.reply=null;$('replyBar').classList.remove('on');}
function editMsg(m){sheet({title:'Edit message',input:{value:m.tx},actions:[{label:'Save',cls:'dg',fn:function(v){v=(v||'').trim();if(!v)return;api('/api/edit',{id:m.i,text:v}).then(poll).catch(function(e){toast(e.message);});}},{label:'Cancel'}]});}
function delMsg(m){sheet({title:'Delete message',text:'The message will be deleted on other devices too.',actions:[{label:'Delete',cls:'dg',fn:function(){api('/api/del',{id:m.i}).then(poll).catch(function(e){toast(e.message);});}},{label:'Cancel'}]});}
function sendText(t){
  var d=Object.assign({text:t},targetOf(S.conv));if(S.reply){d.ru=S.reply.u;d.ri=S.reply.i;}
  clearReply();
  return api('/api/send',d).then(poll).then(function(){S.stick=true;scrollBottom(true);}).catch(function(e){toast(e.message);});
}
function send(){var inp=$('txt');var t=inp.value.trim();if(!t)return;var had=document.activeElement===inp;inp.value='';LS.s('dr:'+S.conv,'');grow();$('ment').classList.remove('on');sendText(t).then(function(){if(had)inp.focus();});}
function quickSend(i){api('/api/quick',Object.assign({code:i},targetOf(S.conv))).then(poll).catch(function(e){toast(e.message);});}
function pingNow(){api('/api/ping',{}).then(function(){toast('Ping sent');poll();}).catch(function(e){toast(e.message);});}

function closePanel(){$('panel').classList.remove('on');}
function sendLocation(){
  closePanel();
  var go=function(p){api('/api/loc',Object.assign({mode:'msg',lat:p.lat,lon:p.lon,acc:p.acc||0},targetOf(S.conv))).then(function(){toast('Location queued for sending');poll();}).catch(function(e){toast(e.message);});};
  if(!geoOK()&&!(S.pos&&S.pos.man)){geoHelp(go);return;}
  startWatch();toast('Getting location...');
  getPos().then(go).catch(function(e){toast('GPS: '+(e.message||'failed'));});
}
function toggleLive(){
  if(S.live){clearInterval(S.live.timer);S.live=null;renderBanner();toast('Live sharing stopped');return;}
  if(!geoOK()){geoHelp(null);return;}
  var sec=+$('liveInt').value;
  startWatch();
  var tick=function(){getPos().then(function(p){return api('/api/loc',{mode:'live',lat:p.lat,lon:p.lon,acc:p.acc||0});}).catch(function(){});};
  tick();S.live={sec:sec,timer:setInterval(tick,sec*1000)};
  renderBanner();closePanel();toast('Live sharing started');
}
function openSOS(){
  closePanel();
  sheet({title:'Send SOS',text:'An emergency message with your location is sent to all devices and repeated until it is acknowledged.',center:true,actions:[{label:'Send SOS',cls:'dg',fn:doSOS},{label:'Cancel'}]});
}
function doSOS(){
  var fin=function(p){var d=p?{lat:p.lat,lon:p.lon,acc:p.acc||0}:{};api('/api/sos',d).then(function(){toast('SOS queued for sending');poll();}).catch(function(e){toast(e.message);});};
  if(S.pos&&S.pos.man){fin(S.pos);return;}if(!geoOK()){fin(null);return;}
  toast('Getting location...');
  getPos(6000).then(fin).catch(function(){fin(null);});
}
function showAlarm(m){
  if(S.alarm)return;
  var go=function(){clearInterval(S.alarm);S.alarm=null;api('/api/send',{text:'SOS from '+(m.n||'?')+' acknowledged by '+(S.me.n||'me')}).then(poll).catch(function(){});};
  var txt='From '+(m.n||'?')+(m.hl?'\nLocation: '+(m.la/1e6).toFixed(5)+','+(m.lo/1e6).toFixed(5):'\nNo location was attached.');
  sheet({title:'SOS: help requested',text:txt,center:true,actions:[{label:'Acknowledge',cls:'dg',fn:go}].concat(m.hl?[{label:'Open in maps',keep:true,fn:function(){location.href='geo:'+(m.la/1e6)+','+(m.lo/1e6)+'?q='+(m.la/1e6)+','+(m.lo/1e6);}}]:[])});
  S.alarm=setInterval(function(){playSound('sos');vib([300,150,300]);},1200);
  playSound('sos');vib([300,150,300]);
}

/* ---------- contacts ---------- */
function ago(s){if(s<0)return 'never';if(s<60)return s+' s ago';if(s<3600)return Math.floor(s/60)+' min ago';return Math.floor(s/3600)+' h ago';}
function qual(r){if(r>-80)return ['Excellent','g'];if(r>-100)return ['Good','g'];if(r>-115)return ['Weak','y'];return ['Very weak','r'];}
function compassFor(c){startCompass();sheet({title:c.n,actions:[{label:'Close'}]});var sh=$('sheet');var d=h('div','cmp dyn');d.dataset.lat=c.la/1e6;d.dataset.lon=c.lo/1e6;sh.insertBefore(d,sh.children[1]||null);sh.insertBefore(h('p',null,S.heading===null?'Live heading needs a secure page (see Settings > Location & GPS). The arrow shows the bearing from north.':'Hold the phone flat and point the top forward. The arrow points to the node.'),sh.children[2]||null);updateDyn();}
function renderRadar(){var box=$('radar');box.textContent='';if(!S.pos)return;var pts=[];S.contacts.forEach(function(c){if(!c.me&&c.hl)pts.push({c:c,d:dist(S.pos.lat,S.pos.lon,c.la/1e6,c.lo/1e6),b:bearing(S.pos.lat,S.pos.lon,c.la/1e6,c.lo/1e6)});});if(!pts.length)return;
  var mx=0;pts.forEach(function(p){if(p.d>mx)mx=p.d;});var st=[50,100,250,500,1000,2000,5000,10000,25000,50000,100000],R=st[st.length-1];for(var i=0;i<st.length;i++){if(st[i]>=mx*1.1){R=st[i];break;}}
  var card=h('div','card radar');card.appendChild(h('div','cn','Radar'));card.appendChild(h('div','muted','North is up. Outer ring = '+fmtDist(R)+'. Tap a node for the compass.'));
  var NS='http://www.w3.org/2000/svg',svg=document.createElementNS(NS,'svg');svg.setAttribute('viewBox','-110 -110 220 220');
  function el(n,a){var e=document.createElementNS(NS,n);for(var k in a)e.setAttribute(k,a[k]);return e;}
  [1,.66,.33].forEach(function(f){svg.appendChild(el('circle',{cx:0,cy:0,r:95*f,fill:'none',stroke:'currentColor','stroke-opacity':.2}));});
  svg.appendChild(el('line',{x1:-95,y1:0,x2:95,y2:0,stroke:'currentColor','stroke-opacity':.12}));svg.appendChild(el('line',{x1:0,y1:-95,x2:0,y2:95,stroke:'currentColor','stroke-opacity':.12}));
  var t=el('text',{x:0,y:-99,'text-anchor':'middle'});t.textContent='N';svg.appendChild(t);svg.appendChild(el('circle',{cx:0,cy:0,r:5,fill:'#28ff78'}));
  pts.forEach(function(p){var r=Math.min(95,p.d/R*95),a=toRad(p.b),x=r*Math.sin(a),y=-r*Math.cos(a);var g=el('g',{style:'cursor:pointer'});g.appendChild(el('circle',{cx:x,cy:y,r:6,fill:(p.c.ls>=0&&p.c.ls<300)?'#00c8ff':'#8a93b8'}));var tx=el('text',{x:x,y:y-10,'text-anchor':'middle'});tx.textContent=p.c.n;g.appendChild(tx);g.addEventListener('click',function(){compassFor(p.c);});svg.appendChild(g);});
  card.appendChild(svg);box.appendChild(card);}
function renderContacts(){var sv=$('v-contacts').scrollTop;
  var box=$('clist');box.textContent='';
  if(!S.pos){
    var c0=h('div','card');c0.appendChild(h('div','hint','Your position is needed to show distance and bearing to nodes.'));
    var bb=h('div','btns');var be=h('button','btn pr','Enable my location');
    be.addEventListener('click',function(){if(!geoOK()){geoHelp(function(p){renderContacts();});return;}startWatch();toast('Getting location...');});
    bb.appendChild(be);c0.appendChild(bb);box.appendChild(c0);
  }
  S.contacts.forEach(function(c){
    var card=h('div','card');
    var r=h('div','cr');
    r.appendChild(h('span','od'+((c.ls>=0&&c.ls<300)||c.me?' on':'')));
    r.appendChild(h('span','cn',c.n));
    if(c.me)r.appendChild(h('span','tag','You'));
    if(c.rl)r.appendChild(h('span','tag','Repeater'));
    card.appendChild(r);
    var r2=h('div','cr');r2.style.marginTop='6px';
    if(!c.me){
      r2.appendChild(h('span','muted','Last heard: '+ago(c.ls)));
      if(c.ls>=0){var q=qual(c.rs);r2.appendChild(h('span','tag '+q[1],c.rs+' dBm • SNR '+c.sn+' • '+q[0]));}
    }
    card.appendChild(r2);
    if(c.hl){
      var lb=locBlock(c.la/1e6,c.lo/1e6,c.ac,c.me);
      var age=h('div','muted','Location updated: '+ago(c.lt));lb.insertBefore(age,lb.children[1]||null);
      card.appendChild(lb);
    }
    if(c.tp>0){
      var bt=h('div','btns');var g=h('a','chip','GPX track · '+c.tp+' points');g.href='/gpx?u='+(c.me?'me':c.u);g.setAttribute('download','track-'+c.u+'.gpx');bt.appendChild(g);card.appendChild(bt);
    }
    if(!c.me){var ab=h('div','btns');var mb=h('button','chip','Message');mb.addEventListener('click',function(){openConv('u'+c.u);});ab.appendChild(mb);if(c.hl&&S.pos){var cb=h('button','chip','Compass');cb.addEventListener('click',function(){compassFor(c);});ab.appendChild(cb);}card.appendChild(ab);}
    box.appendChild(card);
  });
  renderRadar();updateDyn();$('v-contacts').scrollTop=sv;
}

/* ---------- settings ---------- */
var PH=['Balanced: good range and speed (default).','Long range: best range, messages take about 4x longer to send.','Fast: shorter range, messages are about 8x quicker.','Fast wide: quickest, shortest range (about 16x quicker).'];
function presetHint(){$('presetHint').textContent=PH[+$('sPreset').value||0]+' All nodes must use the same preset, otherwise they cannot hear each other.';}
function renderChList(){var b=$('chList');if(!b)return;var sig=JSON.stringify(S.me.chs||[]);if(b.dataset.s===sig)return;b.dataset.s=sig;b.textContent='';
  (S.me.chs||[]).forEach(function(c){var r=h('div','chl');r.appendChild(h('span',null,c.n+'  ·  key '+c.fp));var d=h('button','btn dg','Remove');d.addEventListener('click',function(){sheet({title:'Remove channel',text:'Its messages are deleted from this device.',center:true,actions:[{label:'Remove',cls:'dg',fn:function(){api('/api/chan',{slot:c.i,del:1}).then(poll).catch(function(e){toast(e.message);});}},{label:'Cancel'}]});});r.appendChild(d);b.appendChild(r);});
  if(!(S.me.chs||[]).length)b.appendChild(h('div','hint','No channels yet.'));}
function fillSettings(){
  var m=S.me;if(!m||!m.u)return;
  var fl=function(id,v){var e=$(id);if(e&&!e.dataset.dirty&&document.activeElement!==e)e.value=v;};
  var fb=function(id,v){var e=$(id);if(e&&!e.dataset.dirty&&document.activeElement!==e)e.checked=v;};
  fl('sName',m.n||'');fl('sSsid',m.ssid||'');fl('sFreq',(m.fq/1000).toFixed(3));fl('sPreset',String(m.pr||0));fl('sTxp',String(m.pw||17));
  fb('sRelay',!!m.rl);fb('sCap',!!m.cp);
  fl('sMtPre',String(m.mp||0));fl('sMtName',m.mc||'');fl('sMtHop',String(m.mh||3));fl('sMtSlot',String(m.ms||0));fb('sMtMode',!!m.mt);
  $('mtHopV').textContent=$('sMtHop').value;$('sMtPsk').placeholder='Current: '+(m.mk||'default');
  $('mtHint').textContent=(m.mt?'ON - ':'OFF - ')+'channel "'+(m.mn||'')+'", '+((m.mf||0)/1e6).toFixed(4)+' MHz, key '+(m.mk||'default')+(m.mt?'. The Radio card above is ignored while this mode is on.':'. Turn it on to join a Meshtastic mesh instead of LoRa Link nodes; LoRa Link nodes cannot hear you in that mode.');
  $('txpV').textContent=$('sTxp').value;presetHint();$('sFp').textContent=m.fp||'----';
  fl('sTheme',LS.g('theme','auto'));fl('sNightS',LS.g('nightS','19:00'));fl('sNightE',LS.g('nightE','06:00'));
  $('sSound').value=LS.g('sound','chime');$('sVib').checked=LS.g('vib','1')==='1';renderChList();
}
function cfg(d,okMsg){
  return api('/api/cfg',d).then(function(r){
    toast(okMsg||'Saved');var mp={name:['sName'],ssid:['sSsid'],freq:['sFreq'],relay:['sRelay'],preset:['sPreset'],txp:['sTxp'],cap:['sCap'],mode:['sMtMode'],mtpre:['sMtPre'],mtname:['sMtName'],mthop:['sMtHop'],mtslot:['sMtSlot']};Object.keys(d).forEach(function(k){(mp[k]||[]).forEach(function(id){$(id).dataset.dirty='';});});
    if(r&&r.restart)sheet({title:'Restart required',text:'The device must restart to apply the new Wi-Fi name or password. Reconnect to the new network afterwards.',center:true,actions:[{label:'Restart',cls:'dg',fn:function(){api('/api/restart',{}).catch(function(){});toast('Restarting...');}},{label:'Later'}]});
    poll();
  }).catch(function(e){toast(e.message);});
}
function loadStats(){
  api('/api/stats').then(function(s){
    var t=$('stats');t.textContent='';
    var rows=[['Uptime',s.up+' s'],['Packets sent',s.tx],['Packets received',s.rx],['Valid packets',s.ok],['Rejected (auth)',s.auth],['Rejected (replay)',s.replay],['Duplicates',s.dup],['Retries',s.retry],['Failed sends',s.fail],['Relayed packets',s.relay],['Relays skipped (smart)',s.supp||0],['Average RSSI',s.avg+' dBm'],['Airtime (last hour)',(s.air/1000).toFixed(1)+' s ('+(s.air/36000).toFixed(2)+'%)'],['Send queue',s.q],['Free memory',s.heap+' bytes']];
    rows.forEach(function(r){var tr=document.createElement('tr');tr.appendChild(h('td',null,r[0]));tr.appendChild(h('td',null,String(r[1])));t.appendChild(tr);});
  }).catch(function(){});
}
function doScan(){
  var b=$('doScan');b.disabled=true;toast('Scanning...');
  api('/api/scan',{}).then(function(r){
    b.disabled=false;
    var box=$('scanbox'),lb=$('scanlbl');box.textContent='';lb.textContent='';
    var ch=r.ch||[];if(!ch.length)return;
    var sorted=ch.slice().sort(function(a,b){return a.r-b.r;});var quiet={};sorted.slice(0,3).forEach(function(c){quiet[c.f]=1;});
    ch.forEach(function(c){
      var p=Math.max(4,Math.min(100,(c.r+125)*100/65));
      var d=h('div',quiet[c.f]?'q':'');d.style.height=p+'%';d.title=(c.f/1000).toFixed(3)+' MHz  '+c.r+' dBm';
      d.addEventListener('click',function(){$('sFreq').value=(c.f/1000).toFixed(3);toast((c.f/1000).toFixed(3)+' MHz');});
      box.appendChild(d);
    });
    lb.appendChild(h('span',null,(ch[0].f/1000).toFixed(2)));lb.appendChild(h('span',null,(ch[ch.length-1].f/1000).toFixed(2)));
    toast('Scan complete');
  }).catch(function(e){b.disabled=false;toast(e.message);});
}

/* ---------- banners / views ---------- */
var bnSig='';
function renderBanner(){var a=[];if(!S.online)a.push(['','Connection to the device lost. Stay connected to its Wi-Fi.']);if(S.live)a.push(['info','Live location sharing is on (every '+S.live.sec+' s). Keep this page open.']);var ns=getSched().length;if(ns)a.push(['info',ns+' scheduled message'+(ns>1?'s':'')+'. Keep this page open.']);var sig=JSON.stringify(a);if(sig===bnSig)return;bnSig=sig;var b=$('banner');b.textContent='';a.forEach(function(x){b.appendChild(h('div','bn'+(x[0]?' '+x[0]:''),x[1]));});}
function showView(v){S.view=v;['home','contacts','settings'].forEach(function(x){$('v-'+x).classList.toggle('active',x===v);});var bs=document.querySelectorAll('nav button');for(var i=0;i<bs.length;i++)bs[i].classList.toggle('on',bs[i].dataset.v===v);clearInterval(S.statsTimer);S.statsTimer=null;if(v==='contacts')renderContacts();if(v==='home')renderHome();if(v==='settings'){fillSettings();gpsUI();loadStats();S.statsTimer=setInterval(loadStats,4000);}}

/* ---------- polling ---------- */
function poll(){
  if(busy)return Promise.resolve();
  busy=true;
  var url='/api/poll?rev='+S.rev+'&clr='+S.clr+'&vis='+(document.hidden?0:1)+'&t='+Math.floor(Date.now()/1000);
  return fetch(url,{cache:'no-store'}).then(function(r){return r.json();}).then(function(j){
    busy=false;
    S.online=true;$('dot').classList.add('on');
    var clrChanged=(j.clr!==S.clr);if(clrChanged){S.msgs.clear();S.clr=j.clr;}
    var newIn=[];
    (j.msgs||[]).forEach(function(m){
      if(!S.seen.has(m.q)){S.seen.add(m.q);if(!m.t)S.fs[m.q]=Date.now();if(m.d==='I'&&!S.first&&!m.x)newIn.push(m);}
      S.msgs.set(m.q,m);
    });
    S.rev=j.rev;S.me=j.me||{};S.contacts=j.contacts||[];
    if(S.conv.charAt(0)==='c'&&!chanOf(+S.conv.slice(1))){S.conv='pub';if(S.chatOpen)openConv('pub');}
    $('hname').textContent=S.me.n||'LoRa Link';
    var hs=S.me.mt?('Meshtastic \u2022 '+(S.me.mn||'')):('Key '+(S.me.fp||'----'));
    if(S.contacts.length>1)hs+=' • '+(S.contacts.length-1)+' nodes';
    if(S.me.q)hs+=' • queue: '+S.me.q;
    $('hsub').textContent=hs;csubUpd();maybeOnboard();
    if((j.msgs&&j.msgs.length)||S.firstRender||clrChanged)renderChat();
    if(S.view==='contacts')renderContacts();
    if(S.view==='settings')fillSettings();
    renderBanner();
    newIn.forEach(function(m){var cv=convOf(m),here=S.chatOpen&&S.conv===cv&&!document.hidden;if(!here)S.unr[cv]=(S.unr[cv]||0)+1;else if(!S.stick)S.newBelow++;if(m.k===3)showAlarm(m);else if(m.k===0&&mentionsMe(m.tx)){playSound('mention');vib([200,80,200]);}else{playSound('msg');vib([120,60,120]);}});
    updateFab();renderHome();if(newIn.length&&document.hidden){S.unseen+=newIn.length;document.title='('+S.unseen+') LoRa Link';}
    S.first=false;
  }).catch(function(){
    busy=false;S.online=false;$('dot').classList.remove('on');$('hsub').textContent='Disconnected';renderBanner();
  });
}

/* ---------- wiring ---------- */
$('btnSend').addEventListener('click',send);
$('txt').addEventListener('keyup',function(e){S.sh=e.shiftKey;});
$('txt').addEventListener('keydown',function(e){S.sh=e.shiftKey;if(e.key==='Enter'&&!e.shiftKey){e.preventDefault();send();}});
$('btnPlus').addEventListener('click',function(){$('panel').classList.toggle('on');$('quick').classList.remove('on');});$('btnQuick').addEventListener('click',function(){closePanel();$('quick').classList.toggle('on');});
$('pLoc').addEventListener('click',sendLocation);
$('pLive').addEventListener('click',function(e){if(e.target.tagName!=='SELECT')toggleLive();});
$('pQuick').addEventListener('click',function(){closePanel();$('quick').classList.add('on');});
$('pPing').addEventListener('click',function(){closePanel();pingNow();});
$('pSos').addEventListener('click',openSOS);
$('btnPing').addEventListener('click',pingNow);
$('replyX').addEventListener('click',clearReply);
(function(){var q=$('quick');QUICK.forEach(function(t,i){var c=h('button','chip',t);c.addEventListener('click',function(){quickSend(i);q.classList.remove('on');});q.appendChild(c);});var x=h('button','chip','✕');x.addEventListener('click',function(){q.classList.remove('on');});q.appendChild(x);})();
(function(){var bs=document.querySelectorAll('nav button');for(var i=0;i<bs.length;i++)bs[i].addEventListener('click',function(){navTo(this.dataset.v);});})();

$('saveName').addEventListener('click',function(){cfg({name:$('sName').value.trim()});});
$('savePass').addEventListener('click',function(){var p=$('sPass').value;if(p.length<6){toast('Minimum 6 characters');return;}cfg({pass:p},'Key updated').then(function(){$('sPass').value='';});});
$('resetCtr').addEventListener('click',function(){api('/api/resetctr',{}).then(function(){toast('Counters reset');}).catch(function(e){toast(e.message);});});
$('saveWifi').addEventListener('click',function(){var s=$('sSsid').value.trim(),p=$('sWpass').value;if(!s){toast('Network name is empty');return;}if(p.length<8||p.length>63){toast('Wi-Fi password must be 8-63 characters');return;}cfg({ssid:s,wpass:p});});
$('saveRadio').addEventListener('click',function(){var f=parseFloat($('sFreq').value);if(isNaN(f)||f<433.05||f>434.79){toast('Frequency must be between 433.050 and 434.790');return;}cfg({freq:Math.round(f*1000),relay:$('sRelay').checked?'1':'0',preset:$('sPreset').value,txp:$('sTxp').value},'Radio updated');});
$('doScan').addEventListener('click',doScan);
$('sTheme').addEventListener('change',function(){LS.s('theme',this.value);applyTheme();});
$('sSound').addEventListener('change',function(){LS.s('sound',this.value);unlockAudio();playSound('msg');});
$('sVib').addEventListener('change',function(){LS.s('vib',this.checked?'1':'0');if(this.checked)vib(60);});
$('testSound').addEventListener('click',function(){unlockAudio();playSound('msg');vib([100,50,100]);});
$('refStats').addEventListener('click',loadStats);
$('clrChat').addEventListener('click',function(){sheet({title:'Clear chat',text:'Cleared on this device only.',center:true,actions:[{label:'Clear',cls:'dg',fn:function(){api('/api/clear',{}).then(poll);}},{label:'Cancel'}]});});
$('wipeAll').addEventListener('click',function(){sheet({title:'Wipe everything',text:'This erases the key, settings and chat, then restarts the device. It cannot be undone.',center:true,actions:[{label:'Wipe everything',cls:'dg',fn:function(){api('/api/wipe',{}).catch(function(){});toast('Done. The device is restarting.');}},{label:'Cancel'}]});});
window.addEventListener('beforeinstallprompt',function(e){e.preventDefault();S.installEv=e;$('installBtn').style.display='';});
$('installBtn').addEventListener('click',function(){if(S.installEv){S.installEv.prompt();S.installEv=null;$('installBtn').style.display='none';}});
document.addEventListener('visibilitychange',function(){if(!document.hidden){S.unseen=0;document.title='LoRa Link';poll();}});
if(window.matchMedia){try{matchMedia('(prefers-color-scheme: light)').addEventListener('change',applyTheme);}catch(e){}}
applyTheme();
if('serviceWorker' in navigator){navigator.serviceWorker.register('/sw.js').catch(function(){});}
$('gpsOrigin').textContent=location.origin;
$('cpFlag').addEventListener('click',function(){copyText('chrome://flags/#unsafely-treat-insecure-origin-as-secure');});
$('cpOrigin').addEventListener('click',function(){copyText(location.origin);});
$('gpsTest').addEventListener('click',function(){if(!geoOK()){toast('Still blocked. Finish the setup steps first.');gpsUI();return;}toast('Getting location...');S.watch===null&&startWatch();navigator.geolocation.getCurrentPosition(function(p){setPos(p);gpsUI();toast('GPS works: +-'+Math.round(p.coords.accuracy)+' m');},function(e){toast('GPS: '+e.message);},{enableHighAccuracy:true,timeout:20000,maximumAge:0});});
$('gpsSet').addEventListener('click',function(){var p=parseLatLon($('gpsIn').value);if(!p){toast('Invalid coordinates');return;}setManual(p);toast('Position set');});
$('gpsSend').addEventListener('click',function(){var p=parseLatLon($('gpsIn').value);if(!p){toast('Enter valid coordinates first');return;}setManual(p);api('/api/loc',{mode:'msg',lat:p.lat,lon:p.lon,acc:0}).then(function(){toast('Location queued for sending');poll();}).catch(function(e){toast(e.message);});});
gpsUI();
$('txt').addEventListener('input',grow);
$('chat').addEventListener('scroll',function(){S.stick=$('chat').scrollHeight-$('chat').scrollTop-$('chat').clientHeight<70;updateFab();},{passive:true});
$('chat').addEventListener('click',function(){$('panel').classList.remove('on');$('quick').classList.remove('on');});
$('toBottom').addEventListener('click',function(){S.stick=true;scrollBottom(true);});
$('btnBack').addEventListener('click',function(){if(history.state&&history.state.c)history.back();else closeChat();});

$('btnTheme').addEventListener('click',function(){var d=document.documentElement,cur=d.getAttribute('data-theme');d.classList.add('tt');setTimeout(function(){d.classList.remove('tt');},450);LS.s('theme',cur==='dark'?'light':'dark');applyTheme();if($('sTheme'))$('sTheme').value=LS.g('theme','auto');});
(function(){var vv=window.visualViewport,r=document.documentElement;function fit(){r.style.setProperty('--vh',Math.round(vv?vv.height:window.innerHeight)+'px');r.style.setProperty('--vt',Math.round(vv?vv.offsetTop:0)+'px');if(window.scrollY||window.scrollX)window.scrollTo(0,0);if(S.stick&&S.chatOpen){var t0=performance.now();(function f(){scrollBottom(false);if(performance.now()-t0<320)requestAnimationFrame(f);})();}}
if(vv){vv.addEventListener('resize',fit);vv.addEventListener('scroll',fit);}else window.addEventListener('resize',fit);fit();})();
/* ---------- onboarding ---------- */
var ob={ssid:''};
function obShow(n){['ob1','ob2','ob3'].forEach(function(id,i){$(id).classList.toggle('on',i===n);});var d=document.querySelectorAll('#onb .dots i');for(var i=0;i<d.length;i++)d[i].classList.toggle('on',i===n);}
function maybeOnboard(){if(S.obShown||!S.me||!S.me.u)return;if(LS.g('ob','')==='1')return;if(!(S.me.kd||S.me.wd))return;S.obShown=true;$('obName').value=(S.me.n&&!/^Node-/.test(S.me.n))?S.me.n:'';$('obSsid').value=S.me.ssid||'';$('onb').classList.add('on');obShow(S.me.kd?0:1);}
function obFinish(restart){LS.s('ob','1');if(restart){$('ob3t').textContent='Restarting the device...';$('ob3p').textContent='Reconnect your phone to "'+ob.ssid+'" with the new password, then open '+location.origin+' again.';$('obDone').style.display='none';obShow(2);api('/api/restart',{}).catch(function(){});}else{$('ob3t').textContent='You are all set';$('ob3p').textContent='Your node is ready. Say hello in the public chat.';$('obDone').style.display='';obShow(2);}}
$('obNext1').addEventListener('click',function(){var n=$('obName').value.trim(),p=$('obPass').value;if(!n){toast('Enter your name');return;}if(p.length<6){toast('Passphrase needs at least 6 characters');return;}api('/api/cfg',{name:n}).then(function(){return api('/api/cfg',{pass:p});}).then(function(){$('obSsid').value=n.replace(/\s+/g,'-');obShow(1);poll();}).catch(function(e){toast(e.message);});});
$('obBack2').addEventListener('click',function(){obShow(0);});
$('obNext2').addEventListener('click',function(){var s=$('obSsid').value.trim(),w=$('obWpass').value;if(!s){toast('Wi-Fi name is empty');return;}if(w.length<8||w.length>63){toast('Wi-Fi password must be 8-63 characters');return;}api('/api/cfg',{ssid:s,wpass:w}).then(function(){ob.ssid=s;obFinish(true);}).catch(function(e){toast(e.message);});});
$('obKeep').addEventListener('click',function(){obFinish(false);});
$('obSkip').addEventListener('click',function(){LS.s('ob','1');$('onb').classList.remove('on');});
$('obDone').addEventListener('click',function(){$('onb').classList.remove('on');openConv('pub');});

function navTo(v){var st=history.state||{};if(v==='home'){if(st.v)history.back();else showView('home');}else if(st.v){history.replaceState({v:v},'');showView(v);}else{history.pushState({v:v},'');showView(v);}}
window.addEventListener('popstate',function(e){var st=e.state||{};if(S.chatOpen&&!st.c&&!isWide())closeChat();showView(st.v||'home');});
['btnSend','btnPlus','btnQuick'].forEach(function(id){var e=$(id);e.setAttribute('tabindex','-1');e.addEventListener('mousedown',function(ev){ev.preventDefault();});});
$('txt').addEventListener('beforeinput',function(e){if(e.inputType==='insertLineBreak'&&!S.sh){e.preventDefault();send();}});
$('txt').addEventListener('input',function(){LS.s('dr:'+S.conv,$('txt').value.trim());mentionSuggest();});
$('pSched').addEventListener('click',openSched);
$('btnSearch').addEventListener('click',function(){var b=$('sbar'),on=!b.classList.contains('on');b.classList.toggle('on',on);if(on)setTimeout(function(){$('sInput').focus();},260);else{S.search='';$('sInput').value='';$('sCnt').textContent='';renderChat();}});
$('sX').addEventListener('click',function(){$('btnSearch').click();});
$('sInput').addEventListener('input',function(){S.search=this.value.trim();$('sCnt').textContent='';S.noAnim=true;renderChat();S.noAnim=false;});
$('pinX').addEventListener('click',function(){var all=getPins(),l=all[S.conv]||[];l.pop();all[S.conv]=l;LS.s('pins',JSON.stringify(all));renderPin();});
document.addEventListener('input',function(e){var t=e.target;if(t&&t.id&&/^(sName|sSsid|sFreq|sPreset|sTxp|sRelay|sCap|sMtPre|sMtName|sMtHop|sMtSlot|sMtMode)$/.test(t.id))t.dataset.dirty='1';},true);
document.addEventListener('change',function(e){var t=e.target;if(t&&t.id&&/^(sPreset|sRelay|sCap|sMtPre|sMtMode)$/.test(t.id))t.dataset.dirty='1';},true);
document.addEventListener('focusin',function(e){var t=e.target;if(t&&t.closest&&t.closest('#v-settings,#onb')&&/^(INPUT|SELECT)$/.test(t.tagName)&&t.type!=='checkbox'&&t.type!=='range'){setTimeout(function(){try{t.scrollIntoView({block:'center',behavior:'smooth'});}catch(x){}},330);}});
$('sTxp').addEventListener('input',function(){$('txpV').textContent=this.value;});
$('sPreset').addEventListener('change',presetHint);
$('sCap').addEventListener('change',function(){cfg({cap:this.checked?'1':'0'},'Saved');});
$('sMtHop').addEventListener('input',function(){$('mtHopV').textContent=this.value;});
$('sMtMode').addEventListener('change',function(){var on=this.checked,box=this;
  sheet({title:on?'Switch to Meshtastic?':'Switch back to LoRa Link?',text:on?'The radio will use the Meshtastic protocol and frequency. Nodes running LoRa Link will no longer hear this device until you switch back. The node list is cleared.':'The radio returns to LoRa Link. Meshtastic nodes will no longer hear this device. The node list is cleared.',center:true,actions:[{label:'Switch',cls:'dg',fn:function(){cfg({mode:on?'1':'0'},on?'Meshtastic mode on':'LoRa Link mode on').catch(function(e){toast(e.message);});}},{label:'Cancel',fn:function(){box.dataset.dirty='';box.checked=!on;}}]});});
$('saveMt').addEventListener('click',function(){var u=$('sMtUrl').value.trim(),d;
  if(u){d={mturl:u};}else{d={mtpre:$('sMtPre').value,mtname:$('sMtName').value.trim(),mthop:$('sMtHop').value,mtslot:$('sMtSlot').value||'0'};var k=$('sMtPsk').value.trim();if(k)d.mtpsk=k;}
  cfg(d,'Meshtastic settings applied').then(function(){$('sMtPsk').value='';$('sMtUrl').value='';}).catch(function(e){toast(e.message);});});
$('sNightS').addEventListener('change',function(){LS.s('nightS',this.value||'19:00');applyTheme();});
$('sNightE').addEventListener('change',function(){LS.s('nightE',this.value||'06:00');applyTheme();});
$('chAdd').addEventListener('click',function(){var n=$('chName').value.trim(),p=$('chPass').value;if(!n){toast('Enter a channel name');return;}if(p.length<6){toast('Passphrase needs at least 6 characters');return;}var used={};(S.me.chs||[]).forEach(function(c){used[c.i]=1;});var slot=0;for(var i=1;i<=3;i++)if(!used[i]){slot=i;break;}if(!slot){toast('Maximum 3 channels');return;}api('/api/chan',{slot:slot,name:n,pass:p}).then(function(){$('chName').value='';$('chPass').value='';toast('Channel added');poll();}).catch(function(e){toast(e.message);});});
function rangeTest(){var n=+$('rtN').value,i=0,agg={},got=0,sent=0,out=$('rtOut'),wait=(S.me.pr===1?8000:4500);out.textContent='';$('rtGo').disabled=true;$('rtBar').style.width='0';
  function maxQ(){var q=0;S.msgs.forEach(function(m){if(m.q>q)q=m.q;});return q;}
  function done(){$('rtGo').disabled=false;$('rtBar').style.width='100%';$('rtP').textContent='Done: '+got+' replies to '+sent+' pings.';var names=Object.keys(agg);if(!names.length){out.appendChild(h('div','hint','No node replied. Check the frequency, passphrase, preset and distance.'));return;}
    var t=h('table','rtt'),hd=document.createElement('tr');['Node','Replies','Out dBm','Back dBm','SNR'].forEach(function(x){hd.appendChild(h('th',null,x));});t.appendChild(hd);
    names.forEach(function(k){var a=agg[k],tr=document.createElement('tr');[k,a.n+'/'+sent+' ('+Math.round(100-a.n*100/sent)+'% lost)',Math.round(a.o/a.n),Math.round(a.b/a.n),(a.s/a.n).toFixed(1)].forEach(function(x){tr.appendChild(h('td',null,String(x)));});t.appendChild(tr);});out.appendChild(t);}
  function step(){if(i>=n){done();return;}i++;sent++;$('rtP').textContent='Ping '+i+' of '+n+'...';$('rtBar').style.width=Math.round((i-1)*100/n)+'%';var base=maxQ();
    api('/api/ping',{}).then(function(){setTimeout(function(){poll();setTimeout(function(){S.msgs.forEach(function(m){if(m.d!=='S'||m.q<=base)return;var r=/^Reply from (.+?) \| out: (-?\d+) dBm, SNR (-?\d+) \(.*?\) \| back: (-?\d+) dBm/.exec(m.tx);if(r){var a=agg[r[1]]||(agg[r[1]]={n:0,o:0,b:0,s:0});a.n++;a.o+=+r[2];a.s+=+r[3];a.b+=+r[4];got++;}});step();},500);},wait);}).catch(function(){step();});}
  step();}
$('rtGo').addEventListener('click',rangeTest);
function isStandalone(){return (window.matchMedia&&matchMedia('(display-mode: standalone)').matches)||navigator.standalone===true;}
function doInstall(){if(S.installEv){S.installEv.prompt();return;}sheet({title:'Install LoRa Link',text:'Open the browser menu (three dots) and choose Install app or Add to Home screen.\n\nThe one-tap Install button works only when the browser treats this page as installable. That needs the secure-origin setup in Settings > Location & GPS.',actions:[{label:'Open setup guide',fn:openGuide},{label:'Close'}]});}
function renderInst(){var b=$('inst');b.textContent='';if(isStandalone()||LS.g('instX','')==='1')return;var c=h('div','inst'),ico=h('span','ico');ico.appendChild(svgI('dl'));c.appendChild(ico);var t=h('div','it');t.appendChild(h('b',null,'Install LoRa Link'));t.appendChild(document.createTextNode('Add it to your home screen for one-tap access.'));c.appendChild(t);var bi=h('button','btn pr','Install');bi.addEventListener('click',doInstall);var bx=h('button','ib');bx.appendChild(svgI('x'));bx.title='Hide';bx.addEventListener('click',function(){LS.s('instX','1');renderInst();});c.appendChild(bi);c.appendChild(bx);b.appendChild(c);}
window.addEventListener('appinstalled',function(){LS.s('instX','1');renderInst();});
renderInst();
if(mqW){var onmq=function(){if(isWide()){S.chatOpen=false;openConv(S.conv);}else if(S.chatOpen){closeChat();}};try{mqW.addEventListener('change',onmq);}catch(x){}}
renderHome();
if(isWide())openConv('pub');
poll();setInterval(poll,1500);
})();
</script>
</body>
</html>
)rawliteral";

const char SW_JS[] PROGMEM = R"rawliteral(
self.addEventListener('install',function(e){self.skipWaiting();});
self.addEventListener('activate',function(e){e.waitUntil(self.clients.claim());});
self.addEventListener('fetch',function(e){
  var u=new URL(e.request.url);
  if(e.request.method!=='GET'||u.pathname.indexOf('/api')===0||u.pathname==='/gpx')return;
  e.respondWith(fetch(e.request).then(function(r){var c=r.clone();caches.open('lora6').then(function(ch){ch.put(e.request,c);});return r;}).catch(function(){return caches.match(e.request);}));
});
)rawliteral";

const char MANIFEST_JSON[] PROGMEM = R"rawliteral(
{"name":"LoRa Link","short_name":"LoRa","start_url":"/","scope":"/","display":"standalone","orientation":"portrait","background_color":"#070914","theme_color":"#070914","lang":"en","dir":"ltr","icons":[{"src":"/icon-192.png","sizes":"192x192","type":"image/png","purpose":"any"},{"src":"/icon-512.png","sizes":"512x512","type":"image/png","purpose":"any"}]}
)rawliteral";

const uint8_t ICON192[] PROGMEM = {
  0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x00,0xc0,0x00,0x00,0x00,0xc0,
  0x04,0x03,0x00,0x00,0x00,0xa0,0xf2,0x71,0x34,0x00,0x00,0x00,0x18,0x50,0x4c,0x54,0x45,0x1b,0x34,0x38,0x15,0x18,0x1f,0x16,
  0x11,0x1b,0x0f,0x11,0x16,0x0f,0x11,0x15,0x0f,0x11,0x14,0x0e,0x11,0x15,0x11,0x0f,0x16,0x2a,0x1c,0x36,0x87,0x00,0x00,0x06,
  0x58,0x49,0x44,0x41,0x54,0x78,0xda,0xed,0x9b,0xcf,0x8e,0xdb,0x44,0x1c,0xc7,0xbf,0x33,0x5b,0xda,0x74,0x51,0x93,0xb1,0x2f,
  0x14,0x8a,0x14,0x27,0xcb,0x01,0x71,0xa1,0x94,0x17,0x80,0xaa,0xe7,0x15,0x4f,0x00,0x7d,0x84,0xed,0x93,0xd0,0x47,0x68,0x79,
  0x00,0x84,0x38,0xa3,0xc2,0x89,0x1b,0xa8,0x5c,0x10,0x87,0x36,0xf5,0x72,0xa0,0x20,0x55,0xeb,0x71,0x2a,0xd1,0x6c,0xca,0x7a,
  0x38,0xc4,0x59,0xcf,0xd8,0x9e,0x99,0xdf,0x38,0xe6,0x8f,0x2a,0xe7,0xb2,0xd9,0xc4,0xfe,0x7d,0xe6,0xf7,0x67,0x7e,0x7f,0x66,
  0xd7,0xec,0x16,0xfe,0xd9,0x17,0xc7,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0xf8,0x57,0x00,
  0x17,0x02,0xae,0x8d,0x70,0x22,0x01,0x60,0x0e,0x25,0xfb,0x07,0xb0,0x71,0xfa,0x43,0xf9,0x76,0x81,0x03,0xcc,0x32,0xea,0x7d,
  0xb7,0x88,0xe2,0x17,0xa9,0x69,0xda,0x19,0x11,0x41,0x03,0x88,0xc7,0x69,0xd3,0x7b,0x37,0x26,0x14,0x43,0xed,0xcd,0x09,0x8b,
  0x50,0xdf,0xb7,0x88,0x52,0xbf,0xf1,0x37,0x57,0xbd,0x44,0x51,0xf4,0xec,0xc7,0xf6,0x2f,0x1e,0x3f,0x88,0xfa,0x00,0x44,0x8f,
  0x52,0xdb,0x57,0xc5,0x37,0xf9,0xee,0x00,0x87,0x7c,0x00,0x8f,0xf2,0x5d,0x01,0x6e,0xf9,0x04,0x82,0x07,0xc0,0x3c,0xf2,0xfd,
  0x04,0x37,0x80,0x3d,0x4b,0xbd,0x46,0x5e,0x88,0xee,0x00,0x36,0xf6,0xcb,0x47,0xf1,0xad,0xe8,0x0c,0x18,0x3f,0xa0,0xec,0xc2,
  0xe2,0xbb,0xae,0x00,0xb6,0xa0,0xa5,0x9b,0xb3,0xbc,0x1b,0x80,0xbd,0x9d,0xd2,0x00,0x4e,0x37,0x38,0x00,0xe3,0xaf,0xa8,0x99,
  0xd6,0x65,0x24,0xbe,0xb3,0x81,0x00,0xe0,0x6c,0xd6,0x01,0xf0,0x57,0x4a,0x07,0xe0,0x4b,0x11,0x0c,0xe0,0x0f,0x43,0x0a,0x63,
  0x71,0x1c,0x0c,0xb8,0x16,0x56,0x7a,0xad,0x7e,0xb6,0xd5,0x03,0xfe,0x20,0x0c,0xa0,0xf6,0x46,0x61,0x1a,0x04,0x2a,0x60,0x57,
  0xc1,0xa2,0x41,0xa8,0x02,0x76,0x15,0x78,0x80,0x02,0x2c,0x9a,0x64,0x59,0x96,0xc9,0x48,0x04,0xa8,0x60,0xd1,0xe0,0xdb,0x96,
  0xca,0xf0,0x22,0xcb,0x36,0xa5,0x39,0x93,0xa3,0xb8,0x59,0x8d,0x15,0x1f,0xd1,0x35,0xb8,0xd2,0xf8,0xe4,0xa2,0x5a,0x68,0x85,
  0x5f,0x2e,0x64,0xb3,0x1c,0xa7,0x74,0x13,0xb1,0xc6,0xb5,0xe2,0x8f,0x5a,0x5f,0xa1,0x16,0xb2,0x6e,0x92,0xb3,0x84,0x0c,0xa8,
  0x97,0x01,0xa6,0x9e,0xb4,0x78,0x35,0xad,0x13,0xbe,0xa6,0x03,0x6a,0xf2,0x8b,0xd6,0x0e,0x4b,0x3d,0x61,0x35,0x15,0x04,0xd1,
  0xc9,0xb5,0x18,0x65,0x27,0xb6,0xfe,0xea,0xc5,0x65,0x7f,0xa4,0x72,0xaf,0x8b,0x59,0x61,0x0f,0xfe,0xec,0xb6,0xf1,0xeb,0x31,
  0xad,0xbb,0xae,0xe5,0x69,0xcd,0x3e,0x62,0x99,0x00,0xd0,0x1d,0xf2,0xb9,0x11,0x4c,0x2f,0x93,0x94,0x62,0x22,0xf6,0xb3,0x11,
  0x3f,0xbf,0x6f,0xdf,0xc5,0x57,0x4f,0x47,0xab,0xd5,0x6a,0xb5,0xba,0xbc,0x7f,0x15,0x5b,0xab,0xad,0x0d,0xab,0xbc,0x1c,0x51,
  0x4c,0x64,0x9a,0x64,0xbb,0xdc,0x78,0x56,0x4d,0x1d,0x4a,0xb2,0x3b,0xdb,0x8b,0xf3,0xb6,0x8b,0xdd,0x80,0x13,0xc3,0x71,0xe5,
  0xcf,0x59,0x6d,0xa8,0xb9,0x17,0x97,0x21,0x53,0x08,0x77,0x1c,0x35,0x01,0x5c,0xb7,0xe3,0x67,0x1b,0xb1,0x6c,0x26,0x9b,0x99,
  0xa1,0x14,0x66,0x6c,0x87,0x63,0x02,0xc0,0x88,0xa1,0xbb,0x1b,0xf9,0x49,0xdb,0x7c,0xc0,0x36,0x3b,0x57,0x1d,0x07,0x02,0xc6,
  0x0d,0x03,0xb5,0xca,0x07,0x90,0x1f,0x35,0x8c,0xd4,0xb4,0x51,0x03,0xc0,0xf4,0x5a,0x2c,0x5d,0xf2,0x81,0xfb,0x49,0x7d,0xd9,
  0x05,0x01,0x50,0xb9,0x80,0x6d,0xc6,0xbc,0xc4,0x3e,0x8a,0xe5,0xa2,0x1e,0x76,0x3f,0x79,0x01,0x57,0xea,0xc6,0x8a,0x74,0xf9,
  0x51,0x14,0x45,0xcd,0xdb,0xa5,0xc3,0x09,0x17,0xec,0x2e,0xd8,0xe8,0xa2,0x25,0xb4,0x8b,0xa7,0x32,0x03,0xc0,0xd4,0xfc,0x7c,
  0x82,0x55,0x49,0xaa,0xc5,0x32,0x80,0x33,0x21,0xdd,0x1a,0x68,0x2e,0xd8,0xa0,0x8e,0xce,0xbf,0x41,0x59,0x13,0x14,0xb4,0x5a,
  0xb0,0x14,0x00,0x90,0xdb,0x9d,0x50,0x4f,0x15,0x5a,0x9e,0x58,0xad,0x00,0xf0,0x5f,0xb6,0x29,0xe3,0xa9,0x9e,0x53,0xe5,0xfe,
  0xf9,0x0d,0x2b,0x00,0xaa,0x1a,0x68,0xd7,0x23,0xb7,0x06,0x85,0x19,0x42,0x6c,0xb2,0x95,0x5f,0xcb,0x02,0x27,0xe7,0x8b,0x4e,
  0x4c,0xd3,0x1f,0x7b,0x9c,0x5c,0xb9,0x60,0x02,0x00,0xc2,0x22,0x5f,0x4b,0x42,0x4b,0xd3,0x32,0xca,0x03,0x10,0x86,0x8b,0x99,
  0x23,0x8b,0x6d,0xd3,0xb8,0x4a,0x8c,0x40,0x52,0xc2,0x0d,0x78,0x68,0xa8,0xc2,0x2c,0xcb,0x32,0x44,0x2d,0x0d,0x40,0xe1,0xd6,
  0xa0,0xda,0x66,0x12,0x00,0xa6,0xa5,0xb5,0xf4,0xd0,0x63,0xf5,0x3e,0x45,0x09,0x63,0x09,0xd2,0xb9,0x0f,0xcc,0x32,0xce,0xa5,
  0xe9,0x80,0x58,0x64,0x00,0xd8,0xa4,0x6c,0xc0,0xd4,0x9f,0xfb,0x2d,0x37,0xe5,0x13,0x97,0x06,0x67,0x15,0x4a,0x6c,0x15,0x60,
  0xa5,0x7c,0x36,0x53,0x59,0x59,0x6e,0xca,0x81,0xe6,0xb4,0x5a,0xd5,0x1d,0x5b,0xff,0xc5,0xad,0xa9,0x94,0x83,0x49,0xfd,0xb3,
  0x58,0xdb,0xa3,0x72,0xa6,0x9b,0x43,0x25,0x88,0xef,0x59,0x0a,0xa2,0x63,0x84,0x52,0x09,0xd3,0xdd,0x12,0x1b,0x8e,0x96,0x6f,
  0xe8,0xce,0xcf,0x2f,0x29,0xea,0x8c,0xa6,0xeb,0xb7,0x9c,0x68,0x0a,0xc4,0x35,0x11,0xeb,0x58,0xf7,0xe8,0x3e,0xac,0x19,0xdb,
  0x31,0xc6,0x2a,0x4d,0x01,0xde,0x58,0xa2,0x8a,0xac,0xf1,0xeb,0x00,0x34,0xbb,0xde,0xd2,0xa4,0xd3,0xb6,0x83,0xbc,0x96,0xa0,
  0x6c,0x7a,0x99,0x76,0x30,0x1b,0xb7,0xd5,0x9c,0x4b,0x00,0xa0,0x6e,0x07,0x69,0xd0,0xd2,0x25,0x4a,0x00,0x4c,0x55,0x43,0x4e,
  0x35,0xde,0xac,0x93,0xaa,0x2d,0xb0,0x37,0x90,0xbe,0x83,0xd9,0x49,0xa6,0x25,0x28,0x56,0x64,0x27,0x80,0xd8,0xd6,0xb8,0x25,
  0x45,0x79,0x9f,0x89,0xa4,0xb6,0x53,0xc5,0xe6,0x64,0x59,0x6e,0xdb,0x76,0x95,0x04,0x03,0xc6,0xcd,0xef,0xc5,0xb9,0x02,0x55,
  0xca,0xde,0x16,0x83,0x17,0x9b,0x0d,0xef,0x6c,0x3d,0x7d,0x1a,0xa8,0x4b,0x6d,0x8d,0x67,0x99,0xaa,0xd7,0x02,0xc9,0xae,0x26,
  0xc2,0x7a,0x56,0x6e,0x68,0x23,0xe4,0xcb,0xe0,0x61,0x4c,0xee,0x0c,0x40,0x59,0xe0,0xc7,0xa6,0xa8,0xbb,0xb5,0xfa,0x64,0xcf,
  0xc8,0xed,0xf5,0xcc,0x3b,0x78,0x3a,0xb6,0xb0,0x59,0xd3,0x78,0x7b,0x3d,0xb3,0x76,0xac,0xc2,0x52,0x57,0x3a,0x3b,0xd9,0xdc,
  0xff,0xec,0x0e,0x8b,0xe2,0x19,0x68,0x59,0x28,0x08,0xa0,0xca,0x92,0x70,0x0f,0x50,0x65,0xaa,0xce,0x7b,0x05,0x08,0x00,0xd8,
  0x66,0xfd,0xf5,0x2c,0x40,0x05,0x22,0x20,0x07,0xc0,0xde,0x3a,0xb7,0xff,0x91,0x33,0x20,0x3a,0xf9,0x20,0x01,0xb4,0x92,0xf9,
  0x45,0x3d,0x18,0x77,0x07,0xe4,0x42,0x97,0xa8,0x8e,0xaa,0xa6,0xb8,0x27,0x40,0x6d,0xc5,0xf7,0xb5,0x2a,0xdf,0x13,0xa0,0x66,
  0x73,0x6a,0x94,0xbe,0x72,0x7f,0x2c,0x4d,0x7a,0x11,0x29,0xff,0xd7,0x26,0x8a,0xa2,0xc0,0x1b,0x2e,0x04,0x5d,0xcd,0x8a,0x85,
  0x63,0x2e,0x27,0x68,0xe0,0xd3,0xe7,0x53,0xd9,0x76,0x18,0x58,0x9f,0x2b,0x1d,0x22,0x7d,0x19,0xf2,0x6e,0xe3,0xf4,0xc3,0x9a,
  0x79,0xdb,0x01,0x8a,0x74,0x6b,0xd1,0xdd,0x44,0xee,0x0c,0x76,0x91,0x66,0xfc,0xa4,0x33,0xe0,0xb2,0xbf,0x5c,0x7a,0x4d,0x94,
  0xf8,0xba,0xbc,0xd6,0x51,0xd5,0x95,0xb4,0x42,0xf6,0x01,0x31,0x3c,0x9d,0x00,0x41,0x4b,0xd8,0x29,0x39,0x53,0x84,0x00,0x7a,
  0x49,0x15,0xcb,0xdd,0x25,0x32,0xf7,0xa4,0xff,0x5f,0x02,0x14,0xcd,0x92,0x53,0x77,0x36,0x4d,0x48,0xe1,0xe1,0x02,0x5c,0x77,
  0x02,0x9c,0x11,0x2e,0x48,0x01,0x2b,0x89,0xe7,0x45,0x96,0xee,0xcb,0x6b,0x48,0xee,0x01,0x70,0x8a,0xfb,0x12,0x72,0xb2,0x6e,
  0x0a,0x7c,0xee,0xf2,0x72,0x29,0xd8,0x39,0xd6,0x4c,0x3d,0x00,0xe5,0x6e,0xef,0xfc,0xad,0xc1,0xc4,0x07,0x48,0xbc,0x31,0xee,
  0x9e,0xcb,0x84,0xcf,0xe6,0xee,0xce,0x65,0x0e,0xc4,0x02,0x01,0x3e,0x6e,0x16,0x7d,0x77,0xb2,0xc8,0x22,0x4f,0xd5,0xe3,0xe1,
  0x1f,0x84,0xbd,0xa6,0x5e,0x79,0xa4,0xf3,0x01,0x6a,0xbd,0x6c,0x5d,0xf0,0x4e,0x80,0x86,0x0b,0x5a,0x00,0x3b,0x65,0x6c,0x4e,
  0xf8,0x84,0xed,0xa2,0xc2,0xfb,0x94,0xde,0xf4,0xfa,0x0e,0x0a,0x08,0x0a,0xe0,0xd7,0x3e,0x5d,0xd0,0x06,0xe0,0x3d,0x06,0x69,
  0xab,0x34,0x75,0xbd,0x47,0x0b,0xb5,0x2e,0x77,0xaf,0xcf,0x61,0x83,0x93,0x76,0x4b,0xe7,0x18,0xea,0x17,0xb0,0x47,0x1e,0xa1,
  0x9e,0xf7,0x94,0x26,0xec,0x21,0xf3,0x49,0x27,0x0f,0x04,0x00,0x3a,0xa5,0x8b,0x3d,0x19,0x00,0xe8,0xe2,0x85,0xc3,0x80,0x31,
  0x56,0xcd,0xc3,0xe5,0xbf,0x96,0x86,0xcc,0xc9,0x1d,0x54,0x38,0x0c,0x1a,0xc4,0xc3,0x55,0xb0,0x28,0x60,0x4d,0x3c,0xc1,0x2a,
  0x1c,0x06,0x1e,0x25,0x84,0xaa,0x60,0x53,0xc0,0x9e,0x3a,0x03,0x55,0x38,0x0c,0x3e,0x0c,0x09,0x53,0xe1,0x9d,0x34,0xfc,0xb4,
  0x25,0x0f,0xd8,0xce,0x7c,0x6a,0xdf,0x7e,0xf6,0x85,0x3e,0x27,0x4e,0xf6,0x00,0x0e,0xc2,0x52,0x78,0xb0,0x91,0xf6,0x26,0x9d,
  0x00,0x58,0x12,0x8d,0xc4,0x3f,0x72,0xc1,0x5d,0xcb,0x24,0x1a,0xe9,0x20,0xb4,0xca,0x55,0x46,0x3a,0x20,0x45,0xd0,0xa4,0x33,
  0x00,0xf2,0x26,0xc1,0x01,0x53,0x74,0x07,0x60,0x79,0xd3,0xef,0x00,0xb9,0x0b,0x40,0xf9,0x08,0xfc,0x63,0x89,0x5d,0x00,0x3e,
  0x82,0x57,0xbe,0xbf,0x8d,0x53,0xcb,0x1b,0x8e,0xf8,0xf1,0xca,0xa7,0x3c,0xa2,0xc1,0xde,0xb3,0x3d,0x15,0x73,0x40,0x38,0x42,
  0xa5,0x3c,0x03,0xb2,0xba,0x26,0x9e,0xb6,0x29,0xff,0xe1,0x88,0xf0,0x0c,0x08,0x05,0x80,0x15,0x7f,0xb7,0xa1,0x04,0x9f,0x7f,
  0xb0,0x22,0xa5,0x11,0x5a,0xc2,0x39,0x7d,0xdd,0x44,0xf0,0xf9,0x0d,0x90,0xe4,0x53,0x1f,0xf4,0x01,0xc0,0xc4,0x82,0x9d,0xa5,
  0x9b,0x61,0x79,0xf3,0x2f,0x22,0x3d,0x03,0x00,0x60,0x73,0xf4,0x9e,0x85,0xdc,0x12,0x76,0xfa,0x1e,0x24,0xba,0x97,0xb9,0x7b,
  0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0xe0,0x95,0x01,0xfc,0x0d,0x09,0x45,0xd3,0xbd,0xc8,0x23,0x1e,
  0x05,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82,
};
const size_t ICON192_LEN = 1717;

const uint8_t ICON512[] PROGMEM = {
  0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a,0x00,0x00,0x00,0x0d,0x49,0x48,0x44,0x52,0x00,0x00,0x02,0x00,0x00,0x00,0x02,0x00,
  0x04,0x03,0x00,0x00,0x00,0x06,0x56,0xc9,0xc9,0x00,0x00,0x00,0x18,0x50,0x4c,0x54,0x45,0x1a,0x33,0x38,0x15,0x18,0x1f,0x16,
  0x11,0x1b,0x0f,0x11,0x16,0x0f,0x11,0x15,0x0f,0x11,0x14,0x0e,0x11,0x15,0x11,0x0f,0x16,0xdd,0x80,0xcb,0x0e,0x00,0x00,0x12,
  0x4e,0x49,0x44,0x41,0x54,0x78,0xda,0xed,0x9d,0xdd,0x6e,0x1b,0xc7,0x15,0xc7,0xcf,0xac,0x64,0xc9,0x71,0x10,0x69,0xc8,0x06,
  0xb0,0xeb,0x14,0xe1,0x92,0x56,0x2f,0xd2,0x02,0xb1,0xec,0xbc,0x40,0x93,0xe6,0xba,0xf0,0x13,0x24,0x7e,0x83,0xea,0x51,0xf4,
  0x08,0x71,0x9f,0x20,0xc8,0x75,0xe0,0x06,0x05,0x1a,0xf4,0x22,0x70,0xed,0x9b,0xb6,0x40,0x1d,0x69,0xd5,0xc2,0x0d,0x8c,0x06,
  0xdc,0x21,0x03,0xd8,0xb4,0x5c,0xed,0xf4,0x42,0xb6,0x2c,0x92,0xfb,0x71,0x66,0xe6,0xcc,0xec,0x2c,0x75,0xe6,0xce,0xf2,0x72,
  0xb9,0xe7,0x37,0xff,0xf3,0x31,0xb3,0x67,0x97,0xe2,0x53,0xb8,0xd8,0x23,0x01,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,
  0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,
  0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,
  0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,
  0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,
  0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0xdd,0x1d,0xeb,0x2d,0x7d,0xaf,0x90,0x00,0x63,0xf5,0xfa,0x5f,0x23,0x00,0xad,0x2e,0x0e,
  0x00,0x21,0x8b,0x4c,0x7e,0x7d,0xfe,0x2f,0x07,0x00,0x49,0x2a,0x86,0x90,0xb7,0x70,0x31,0xa1,0x5f,0xa4,0xd4,0xd3,0x87,0x45,
  0x56,0xe5,0x8f,0xbb,0xd3,0x34,0x5f,0x69,0x00,0x42,0x1e,0x9e,0x64,0xf5,0x87,0x8c,0xfa,0xdb,0x6a,0x55,0x01,0xf4,0x0a,0xf5,
  0x00,0x13,0x97,0x87,0xc3,0x7c,0x15,0x01,0xf4,0xc6,0xe3,0x0c,0x7b,0xec,0x68,0x94,0xaf,0x1a,0x00,0x51,0xe0,0xcd,0x0f,0x8a,
  0x60,0x6d,0x14,0xc4,0xfc,0xed,0x27,0x7f,0x37,0xf3,0xec,0x3c,0x7b,0xeb,0xda,0x6c,0x65,0x00,0xf4,0x9e,0x7d,0x6b,0x1c,0xd8,
  0x74,0x7e,0x74,0x5d,0xce,0x56,0x02,0x80,0xd0,0x7f,0xfe,0xc1,0xe6,0x73,0xfa,0x3f,0xc9,0xcf,0xfd,0x13,0xf0,0x5f,0x0a,0xcb,
  0x1f,0x1f,0xd8,0x7e,0xf4,0xfb,0xfb,0xa2,0xf3,0x00,0x84,0xfc,0x3e,0xb3,0xff,0x74,0xf1,0x9d,0x92,0xdd,0xce,0x02,0xa2,0x78,
  0xe0,0x3a,0x43,0xb7,0x75,0x87,0x15,0xe0,0x20,0xff,0x33,0x11,0x3c,0x10,0xdd,0x05,0xd0,0x73,0x91,0xff,0x1b,0x37,0x98,0xf8,
  0x74,0x03,0x9f,0xab,0xc1,0xed,0xaf,0x69,0xce,0xf3,0x18,0x06,0xfe,0x96,0x07,0xfe,0xd2,0xa0,0xd0,0xdf,0x52,0x9d,0x6a,0x9c,
  0xf8,0x2b,0x8a,0xbc,0xb9,0x80,0xd8,0x7a,0x40,0x77,0xb2,0xef,0xff,0x28,0xbb,0x06,0x40,0x6c,0xdd,0xa7,0x3c,0x5d,0xe1,0x8d,
  0x40,0xd2,0x09,0xfb,0x3d,0x12,0x48,0xba,0x61,0xbf,0x3f,0x02,0x49,0x47,0xec,0xf7,0x46,0x20,0xe9,0x8a,0xfd,0xbe,0x08,0xf8,
  0x00,0xe0,0xc7,0x7e,0x4f,0x04,0x3c,0x00,0xd8,0xf6,0x64,0x3f,0x40,0x71,0x44,0x4f,0x60,0xbd,0x43,0xf6,0x03,0x3c,0x86,0xed,
  0xe8,0x15,0xd0,0x3b,0xf0,0xb9,0xba,0x78,0x3c,0x89,0x1d,0x80,0x78,0x9c,0x81,0x57,0x02,0x22,0x6e,0x00,0xe2,0x7f,0x7e,0xed,
  0x07,0x78,0x20,0xa3,0x06,0xb0,0xf5,0xd0,0xb3,0xfd,0xe4,0xa9,0x80,0x16,0x80,0xcf,0x00,0xf8,0x26,0x15,0xc4,0x0b,0x40,0x1c,
  0x40,0x80,0x41,0x1b,0x08,0x29,0x01,0x88,0x1f,0xb3,0x10,0x00,0xe0,0x40,0x46,0x0a,0x60,0x2b,0x8c,0xfd,0xb4,0x61,0x80,0x10,
  0x40,0xff,0x3e,0x04,0x1a,0x94,0x61,0x80,0x0e,0x80,0xf8,0x27,0x04,0x1b,0x8f,0x63,0x04,0x10,0xca,0x01,0x00,0x00,0xe0,0xaf,
  0x32,0x3a,0x00,0xe1,0x1c,0x80,0xd6,0x09,0x92,0x0e,0x3a,0x00,0xa9,0x13,0x24,0x5d,0x74,0x00,0x4a,0x27,0x20,0x02,0x90,0xdc,
  0x0f,0x6c,0x3f,0x99,0x13,0xd0,0x00,0x10,0xff,0x85,0xe0,0x83,0xa8,0x1c,0x4a,0xba,0xe9,0x00,0x00,0x50,0xfc,0x29,0x1e,0x00,
  0x61,0xd6,0x00,0x8b,0xe3,0xe5,0x24,0x1a,0x00,0xbf,0xc8,0xda,0x00,0x40,0xe3,0x04,0x14,0x37,0x47,0xc3,0x47,0xc0,0xd3,0xa1,
  0xd7,0x2e,0x47,0xa1,0x00,0xf1,0x1e,0xb4,0x34,0x28,0x24,0x40,0x00,0x60,0xeb,0xcb,0xb6,0x00,0x14,0xdf,0xc4,0x00,0xa0,0x9d,
  0x08,0x78,0x3a,0x4e,0xd2,0x08,0x00,0xb4,0x91,0x02,0xcf,0xc6,0x57,0xed,0x03,0x68,0x53,0x00,0x00,0x2f,0xd3,0xd6,0x01,0xb4,
  0x2a,0x00,0x02,0x09,0x24,0x9d,0x16,0x00,0x81,0x04,0x92,0x6e,0x0b,0xc0,0x5d,0x02,0x49,0xb7,0x05,0xe0,0x2e,0x81,0xa4,0xe3,
  0x02,0x70,0x96,0x40,0xd2,0x71,0x01,0x38,0x4b,0x20,0xe9,0xba,0x00,0x5c,0x25,0x90,0x74,0x5d,0x00,0xae,0x12,0x48,0x3a,0x2f,
  0x00,0x47,0x09,0x24,0x9d,0x17,0x80,0xa3,0x04,0x5c,0x7a,0x84,0xb6,0xbe,0x73,0xa1,0x27,0x41,0x1f,0xbe,0x9e,0x85,0xd4,0xed,
  0xd9,0xe9,0xaf,0x3e,0x6c,0x05,0x80,0xbd,0x00,0x84,0x1c,0x2b,0x18,0x9f,0x5b,0xd6,0x1e,0x00,0x48,0x35,0xb2,0x86,0xf0,0x32,
  0xb5,0xf7,0x45,0x87,0x1d,0xa1,0xe4,0x6f,0x76,0x9f,0xeb,0xfd,0xf0,0x3c,0x5f,0xee,0x7e,0x9f,0x41,0xae,0x2e,0x27,0x96,0x0f,
  0xca,0xbd,0xbc,0xdc,0x46,0x0c,0xb0,0xda,0x08,0x12,0xdb,0xba,0x5a,0x38,0x6a,0x7c,0xa8,0x7a,0x36,0x67,0x75,0xd8,0x1a,0xb2,
  0x57,0x80,0xcd,0x4e,0xe0,0xc6,0x49,0xae,0x1a,0xe6,0x38,0x9f,0x5d,0x37,0x57,0x81,0xc3,0xee,0x60,0x12,0x50,0x00,0x1b,0xf0,
  0x14,0xf3,0x40,0xf0,0x41,0xde,0x0b,0x28,0x01,0x6b,0x00,0xe2,0xa1,0x71,0xe4,0x7b,0x8a,0x7d,0x1e,0xfa,0x40,0xdd,0x35,0x3c,
  0x79,0x61,0x0d,0xc0,0xfa,0xb9,0x41,0x6d,0xf8,0x44,0x8c,0x3c,0x34,0x3a,0xbc,0x6f,0xf8,0xb4,0xe0,0xda,0x6e,0x68,0x05,0x14,
  0x66,0x9c,0xb5,0x99,0xfd,0x30,0x36,0x8c,0x86,0x27,0xb6,0x12,0xb0,0x0d,0x82,0x89,0x91,0x00,0xc4,0xd8,0x3c,0xb0,0xe5,0x57,
  0xcc,0x0c,0xb9,0x1c,0x56,0x01,0x46,0x21,0x50,0x8e,0x6d,0xbe,0x62,0x6c,0xf4,0xdc,0xb0,0x6d,0x18,0xb4,0x04,0x60,0x12,0x02,
  0x8d,0xe5,0x7f,0x16,0x66,0x32,0x03,0xa3,0x8a,0xb0,0x00,0x0c,0xd6,0x81,0xa2,0xb0,0x2e,0xf3,0xf5,0xa1,0x41,0x6f,0xf8,0x57,
  0x61,0x01,0x18,0xb8,0xbf,0xcb,0x32,0x67,0x8c,0x27,0x60,0x19,0x06,0xed,0x00,0x24,0x5f,0xfa,0x75,0x7f,0x1b,0x02,0x96,0x3d,
  0x33,0x76,0x00,0xde,0xf1,0x94,0xfd,0xcb,0x08,0xa0,0xdb,0x20,0x02,0x02,0x40,0x2f,0x84,0xdd,0xed,0x07,0x28,0xb0,0x04,0xec,
  0xf6,0x45,0xec,0x00,0x64,0xe1,0xec,0x37,0x20,0xf0,0x28,0x18,0x00,0x6c,0x11,0x40,0x62,0x3f,0x9e,0x80,0x55,0x29,0x60,0xb5,
  0x23,0x84,0x2b,0x02,0x36,0x9e,0x36,0x1d,0x31,0x3a,0x2b,0x79,0x44,0x6d,0xe9,0x5f,0x08,0xd4,0xca,0xc0,0x2a,0xdf,0xda,0x2c,
  0x86,0x70,0xeb,0x20,0x31,0xae,0xab,0x8d,0x46,0x0b,0xef,0x0e,0x14,0xb2,0x36,0x5d,0xe2,0x16,0x06,0xeb,0x37,0xc3,0xb8,0xc0,
  0xfb,0xb8,0xfa,0xa7,0xfa,0xbf,0x46,0xb2,0x97,0x2f,0x2c,0x8d,0x75,0x2e,0x7a,0xa3,0xb4,0x52,0xc3,0x39,0x6a,0x7d,0x6c,0x53,
  0x0d,0xda,0x2c,0x86,0x0e,0x31,0x52,0xab,0xda,0xe1,0x14,0xfd,0x6b,0x50,0xb1,0x32,0x9a,0xbd,0x10,0xd7,0xab,0xb6,0x0c,0xfe,
  0xf2,0x16,0xe6,0x3b,0xfb,0x41,0x14,0xa0,0x31,0x39,0x40,0x96,0xdb,0x2f,0xae,0xca,0xda,0xbd,0xdf,0xbc,0x57,0x35,0x21,0xa8,
  0x40,0xf8,0x28,0x08,0x00,0x8c,0x07,0x54,0x24,0xc0,0x44,0x1e,0x37,0xae,0x82,0x7b,0xc3,0x52,0x21,0x17,0xcf,0xfc,0xf8,0x80,
  0x39,0x00,0xcc,0x42,0x70,0xa3,0xd4,0x7e,0x31,0x44,0x3d,0xfa,0xac,0xc4,0xb0,0xec,0xcf,0x2f,0x10,0x45,0x71,0x11,0x42,0x01,
  0x08,0x0f,0x10,0x2f,0xca,0xfe,0xda,0x97,0xd8,0x34,0xa5,0xfa,0x65,0x33,0x39,0x46,0x04,0xc2,0x2c,0x00,0x00,0xc4,0x3a,0xe0,
  0x33,0x55,0x36,0xfd,0x06,0xbb,0x7c,0xba,0x54,0x04,0xfb,0xcd,0x02,0x3f,0xf4,0x0f,0x00,0x53,0x06,0xef,0xbb,0x4c,0xff,0x2b,
  0x11,0x94,0x10,0xd0,0xcd,0xcb,0x9d,0x22,0xf5,0xaf,0x80,0x66,0x00,0x25,0x53,0xdd,0x37,0x7e,0x25,0x5c,0x99,0x1b,0x20,0x62,
  0xdc,0x23,0xef,0x00,0x9a,0xe3,0xcc,0xe7,0xcb,0x73,0x3d,0xb4,0x78,0x25,0x9e,0x4e,0x96,0x67,0xb3,0x79,0x8f,0xec,0xd0,0x3b,
  0x80,0xb1,0xb9,0x03,0x88,0xa1,0x4d,0x91,0x0e,0x7a,0x92,0x9a,0x3b,0x81,0xf1,0xbe,0x50,0x42,0x1e,0x02,0x96,0x26,0x5b,0xa4,
  0x56,0xf6,0x03,0xc0,0x32,0x81,0x66,0x27,0xc8,0x3c,0x03,0x28,0x8c,0x1d,0xc0,0xde,0xfe,0x32,0x02,0x8d,0x4e,0x70,0xe4,0x19,
  0xc0,0xfb,0xa6,0x0e,0xe0,0x62,0x7f,0x09,0x81,0x46,0x27,0x30,0x2d,0x06,0x0d,0x01,0x34,0x96,0x81,0x8b,0x0e,0x90,0x38,0xd9,
  0x0f,0x30,0x19,0x1a,0x1a,0x68,0x0a,0xc0,0x70,0x35,0xd8,0xd8,0x15,0xb2,0x68,0xae,0xf3,0xbb,0x91,0x67,0x97,0x17,0xce,0x30,
  0x6d,0xb8,0x07,0x66,0xd8,0x2d,0x62,0xa8,0x80,0x86,0x32,0x50,0xe4,0x8b,0xf1,0x1f,0x9c,0xc7,0x62,0x36,0x2c,0x68,0x83,0x80,
  0x21,0x80,0x2d,0xb3,0xff,0x96,0xca,0x1d,0x80,0x9e,0x34,0x88,0xcc,0x2d,0x11,0x26,0x94,0x21,0x60,0x31,0x47,0xf6,0x81,0x64,
  0x5c,0x6d,0x2c,0x34,0x1d,0x82,0x80,0x21,0x80,0xcc,0x24,0x47,0x6e,0x12,0xbd,0x12,0xf9,0x78,0xc1,0x91,0x72,0x49,0x58,0x0d,
  0x27,0x94,0x21,0x60,0x5e,0x9c,0xe2,0x3a,0x10,0x0d,0xb5,0x67,0xe2,0xe6,0x47,0x1e,0x01,0x6c,0x99,0xa4,0xc0,0x3d,0x45,0x05,
  0x00,0xfe,0x20,0x0d,0x54,0x6e,0x16,0x04,0x12,0xc2,0x10,0x30,0x6f,0x70,0xff,0x0b,0x32,0xfb,0x41,0x6f,0x1a,0x4c,0x72,0xe1,
  0x11,0x40,0x86,0x17,0xc0,0x26,0xe9,0x3b,0xd1,0x17,0xc2,0x40,0xbd,0x8d,0x8f,0xbc,0x01,0x78,0x07,0x2f,0x00,0x71,0x05,0x48,
  0x87,0x4a,0xf1,0x12,0x30,0x0a,0x02,0xeb,0x64,0x21,0x60,0x7e,0xc6,0x11,0x32,0xec,0xc1,0xab,0x5f,0x5c,0x12,0x43,0x44,0xb3,
  0xf8,0x74,0x41,0x02,0xaa,0x2e,0x08,0x28,0x3f,0x00,0xea,0x43,0xc0,0xdc,0x97,0x6e,0x36,0x9d,0x4a,0x1e,0xc8,0xb3,0x7b,0xec,
  0xfa,0x00,0x00,0x9a,0x7e,0x5f,0x49,0x0f,0xe7,0xf6,0x3a,0x8e,0xb6,0xeb,0x82,0x80,0xf2,0xe3,0x02,0xb5,0x21,0x40,0x1b,0x38,
  0x40,0x4f,0x8f,0x0f,0x16,0x0b,0xba,0xf1,0xa1,0xee,0x19,0x38,0x41,0x6d,0x14,0x78,0xe4,0x29,0x06,0x9c,0xa0,0x05,0x50,0xeb,
  0x00,0xbd,0xfc,0xa0,0x74,0x86,0xd4,0x81,0xda,0xc0,0x3b,0xc1,0x11,0xd1,0xa6,0x48,0x42,0x14,0x02,0x34,0xf6,0xa4,0x1b,0x35,
  0xed,0xf2,0xfa,0x69,0x5d,0x7f,0xe8,0xbc,0x42,0xea,0x24,0x50,0x78,0x02,0x90,0x61,0x05,0x30,0xa8,0x3e,0x4e,0x3e,0xad,0xf5,
  0x4f,0x7d,0x50,0xd7,0x1d,0x29,0x91,0x57,0x63,0x52,0x09,0x24,0x34,0x21,0xe0,0xf3,0xb9,0x73,0x56,0xda,0x88,0xe8,0x99,0xac,
  0xeb,0x0d,0x44,0x77,0x8c,0xf9,0x09,0x82,0x35,0x5f,0x2f,0xf6,0x51,0x02,0x90,0xa8,0x9e,0xc1,0xb1,0x90,0x18,0x09,0xd4,0x15,
  0x5a,0x13,0x2f,0x00,0xde,0x41,0x46,0x87,0x7e,0x95,0x95,0xdb,0xc8,0x4d,0xfb,0x71,0xe5,0x1b,0xb4,0x37,0x91,0xd3,0x7c,0xe4,
  0x05,0xc0,0x16,0xce,0x39,0xaa,0xfa,0x79,0x04,0xaa,0xaf,0xe0,0xd4,0x87,0xab,0xf6,0x7e,0x8f,0xcf,0xa7,0x42,0x2d,0x29,0xd6,
  0x43,0x06,0x00,0x1e,0xe2,0x52,0x40,0xc5,0x77,0x1b,0xb5,0x0c,0xeb,0x43,0x89,0x48,0x85,0x47,0x14,0x69,0x00,0x0f,0xa0,0x26,
  0x06,0xea,0xe6,0x48,0x61,0xda,0x32,0x5d,0x41,0x40,0xa7,0xb8,0x60,0x9f,0x79,0x00,0x50,0x1d,0x74,0x84,0x6a,0x14,0x80,0x79,
  0xcb,0x78,0x05,0x01,0xa4,0x04,0x8e,0x3c,0x00,0xd8,0x46,0x05,0x87,0x0a,0x01,0x58,0xb4,0xf0,0x95,0x13,0x98,0x93,0x80,0x76,
  0x4f,0x98,0x06,0x00,0x0a,0x94,0x6f,0x94,0xcf,0xdb,0xe7,0xe6,0xf6,0x57,0xdd,0x05,0x9b,0xa2,0xc2,0xe0,0x89,0x07,0x00,0xca,
  0x3e,0x02,0x6c,0xef,0xdb,0x6c,0x01,0x94,0x67,0x8d,0x39,0x09,0x1c,0xb9,0xd7,0x82,0x68,0x00,0x42,0x61,0x7c,0xa3,0xf4,0x7b,
  0x65,0x06,0x56,0x43,0x3f,0x6b,0x92,0x40,0x81,0x2a,0x99,0x88,0x00,0x64,0x88,0xff,0x28,0x15,0xc0,0x86,0x75,0xcf,0x74,0x69,
  0x63,0x98,0xde,0xc3,0xc8,0xf2,0x11,0x39,0x80,0x13,0x9b,0x35,0x22,0x40,0x55,0xcb,0x18,0xb2,0x26,0xbc,0x5b,0xf2,0xc7,0x7b,
  0x98,0x9a,0x17,0x2d,0x3a,0xf4,0x8e,0xd0,0x16,0x26,0x36,0x94,0xdd,0x0a,0xfe,0x6c,0xbf,0x72,0x65,0xf4,0xea,0xd6,0x61,0x51,
  0x7d,0xb5,0xfb,0x65,0x67,0x3c,0xb7,0xe1,0x53,0x98,0x27,0x6d,0x5b,0x00,0x12,0x01,0xa0,0x74,0x19,0xb8,0x5f,0x91,0xa7,0x86,
  0x39,0xbc,0xee,0x18,0xef,0x41,0xaf,0x02,0x82,0x2e,0xdb,0xfa,0xc2,0xe4,0x38,0x8d,0xdd,0x16,0x43,0xbb,0xc0,0x43,0xc4,0x31,
  0xbf,0x47,0x4f,0xc5,0x48,0xca,0xb9,0x1b,0xc9,0xf9,0xa4,0x37,0xea,0xa1,0xc3,0x39,0x22,0xc2,0xa1,0x8b,0x61,0x6c,0x7f,0x80,
  0xa8,0x6c,0x0c,0xb8,0xf2,0xfc,0xec,0x98,0x7f,0x94,0x54,0x00,0xdf,0x94,0x9c,0x6b,0x54,0xd6,0x2f,0x3e,0x83,0xed,0x4b,0x25,
  0x7f,0x9e,0x5c,0x2b,0x3d,0xf6,0xf5,0xca,0xb3,0x7a,0x66,0x2f,0x13,0x2b,0xa0,0x5a,0x6c,0xbd,0xba,0x89,0x29,0x71,0x80,0xbe,
  0xac,0xe8,0x89,0x3f,0x16,0xc8,0xee,0xc8,0xb3,0x8b,0x26,0xb8,0xfb,0x82,0x05,0xa0,0x11,0xe1,0x41,0x62,0x3e,0x56,0xdb,0x32,
  0x8b,0xec,0x8e,0x7c,0x5d,0x0c,0xd5,0x6d,0x3e,0x1f,0x11,0x03,0xa8,0xeb,0xf3,0xde,0xac,0x0c,0x81,0xcb,0x7b,0xfd,0x49,0x7d,
  0x70,0x2a,0x6b,0x12,0xce,0x2a,0x77,0x7c,0xd2,0x3a,0x9f,0x25,0x06,0x50,0x17,0x54,0x5e,0xdd,0xb8,0x1b,0x20,0x8a,0xa7,0x7e,
  0x63,0xc3,0xfc,0x72,0x93,0xb0,0xae,0xb2,0xae,0xa7,0x6c,0xea,0x16,0x4b,0x00,0xb5,0x85,0x85,0xda,0x2b,0xaf,0x95,0x97,0x6a,
  0x07,0x4c,0xcb,0xb0,0xea,0x37,0xaf,0x42,0x74,0xda,0x38,0xc7,0xd8,0xd5,0x00,0xcd,0xcb,0xd5,0xef,0xc9,0xd2,0xeb,0xc9,0x2c,
  0xec,0x2f,0x79,0xf2,0x47,0x4f,0x4a,0x17,0x04,0x7b,0x24,0x97,0x8e,0x04,0xd0,0xd4,0x21,0x9b,0x94,0x79,0x80,0xb6,0xb2,0xbf,
  0x84,0xc0,0xf2,0x6c,0x6a,0xd9,0xd8,0x7f,0x40,0x5b,0x08,0x35,0x5d,0xba,0x96,0x25,0x1e,0xb0,0xf0,0x97,0x11,0x3a,0x67,0xe9,
  0xc5,0x64,0xb0,0x1c,0xd2,0x65,0x63,0x06,0x0c,0x0b,0x00,0x44,0xda,0xf4,0x19,0x91,0xe3,0x75,0xa9,0x45,0x93,0x04,0x26,0x8d,
  0x0d,0x48,0x47,0x21,0x63,0x40,0x29,0xf0,0xf9,0xbf,0x6c,0x4a,0xa3,0xd3,0x0d,0x9b,0xac,0x51,0xce,0x73,0x66,0x04,0x60,0xcb,
  0x9c,0xc8,0xfc,0x05,0x24,0x86,0x1d,0x23,0xca,0xa4,0x27,0x26,0x00,0x00,0xe5,0x28,0x09,0x61,0xfc,0x5b,0xa1,0x26,0x3d,0x31,
  0x01,0xb2,0x80,0x72,0x9c,0x00,0xf3,0x09,0x5c,0xea,0x89,0x31,0xfd,0x3c,0xf2,0x13,0xde,0x7e,0x7d,0x5e,0x95,0x14,0xcb,0x66,
  0x04,0x87,0x76,0x5b,0x3c,0x86,0x93,0x86,0x03,0x20,0x8c,0xbf,0x7e,0xee,0x7e,0xb9,0x5d,0xcb,0x98,0x4a,0xa9,0xc3,0x72,0x40,
  0x05,0xcc,0xdf,0x2f,0x97,0x76,0x27,0x99,0x5a,0xc4,0x34,0x4f,0x31,0xc0,0xf8,0x99,0xdc,0x2d,0x57,0x07,0x58,0x76,0x02,0xe3,
  0x30,0x74,0x44,0xe9,0x02,0xa6,0x02,0x98,0x73,0x19,0xeb,0xa6,0xe9,0xb9,0x76,0x19,0x2d,0x5b,0x54,0x80,0xa9,0xfe,0xe6,0x8e,
  0xef,0x29,0x9a,0xab,0x3b,0x72,0xb9,0x08,0x47,0x00,0xa6,0x75,0xd0,0xb6,0x83,0x7a,0x96,0x96,0xbd,0xb6,0x99,0x90,0x12,0x80,
  0x8b,0x07,0x38,0x29,0x77,0xea,0x58,0x8d,0x51,0x01,0x30,0xcc,0x82,0x5b,0x44,0x02,0x58,0x90,0x80,0xe1,0x6f,0x8b,0xe1,0x14,
  0x83,0x03,0x60,0x38,0x8b,0x8a,0x4a,0x00,0xd8,0x5b,0xa1,0x0e,0x03,0x75,0x67,0x48,0x28,0x6b,0x00,0xf5,0x02,0x38,0x6d,0x88,
  0xcb,0x6b,0x25,0x70,0x4e,0x7d,0x99,0xb4,0x9e,0x07,0x37,0x00,0x16,0x75,0x28,0x42,0x00,0xbd,0xf1,0x44,0xbf,0x7a,0x16,0x7d,
  0x94,0x23,0x97,0x04,0xf6,0x59,0x34,0x68,0x10,0xdc,0x43,0x08,0x40,0xe6,0x07,0xea,0x2c,0x4e,0x1f,0xa8,0x09,0x22,0x0a,0x08,
  0x49,0x06,0xdf,0x10,0x80,0x69,0x20,0xbb,0x27,0x9b,0x3e,0x29,0xf2,0xf9,0xa6,0x01,0x5d,0x28,0xd1,0x28,0x81,0x3d,0xc3,0xcb,
  0x98,0x90,0x01,0x38,0x31,0x95,0xc0,0xd9,0x59,0x2b,0xba,0x66,0x4b,0xde,0x33,0xa9,0xab,0x5e,0x22,0x7b,0x56,0x01,0x1a,0x3f,
  0x85,0x95,0x41,0x5b,0x2e,0x70,0x76,0xbf,0xb0,0xbc,0x6d,0xba,0xa2,0x65,0xba,0xea,0x25,0xb2,0xaf,0xa4,0xb1,0xe9,0x67,0x39,
  0xe4,0x6b,0x3f,0x40,0x56,0x0b,0xa0,0xb2,0x67,0xb0,0xa2,0x3f,0x54,0x38,0x2c,0xa9,0x89,0x00,0x58,0xec,0x08,0x6e,0x56,0xa6,
  0xcf,0xba,0x9e,0xc9,0x52,0x02,0xa7,0x61,0xd0,0x62,0x73,0xa0,0x68,0x53,0x01,0xc7,0xc3,0xaa,0x10,0x58,0xdb,0x33,0x59,0x4a,
  0x60,0xea,0xb6,0xa2,0x6a,0x07,0x00,0xa8,0xbd,0x72,0x0f,0x68,0xe8,0x99,0x2c,0x8b,0x03,0x5a,0x3a,0x16,0xd4,0xad,0x00,0x80,
  0x7b,0xb2,0x2c,0x04,0xea,0xfd,0x86,0xf0,0x99,0x95,0x06,0x94,0xbd,0x76,0x01,0xd8,0x15,0x82,0x65,0x5b,0xe1,0xcd,0x45,0x75,
  0x59,0xac,0x9f,0xd8,0x3d,0x87,0x8c,0x5a,0x3b,0xf8,0x53,0x40,0x79,0x02,0x68,0x3e,0xa6,0xec,0xed,0x99,0x1a,0x3a,0xe8,0x02,
  0xa5,0xe9,0x04,0x13,0xca,0xf6,0x83,0x5e,0x12,0x0a,0x00,0xd5,0x32,0x14,0xb7,0xbb,0xae,0xa3,0x03,0x60,0xba,0x1a,0x76,0x4b,
  0xcc,0x8d,0xaf,0xc8,0xa0,0x5d,0x0d,0x79,0x0c,0x82,0x96,0x0b,0x74,0xa0,0xbb,0x11,0xf8,0x30,0xae,0x18,0x20,0xd0,0xda,0x2e,
  0xee,0x46,0x16,0x03,0x02,0x46,0xc0,0xe0,0x71,0x30,0x20,0x80,0x0c,0xef,0xa5,0x01,0xe3,0xe0,0x3a,0xb4,0x38,0x46,0x5a,0xf5,
  0x00,0x84,0xcc,0x97,0x76,0xc4,0x9e,0x5d,0x59,0x3d,0x05,0x2c,0xce,0xaa,0x18,0xf5,0x72,0x05,0x00,0xa0,0x73,0xb8,0xba,0xa8,
  0x84,0x17,0x24,0x89,0x80,0x2a,0x0b,0xd0,0xac,0x44,0x16,0x22,0xc0,0x5c,0xcb,0xf4,0x72,0x9b,0x34,0x49,0x22,0x98,0x10,0x01,
  0x38,0xf1,0x20,0x80,0xc5,0x9e,0xc1,0xc5,0xfe,0xd0,0x82,0xe2,0x3b,0xb3,0x88,0x5c,0x60,0x43,0xd5,0xda,0x6f,0xf7,0x66,0xf8,
  0x2e,0xc5,0x80,0x17,0x0b,0xd1,0xaf,0x44,0x22,0xf3,0xdd,0x91,0x6a,0xc5,0x00,0xcc,0x8d,0x5e,0xe9,0x7d,0x10,0x3d,0x77,0x2d,
  0xfa,0xee,0x6a,0x01,0x50,0x88,0xef,0x9c,0x7f,0x4b,0xca,0xfe,0xea,0x2a,0x60,0x80,0xca,0x5b,0x72,0xb5,0x00,0x9c,0x4b,0x73,
  0xfd,0x6a,0xf7,0x3e,0xd7,0x4d,0xd4,0x5b,0xb1,0x18,0xf0,0xa6,0xe7,0xad,0x6e,0x4d,0xf4,0xe6,0x15,0x19,0x02,0x56,0x0c,0x00,
  0x4c,0x24,0x46,0xda,0x67,0x37,0x02,0xf7,0x56,0xaf,0x14,0x4e,0x30,0x53,0xfb,0xfa,0x5e,0x30,0xe5,0xdb,0x18,0xa3,0x59,0x0b,
  0xa4,0x98,0xd8,0x76,0x2a,0x81,0x4d,0x0d,0xab,0x07,0x00,0xa6,0x12,0xe1,0xdb,0xa7,0x8f,0x43,0x5d,0x81,0x55,0x04,0xa0,0x37,
  0x31,0xc9,0xed,0x39,0x58,0xdd,0x07,0xec,0x44,0x1d,0x70,0x8c,0x79,0xd1,0xf6,0xb1,0xf4,0x77,0x1f,0xb0,0xf5,0x42,0x48,0xa5,
  0x88,0xaf,0x13,0x22,0xe4,0x25,0x05,0xae,0x04,0xa7,0x98,0x07,0x47,0xd2,0x15,0x06,0x80,0x0a,0xee,0x6a,0x85,0x01,0x44,0x38,
  0x18,0xc0,0x2a,0x43,0x1a,0xb0,0x02,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x38,0x03,0xd0,0x69,0x47,0x8d,0x4b,0x59,0x01,
  0x0c,0x80,0x01,0x90,0x00,0xd8,0x5d,0x61,0xdb,0x50,0x07,0xa9,0x6e,0x02,0x40,0xb5,0xf7,0xad,0xb2,0x0b,0xa0,0x76,0x1f,0xd6,
  0x5b,0x9e,0x24,0x59,0xff,0xd8,0xa0,0xff,0xb1,0x0e,0x2d,0xca,0x44,0xe4,0x7a,0x0c,0x36,0x3f,0x4a,0x8c,0x1b,0x6b,0x64,0xb6,
  0x4d,0xfc,0x5c,0xa0,0x1c,0x9f,0x5a,0x3e,0x46,0x3d,0xe1,0xe8,0xc9,0x05,0x12,0xb2,0x33,0x99,0x5f,0xdf,0xd9,0xb3,0x63,0xf8,
  0x5f,0x1e,0x58,0xa5,0x3a,0xe0,0xfc,0x9b,0x15,0xf5,0x24,0x6e,0x00,0x5e,0x28,0xcd,0x3d,0x3b,0x58,0xf8,0x20,0x30,0x88,0x5a,
  0x01,0x0b,0x6e,0x55,0xc8,0x0b,0xe6,0x02,0x4b,0x6f,0x1b,0xcd,0x62,0x06,0x40,0xbf,0x1e,0x5e,0x7e,0x76,0xc4,0x43,0xa0,0x4d,
  0x23,0x56,0xc0,0x56,0x34,0xf5,0xb6,0x97,0x57,0x68,0x34,0x8f,0x2c,0x40,0xae,0x4d,0x54,0xc4,0x00,0xca,0xc6,0x33,0x6a,0x37,
  0x8b,0x38,0x08,0x96,0xce,0xf6,0x8b,0x10,0x5f,0x62,0x09,0x60,0x4a,0x3c,0x37,0x2a,0xc4,0xaa,0x3b,0x89,0x57,0x01,0x15,0x73,
  0xb3,0x11,0x6f,0x10,0x14,0x41,0x00,0x10,0xfb,0xc0,0x20,0x5e,0x00,0x2a,0x48,0xac,0x95,0xd1,0xba,0x80,0x30,0x48,0x8e,0x1e,
  0x30,0xdb,0x00,0xa0,0x2d,0x05,0x75,0x18,0xa1,0xa5,0xd1,0x2a,0x40,0x87,0xdd,0x77,0x20,0x01,0x90,0x06,0x71,0x4e,0x52,0x00,
  0x89,0x8a,0x56,0x01,0x59,0xbb,0x15,0xa7,0x15,0x80,0x69,0x10,0x05,0xc0,0x9d,0xf0,0x96,0xb5,0xa1,0x80,0x6a,0x6d,0x7e,0x11,
  0xad,0x02,0x28,0x39,0x09,0xd7,0xcc,0x45,0x59,0x07,0xa1,0x2d,0x4b,0xa1,0x6b,0x63,0x97,0x14,0x80,0xee,0x9c,0xfd,0xc4,0xbf,
  0x30,0xd1,0x46,0x7c,0x0e,0x92,0x05,0x57,0x17,0x00,0xf5,0x0f,0x2d,0x4d,0x2f,0x3a,0x80,0xb5,0xae,0x01,0x48,0x89,0x01,0x10,
  0x46,0x41,0x1d,0x24,0xd7,0x68,0x6a,0x00,0x5d,0xcb,0x83,0x92,0x18,0x00,0x65,0x14,0x94,0x21,0x0a,0xa1,0x4e,0x02,0x20,0xdc,
  0x0f,0xc0,0x66,0x41,0x3c,0x00,0xc2,0x62,0x58,0x05,0x00,0xb0,0x46,0x6e,0x17,0xe1,0xfd,0xeb,0x10,0xcf,0xc5,0x69,0x72,0x00,
  0x22,0x00,0x80,0x6d,0xba,0xef,0x18,0x90,0x03,0x20,0x4c,0x03,0x95,0xbf,0x16,0x42,0x18,0x67,0x52,0x7a,0xd7,0xde,0xf5,0x2e,
  0x5b,0xb2,0xf7,0x16,0x9a,0x98,0x85,0x3f,0x92,0x30,0x45,0x25,0xfe,0xbd,0x4c,0xd1,0x03,0x20,0x5c,0x0d,0x68,0xef,0x21,0x40,
  0x78,0x50,0x00,0xe5,0x6a,0x20,0xf5,0x9d,0x1c,0x52,0x0f,0x00,0x28,0x8b,0xe1,0xa9,0x6f,0x00,0x37,0x7d,0x00,0x20,0x8c,0xd1,
  0xa5,0xe7,0x1a,0x10,0x02,0x50,0x1e,0x00,0x90,0xee,0x89,0x08,0x87,0xe2,0x95,0xb2,0x10,0x36,0x02,0xe0,0xf9,0xde,0x00,0xa5,
  0x00,0x92,0xb6,0x0f,0xb5,0x90,0x00,0xa5,0x00,0x4c,0x60,0x9a,0x58,0x45,0xba,0x25,0xb0,0xb7,0x00,0x84,0x52,0x00,0x26,0x45,
  0x9b,0x01,0x00,0x4d,0x5a,0x0b,0xde,0x5b,0x70,0x09,0x4a,0x01,0x98,0xa8,0xc9,0x44,0x01,0xb4,0x1b,0xa3,0x57,0xcf,0xff,0x83,
  0xf6,0x75,0x92,0x6b,0x9e,0x00,0xd0,0x76,0x74,0x1f,0x9f,0x33,0x9a,0xf8,0x99,0x11,0x93,0xb3,0x99,0x00,0xa0,0x6e,0x95,0x7a,
  0xfd,0x0e,0x4d,0x31,0x22,0xbe,0xf1,0x34,0xf0,0x04,0x80,0x7a,0x63,0x54,0x8b,0x91,0x04,0x80,0x91,0xa4,0x7e,0x6c,0xca,0xe4,
  0x3a,0x8d,0x9e,0x1a,0x3b,0xff,0xcb,0x6f,0x24,0x23,0x17,0x3d,0x0f,0x4f,0x8d,0x19,0x65,0x54,0xa3,0xe4,0xde,0x91,0xdb,0x43,
  0x49,0x1c,0x07,0xb7,0x37,0x06,0xde,0x6c,0xea,0xc8,0xdd,0x91,0xd4,0xdf,0xa4,0x76,0x02,0x80,0x59,0x51,0x6d,0x06,0x60,0xba,
  0x72,0x21,0xc0,0xeb,0xd1,0x5d,0x08,0x01,0x86,0x26,0x75,0x22,0x08,0xa4,0x3e,0xe7,0x34,0x5d,0xb5,0x10,0x60,0x0a,0x60,0xba,
  0x6a,0x21,0xc0,0xf3,0xe1,0xf1,0x87,0x00,0x53,0x8b,0x3a,0x10,0x04,0x52,0xbf,0x53,0xda,0x8f,0xdd,0xfe,0x35,0xe5,0x17,0x40,
  0xf4,0x3e,0x30,0xf4,0x6d,0x50,0xec,0x3e,0x70,0xd3,0x33,0x00,0xda,0x8d,0xc1,0xd6,0x93,0xa0,0x85,0x02,0xfe,0x75,0xd1,0x01,
  0x44,0x1e,0x04,0x06,0xde,0xed,0x89,0xdb,0x07,0x12,0xe9,0x7f,0x42,0xa3,0x4e,0x84,0xe6,0xf7,0xf0,0xcd,0x01,0x44,0x5d,0x0d,
  0xbf,0x1f,0x00,0x40,0xd4,0xc5,0x60,0x1a,0x00,0x40,0xcc,0x3e,0x60,0x5a,0x06,0xda,0x01,0x88,0xb8,0x73,0x7e,0x08,0x21,0x00,
  0x14,0xf1,0xfa,0xc0,0x20,0x08,0x80,0x78,0x7d,0xc0,0xc2,0x03,0xac,0x00,0xac,0xad,0x90,0x07,0x58,0x01,0x88,0xd6,0x07,0x06,
  0x81,0x00,0xc4,0xea,0x03,0x36,0x1e,0x60,0x07,0x20,0xd2,0xf5,0xc0,0x6e,0x30,0x5b,0xe2,0x5c,0x0f,0x24,0x01,0x3f,0xb5,0x76,
  0xd1,0x01,0x44,0xb9,0x2f,0x74,0xf3,0x82,0x03,0x58,0x0b,0xa9,0x1b,0xf8,0x69,0x15,0xd6,0x41,0x4e,0x01,0x7d,0xf7,0xa2,0x03,
  0x88,0x2e,0x13,0x5a,0x15,0x01,0x2e,0x00,0x62,0x8b,0x02,0x77,0x02,0xcf,0x64,0x6c,0xa5,0x80,0x75,0xaf,0xb5,0xb5,0x94,0x9f,
  0xc4,0x05,0x60,0x14,0x1c,0x40,0x11,0x97,0x04,0xac,0x9b,0xcd,0xed,0x83,0x59,0x54,0x61,0xd0,0x36,0x04,0x3a,0x01,0x88,0x29,
  0x0c,0xde,0x69,0x61,0x1e,0xf5,0x28,0x1e,0xfb,0x2f,0x65,0x6d,0x08,0x39,0xa2,0x1b,0x04,0x1f,0x42,0x1b,0x00,0xe2,0xc9,0x84,
  0x49,0x4b,0x9f,0xbd,0x14,0x4d,0x0e,0x6c,0x09,0x40,0x71,0x27,0x12,0x01,0x0c,0x5a,0x02,0x10,0x4b,0x31,0x64,0x5d,0x04,0x39,
  0x03,0x88,0x43,0x02,0x4e,0x02,0x70,0x2c,0x67,0x9e,0x74,0x5e,0x00,0x8e,0x00,0x62,0x90,0x80,0x9b,0x00,0x5c,0x0b,0xda,0x27,
  0x5d,0x17,0x80,0x2b,0x80,0xf6,0x25,0xe0,0x28,0x00,0xe7,0x25,0xcd,0x93,0x8e,0x0b,0xc0,0x19,0x40,0xdb,0x12,0x70,0x15,0x80,
  0xfb,0xa2,0xf6,0x49,0xb7,0x05,0xe0,0x0e,0xa0,0x5d,0x09,0x38,0x0b,0x80,0x60,0x5b,0xe3,0x49,0xa7,0x05,0x40,0x00,0xa0,0x4d,
  0x09,0xb8,0x0b,0x00,0xd6,0xdc,0xb7,0x35,0x7e,0xda,0x50,0x6d,0x01,0xb8,0xd1,0xee,0x52,0xfa,0xf5,0xbe,0x40,0x6b,0x5b,0x43,
  0x97,0xb6,0xa3,0x00,0x00,0xd3,0xb4,0x25,0x00,0xbf,0x83,0x38,0x00,0xb4,0x25,0x81,0xb5,0x2c,0x12,0x00,0x30,0x6d,0x25,0x0e,
  0x26,0xbf,0x81,0x58,0x00,0xe8,0x7f,0x77,0x33,0x05,0x92,0x01,0x00,0xf8,0xa4,0x85,0x08,0x48,0xf3,0xe6,0x21,0x22,0x00,0x2d,
  0xc4,0xc1,0x0f,0x55,0x4c,0x00,0xf4,0x2f,0x43,0xdb,0xbf,0x43,0x15,0x49,0x88,0xce,0x33,0x0e,0xec,0x04,0xc9,0x20,0x32,0x00,
  0xa1,0x9d,0xe0,0x96,0x8a,0x0d,0x40,0x58,0x27,0xd8,0xa1,0x93,0x12,0xd9,0x99,0x42,0x3a,0x41,0x32,0x88,0x10,0x40,0x48,0x27,
  0x20,0x73,0x00,0x52,0x00,0xfa,0x46,0xf7,0x1c,0x80,0xb6,0xcf,0x43,0x05,0x72,0x82,0xb5,0x41,0xa4,0x00,0x60,0xba,0x1b,0x66,
  0x0d,0xa0,0x62,0x05,0xa0,0xd7,0x43,0x84,0x81,0xdb,0x0a,0x62,0x05,0x00,0x7a,0xc7,0x3f,0x81,0x1d,0xda,0x97,0x0f,0x12,0xf7,
  0x7a,0xe5,0xde,0xb7,0x06,0x76,0xb6,0x21,0x66,0x00,0x30,0xf1,0x1c,0x08,0x93,0x01,0xc4,0x0d,0xc0,0x73,0x20,0x4c,0x3e,0x56,
  0xb1,0x03,0xf0,0x1a,0x08,0x93,0x4f,0x14,0xc4,0x0e,0x00,0xf4,0xbb,0xfe,0x08,0xdc,0xce,0x21,0x7e,0x00,0x1e,0x53,0xc1,0x8e,
  0x87,0x9f,0x3d,0xf4,0xd1,0xf1,0x9b,0x7b,0x22,0x40,0x9d,0x00,0xbc,0x01,0xf0,0x44,0xc0,0x8b,0xfd,0x9e,0x7a,0xbe,0x7d,0x10,
  0xf0,0x63,0xbf,0xaf,0xa6,0x77,0x7a,0x02,0x9e,0xec,0xf7,0xd6,0xf5,0x4f,0x4d,0xc0,0x97,0xfd,0xfe,0x1e,0x7b,0xa0,0x25,0xe0,
  0xcd,0x7e,0x8a,0xdb,0xe3,0x15,0x63,0xf6,0x5e,0x41,0x55,0xb5,0x24,0xbf,0xf5,0xf7,0xb3,0xbf,0x1e,0x1f,0x7c,0xc9,0xdf,0xbd,
  0x4d,0x74,0x8d,0x1e,0xea,0x9f,0x10,0x00,0x40,0x27,0x24,0x2b,0xa3,0xe4,0x63,0x9f,0x3f,0xfb,0xec,0xf5,0xd1,0x27,0x3d,0x25,
  0x20,0xb0,0x43,0xbe,0xfe,0x09,0x14,0x03,0x00,0x00,0xe0,0xc5,0xaf,0xb4,0xdb,0xe5,0x27,0x1f,0x5d,0xf5,0x6a,0xbf,0xf7,0x87,
  0xdf,0xd4,0xbb,0x4e,0x22,0x48,0x3e,0xd1,0x7e,0xed,0xf7,0xad,0x00,0x00,0x38,0xfe,0xc0,0xde,0x86,0x1b,0xb7,0x94,0xef,0xcb,
  0x5b,0xf7,0xfd,0x05,0xa0,0xd5,0x4e,0x72,0x60,0xf5,0xc9,0x51,0x5f,0x7b,0xb7,0x3f,0x80,0x02,0x00,0x66,0x6f,0xd9,0x44,0x82,
  0xe4,0xa3,0x6b,0x33,0xff,0xd7,0x16,0x04,0x00,0xc0,0xec,0x6d,0x63,0x3f,0xb8,0x71,0xfb,0x79,0x08,0xfb,0x03,0x01,0x00,0x98,
  0xbd,0x27,0x37,0x0d,0x10,0x8c,0x3e,0x82,0x59,0x98,0x0b,0x0b,0x05,0x00,0x66,0x02,0x8d,0x20,0xb9,0xf5,0xeb,0x50,0xe6,0x07,
  0x04,0x00,0x30,0x13,0x6f,0x7f,0x20,0x9a,0x8b,0xda,0x1b,0x3f,0xbb,0x35,0x0b,0x66,0x3e,0x80,0xf8,0x14,0x82,0x8e,0xde,0x18,
  0x1e,0xd4,0x4d,0x7e,0x3a,0x0a,0x10,0xf9,0xdb,0x04,0x00,0x20,0x64,0x91,0x15,0x59,0x99,0xf1,0xbb,0x93,0x61,0x60,0xeb,0x5b,
  0x01,0x70,0x0a,0x41,0x1f,0x82,0x38,0x39,0xc3,0x30,0x02,0x48,0x52,0xc8,0x5b,0xb9,0x94,0x56,0x00,0xbc,0x72,0x87,0x37,0x2b,
  0xe7,0xf6,0x2e,0x62,0xbd,0xbd,0xaf,0x6e,0xd3,0xec,0x70,0x8b,0xa1,0xe8,0x07,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,
  0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,
  0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,
  0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,
  0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,0x60,0x00,0x0c,0x80,0x01,0x30,0x00,0x06,0xc0,0x00,0x18,0x00,0x03,
  0x60,0x00,0x0c,0xa0,0xdb,0xe3,0xff,0x62,0xcf,0xbd,0xe7,0x96,0x9f,0x26,0x1f,0x00,0x00,0x00,0x00,0x49,0x45,0x4e,0x44,0xae,
  0x42,0x60,0x82,
};
const size_t ICON512_LEN = 4779;
