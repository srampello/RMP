/*
 * RMP Robotics - Banco de pruebas universal para ESP32 Super Mini
 *
 * Compatible con ESP32-C3 Super Mini y ESP32-S3 SuperMini.
 * No requiere librerias externas: utiliza las incluidas en ESP32 Arduino Core.
 *
 * Funciones:
 * - Configuracion web y persistente de pines.
 * - MUX analogico CD74HC4067 (4 a 16 canales).
 * - LED y boton.
 * - Dos motores con drivers IN/IN, PWM/DIR o IN/IN/PWM.
 * - Panel web RMP con tres pestanas.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <soc/soc_caps.h>

#if __has_include(<esp_arduino_version.h>)
#include <esp_arduino_version.h>
#endif

#ifndef ESP_ARDUINO_VERSION_MAJOR
#define ESP_ARDUINO_VERSION_MAJOR 2
#endif

// ---------------------------------------------------------------------------
// Red del banco de pruebas
// ---------------------------------------------------------------------------

static const char *AP_SSID = "RMP_TEST";
static const char *AP_PASSWORD = "RMP2026";
static const uint16_t PWM_FREQUENCY = 20000;
static const uint8_t PWM_RESOLUTION = 8;
static const uint32_t MOTOR_TIMEOUT_MS = 1500;

WebServer server(80);
Preferences preferences;

enum MotorMode : uint8_t {
  MOTOR_IN_IN = 0,
  MOTOR_PWM_DIR = 1,
  MOTOR_IN_IN_PWM = 2
};

enum ButtonPull : uint8_t {
  BUTTON_FLOATING = 0,
  BUTTON_PULLUP = 1,
  BUTTON_PULLDOWN = 2
};

struct PinConfig {
  int8_t muxS0;
  int8_t muxS1;
  int8_t muxS2;
  int8_t muxS3;
  int8_t muxSig;
  int8_t muxEnable;

  int8_t led;
  int8_t button;

  int8_t motor1A;
  int8_t motor1B;
  int8_t motor1Pwm;
  int8_t motor2A;
  int8_t motor2B;
  int8_t motor2Pwm;
  int8_t motorEnable;

  uint8_t motorMode;
  uint8_t buttonPull;
  bool ledActiveHigh;
  bool buttonActiveLow;
  bool motorEnableActiveHigh;
  bool motor1Invert;
  bool motor2Invert;

  uint8_t muxChannels;
  uint8_t adcSamples;
  uint16_t scanIntervalMs;
};

PinConfig config;

uint16_t muxValues[16] = {0};
bool ledState = false;
bool buttonPressed = false;
bool lastButtonRaw = false;
uint32_t buttonChangedAt = 0;
uint32_t buttonPressCount = 0;

int16_t motor1Command = 0;
int16_t motor2Command = 0;
uint32_t lastMotorCommandAt = 0;
uint32_t lastMuxScanAt = 0;
uint32_t restartAt = 0;
bool hardwareReady = false;

int8_t pwmPins[6] = {-1, -1, -1, -1, -1, -1};
uint8_t pwmPinCount = 0;

// ---------------------------------------------------------------------------
// Interfaz web
// ---------------------------------------------------------------------------

static const char INDEX_HTML[] PROGMEM = R"RMPHTML(
<!doctype html>
<html lang="es">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
  <title>RMP | Banco de pruebas ESP32</title>
  <style>
    :root{
      --bg:#07060a;--panel:#111016;--panel2:#18151f;--line:#2b2635;
      --violet:#7c3aed;--violet2:#a855f7;--white:#f8f7fb;--muted:#a8a2b3;
      --ok:#35d07f;--danger:#ff4d6d;--warning:#f7b955;
    }
    *{box-sizing:border-box}
    body{margin:0;background:radial-gradient(circle at 85% 0,#24113d 0,transparent 32%),var(--bg);
      color:var(--white);font-family:Inter,Segoe UI,Arial,sans-serif;min-height:100vh}
    header{display:flex;align-items:center;gap:16px;padding:18px clamp(16px,4vw,40px);
      border-bottom:1px solid var(--line);background:rgba(7,6,10,.9);position:sticky;top:0;z-index:10;
      backdrop-filter:blur(14px)}
    .logo{width:78px;height:48px;filter:drop-shadow(0 0 14px rgba(168,85,247,.35))}
    .brand h1{font-size:clamp(18px,3vw,25px);margin:0;letter-spacing:.08em}
    .brand p{margin:4px 0 0;color:var(--muted);font-size:12px;letter-spacing:.12em}
    .online{margin-left:auto;display:flex;align-items:center;gap:8px;color:var(--muted);font-size:13px}
    .dot{width:9px;height:9px;border-radius:50%;background:var(--ok);box-shadow:0 0 12px var(--ok)}
    nav{display:flex;gap:8px;overflow:auto;padding:14px clamp(16px,4vw,40px) 0}
    .tab{border:1px solid var(--line);background:var(--panel);color:var(--muted);
      padding:11px 16px;border-radius:12px 12px 0 0;font-weight:700;cursor:pointer;white-space:nowrap}
    .tab.active{color:white;background:linear-gradient(135deg,#4c1d95,var(--violet));border-color:var(--violet2)}
    main{padding:22px clamp(16px,4vw,40px) 40px;max-width:1300px;margin:auto}
    .page{display:none}.page.active{display:block}
    .hero{display:flex;justify-content:space-between;align-items:flex-end;gap:20px;margin-bottom:20px}
    h2{margin:0;font-size:clamp(23px,4vw,34px)} h3{margin:0 0 14px;font-size:17px}
    .sub{color:var(--muted);margin:7px 0 0;line-height:1.5}
    .grid{display:grid;grid-template-columns:repeat(12,1fr);gap:16px}
    .card{grid-column:span 6;background:linear-gradient(150deg,var(--panel2),var(--panel));
      border:1px solid var(--line);border-radius:18px;padding:18px;box-shadow:0 15px 35px rgba(0,0,0,.2)}
    .wide{grid-column:span 12}.third{grid-column:span 4}
    .fields{display:grid;grid-template-columns:repeat(3,1fr);gap:12px}
    label{display:flex;flex-direction:column;gap:6px;color:var(--muted);font-size:12px;font-weight:700}
    input,select{width:100%;border:1px solid #373043;background:#0c0a10;color:white;border-radius:10px;
      padding:11px 12px;font-size:15px;outline:none}
    input:focus,select:focus{border-color:var(--violet2);box-shadow:0 0 0 3px rgba(168,85,247,.14)}
    .check{display:flex;flex-direction:row;align-items:center;gap:9px;padding-top:22px}
    .check input{width:18px;height:18px;accent-color:var(--violet)}
    .actions{display:flex;flex-wrap:wrap;gap:10px;margin-top:16px}
    button{border:1px solid #3b3347;background:#17131d;color:white;border-radius:11px;padding:11px 16px;
      font-weight:800;cursor:pointer;touch-action:none}
    button:hover{border-color:var(--violet2)}
    .primary{background:linear-gradient(135deg,#5b21b6,var(--violet));border-color:var(--violet2)}
    .danger{background:#351019;border-color:#7d2638;color:#ff9eb0}
    .success{background:#0d3020;border-color:#1f7048;color:#72e6aa}
    .status{display:flex;align-items:center;justify-content:space-between;padding:14px;border-radius:12px;
      background:#0c0a10;border:1px solid var(--line);margin-top:10px}
    .value{font-size:25px;font-weight:900;color:white}
    .pill{padding:6px 10px;border-radius:999px;background:#282230;color:var(--muted);font-size:12px;font-weight:800}
    .pill.on{background:#153c29;color:#75e6ad}.pill.off{background:#351019;color:#ff9eb0}
    .motor{padding:15px;border-radius:14px;background:#0c0a10;border:1px solid var(--line)}
    .motor-head{display:flex;justify-content:space-between;align-items:center;margin-bottom:12px}
    .slider-row{display:grid;grid-template-columns:1fr 55px;gap:10px;align-items:center}
    input[type=range]{padding:0;accent-color:var(--violet);border:0}
    .drive-grid{display:grid;grid-template-columns:repeat(3,1fr);gap:9px;margin-top:12px}
    .drive-grid button{min-height:48px}
    .movement{display:grid;grid-template-columns:repeat(3,1fr);grid-template-areas:". f ." "l s r" ". b .";gap:10px;max-width:420px;margin:auto}
    .movement button{min-height:58px}.mf{grid-area:f}.ml{grid-area:l}.ms{grid-area:s}.mr{grid-area:r}.mb{grid-area:b}
    .dashboard-metrics{display:grid;grid-template-columns:repeat(6,1fr);gap:10px;margin:16px 0}
    .metric{background:linear-gradient(150deg,var(--panel2),var(--panel));border:1px solid var(--line);
      border-radius:13px;padding:13px 14px;min-width:0}
    .metric small{display:block;color:var(--muted);font-size:10px;font-weight:800;letter-spacing:.08em;text-transform:uppercase}
    .metric strong{display:block;font-size:21px;margin-top:7px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
    .metric em{color:var(--violet2);font-size:11px;font-style:normal;font-weight:800}
    .chart-card{margin-top:14px}
    .chart-head{display:flex;justify-content:space-between;align-items:center;gap:12px;margin-bottom:12px}
    .chart-head h3{margin:0}.chart-tools{display:flex;align-items:end;gap:9px;flex-wrap:wrap}
    .chart-tools label{min-width:105px}.chart-tools select{padding:8px 10px;font-size:13px}
    .chart-wrap{height:300px;border:1px solid var(--line);border-radius:13px;background:#0a0810;padding:8px;overflow:hidden}
    .chart-wrap.history{height:260px}
    canvas{display:block;width:100%;height:100%}
    .legend{display:flex;gap:15px;color:var(--muted);font-size:12px;font-weight:700}
    .legend span{display:flex;align-items:center;gap:7px}
    .legend i{width:18px;height:3px;border-radius:9px;background:var(--violet2)}
    .legend .line-b{background:var(--white)}
    .notice{padding:13px 15px;border-radius:12px;background:#21172d;border:1px solid #4d2d70;color:#d9c2ef;line-height:1.5;font-size:13px}
    .error{background:#351019;border-color:#7d2638;color:#ffb4c2}
    footer{text-align:center;color:#777080;font-size:12px;padding:20px}
    @media(max-width:920px){.dashboard-metrics{grid-template-columns:repeat(3,1fr)}}
    @media(max-width:820px){.card,.third{grid-column:span 12}.fields{grid-template-columns:repeat(2,1fr)}.chart-head{align-items:flex-start;flex-direction:column}.chart-wrap{height:255px}}
    @media(max-width:480px){.fields{grid-template-columns:1fr}.dashboard-metrics{grid-template-columns:repeat(2,1fr)}.online{display:none}.logo{width:62px}.drive-grid{grid-template-columns:1fr}.chart-wrap{height:230px}.chart-tools{width:100%}.chart-tools label{flex:1}}
  </style>
</head>
<body>
<header>
  <svg class="logo" viewBox="0 0 180 100" role="img" aria-label="RMP">
    <defs><linearGradient id="g" x1="0" x2="1"><stop stop-color="#fff"/><stop offset="1" stop-color="#d8b4fe"/></linearGradient></defs>
    <text x="3" y="72" fill="url(#g)" font-family="Arial Black,Impact,sans-serif" font-size="72" font-weight="900" letter-spacing="-8">RMP</text>
  </svg>
  <div class="brand"><h1>RMP ROBOTICS</h1><p>BANCO DE PRUEBAS UNIVERSAL</p></div>
  <div class="online"><i class="dot"></i><span id="connection">ESP32 conectado</span></div>
</header>

<nav>
  <button class="tab active" data-page="config">1. Configuración</button>
  <button class="tab" data-page="actuators">2. Actuadores</button>
  <button class="tab" data-page="sensors">3. Sensores MUX</button>
</nav>

<main>
  <section id="config" class="page active">
    <div class="hero"><div><h2>Configuración de pines</h2><p class="sub">Adaptá el mismo firmware a cada robot. Los cambios quedan guardados en la memoria del ESP32.</p></div></div>
    <div id="configMessage" class="notice">Usá <b>-1</b> solamente en conexiones opcionales. Al guardar, el ESP32 se reiniciará.</div>
    <div class="grid" style="margin-top:16px">
      <article class="card">
        <h3>MUX CD74HC4067</h3>
        <div class="fields">
          <label>S0<input id="muxS0" type="number" min="-1" max="48"></label>
          <label>S1<input id="muxS1" type="number" min="-1" max="48"></label>
          <label>S2<input id="muxS2" type="number" min="-1" max="48"></label>
          <label>S3<input id="muxS3" type="number" min="-1" max="48"></label>
          <label>SIG / ADC<input id="muxSig" type="number" min="-1" max="48"></label>
          <label>EN opcional<input id="muxEnable" type="number" min="-1" max="48"></label>
          <label>Canales
            <select id="muxChannels"><option>4</option><option>8</option><option>12</option><option selected>16</option></select>
          </label>
          <label>Muestras por canal<input id="adcSamples" type="number" min="1" max="32"></label>
          <label>Intervalo (ms)<input id="scanIntervalMs" type="number" min="20" max="1000"></label>
        </div>
      </article>

      <article class="card">
        <h3>LED y botón</h3>
        <div class="fields">
          <label>Pin LED<input id="led" type="number" min="-1" max="48"></label>
          <label>Pin botón<input id="button" type="number" min="-1" max="48"></label>
          <label>Resistencia botón
            <select id="buttonPull"><option value="0">Sin pull interno</option><option value="1">INPUT_PULLUP</option><option value="2">INPUT_PULLDOWN</option></select>
          </label>
          <label class="check"><input id="ledActiveHigh" type="checkbox"> LED activo en HIGH</label>
          <label class="check"><input id="buttonActiveLow" type="checkbox"> Botón activo en LOW</label>
        </div>
      </article>

      <article class="card wide">
        <h3>Motores</h3>
        <div class="fields">
          <label>Tipo de control
            <select id="motorMode">
              <option value="0">IN1 + IN2 (DRV8833 / RZ7889)</option>
              <option value="1">PWM + DIR (IFX9201 / BTN9960)</option>
              <option value="2">IN1 + IN2 + PWM (TB6612FNG)</option>
            </select>
          </label>
          <label>ENABLE / STBY opcional<input id="motorEnable" type="number" min="-1" max="48"></label>
          <label class="check"><input id="motorEnableActiveHigh" type="checkbox"> ENABLE activo en HIGH</label>
        </div>
        <div class="grid" style="margin-top:14px">
          <div class="motor" style="grid-column:span 6">
            <h3>Motor izquierdo</h3>
            <div class="fields">
              <label>Pin A / PWM / IN1<input id="motor1A" type="number" min="-1" max="48"></label>
              <label>Pin B / DIR / IN2<input id="motor1B" type="number" min="-1" max="48"></label>
              <label>Pin PWM (modo TB6612)<input id="motor1Pwm" type="number" min="-1" max="48"></label>
              <label class="check"><input id="motor1Invert" type="checkbox"> Invertir sentido</label>
            </div>
          </div>
          <div class="motor" style="grid-column:span 6">
            <h3>Motor derecho</h3>
            <div class="fields">
              <label>Pin A / PWM / IN1<input id="motor2A" type="number" min="-1" max="48"></label>
              <label>Pin B / DIR / IN2<input id="motor2B" type="number" min="-1" max="48"></label>
              <label>Pin PWM (modo TB6612)<input id="motor2Pwm" type="number" min="-1" max="48"></label>
              <label class="check"><input id="motor2Invert" type="checkbox"> Invertir sentido</label>
            </div>
          </div>
        </div>
        <div class="actions"><button class="primary" onclick="saveConfig()">Guardar configuración</button></div>
      </article>
    </div>
  </section>

  <section id="actuators" class="page">
    <div class="hero"><div><h2>Prueba de actuadores</h2><p class="sub">Probá LED, botón y motores sin modificar el programa.</p></div></div>
    <div class="grid">
      <article class="card third">
        <h3>LED</h3>
        <div class="status"><span>Salida</span><span id="ledStatus" class="pill off">APAGADO</span></div>
        <div class="actions"><button class="success" onclick="setLed(1)">Encender</button><button onclick="setLed(0)">Apagar</button></div>
      </article>
      <article class="card third">
        <h3>Botón físico</h3>
        <div class="status"><span>Estado</span><span id="buttonStatus" class="pill off">LIBRE</span></div>
        <div class="status"><span>Pulsaciones</span><strong id="buttonCount" class="value">0</strong></div>
      </article>
      <article class="card third">
        <h3>Estado de motores</h3>
        <div class="status"><span>Izquierdo</span><strong id="motor1State">0</strong></div>
        <div class="status"><span>Derecho</span><strong id="motor2State">0</strong></div>
        <div class="actions"><button class="danger" onclick="stopAll()">PARADA</button></div>
      </article>

      <article class="card">
        <div class="motor-head"><h3>Motor izquierdo</h3><span id="leftPwmLabel" class="pill">PWM 120</span></div>
        <div class="slider-row"><input id="leftPwm" type="range" min="0" max="255" value="120"><b id="leftPwmValue">120</b></div>
        <div class="drive-grid">
          <button data-hold="L,1">Mantener adelante</button>
          <button class="danger" onclick="motorStop('L')">Detener</button>
          <button data-hold="L,-1">Mantener atrás</button>
        </div>
      </article>
      <article class="card">
        <div class="motor-head"><h3>Motor derecho</h3><span id="rightPwmLabel" class="pill">PWM 120</span></div>
        <div class="slider-row"><input id="rightPwm" type="range" min="0" max="255" value="120"><b id="rightPwmValue">120</b></div>
        <div class="drive-grid">
          <button data-hold="R,1">Mantener adelante</button>
          <button class="danger" onclick="motorStop('R')">Detener</button>
          <button data-hold="R,-1">Mantener atrás</button>
        </div>
      </article>

      <article class="card wide">
        <h3>Movimiento conjunto</h3>
        <p class="sub">Los motores se accionan únicamente mientras mantenés presionado el botón.</p>
        <div class="movement" style="margin-top:18px">
          <button class="mf primary" data-move="forward">▲ Adelante</button>
          <button class="ml" data-move="left">◀ Izquierda</button>
          <button class="ms danger" onclick="stopAll()">■ STOP</button>
          <button class="mr" data-move="right">Derecha ▶</button>
          <button class="mb" data-move="back">▼ Atrás</button>
        </div>
      </article>
    </div>
  </section>

  <section id="sensors" class="page">
    <div class="hero">
      <div><h2>Monitor de sensores MUX</h2><p class="sub">Perfil instantáneo e historial ADC de hasta 16 canales.</p></div>
      <div><span id="scanRate" class="pill">-- ms</span></div>
    </div>

    <div class="dashboard-metrics">
      <div class="metric"><small>Máximo ADC</small><strong id="sensorMax">0</strong><em id="sensorMaxCh">CH --</em></div>
      <div class="metric"><small>Mínimo ADC</small><strong id="sensorMin">0</strong><em id="sensorMinCh">CH --</em></div>
      <div class="metric"><small>Promedio</small><strong id="sensorAvg">0</strong><em>ADC</em></div>
      <div class="metric"><small>Rango</small><strong id="sensorRange">0</strong><em>MAX − MIN</em></div>
      <div class="metric"><small>Canales</small><strong id="sensorCount">0</strong><em>ACTIVOS</em></div>
      <div class="metric"><small>Actualización</small><strong id="sensorPeriod">--</strong><em>MILISEGUNDOS</em></div>
    </div>

    <article class="card wide chart-card">
      <div class="chart-head">
        <div><h3>Perfil instantáneo</h3><p class="sub">Lectura actual de todos los canales del CD74HC4067.</p></div>
        <span class="pill">Escala 0–4095</span>
      </div>
      <div class="chart-wrap"><canvas id="profileChart"></canvas></div>
    </article>

    <article class="card wide chart-card">
      <div class="chart-head">
        <div><h3>Historial en tiempo real</h3><p class="sub">Comparación temporal de dos canales seleccionados.</p></div>
        <div class="chart-tools">
          <label>Canal A<select id="historyChannelA" onchange="clearSensorHistory()"></select></label>
          <label>Canal B<select id="historyChannelB" onchange="clearSensorHistory()"></select></label>
          <button id="historyPause" onclick="toggleSensorHistory()">Pausar</button>
          <button onclick="clearSensorHistory()">Limpiar</button>
        </div>
      </div>
      <div class="legend"><span><i></i><b id="legendA">CH 0</b></span><span><i class="line-b"></i><b id="legendB">CH 1</b></span></div>
      <div class="chart-wrap history" style="margin-top:10px"><canvas id="historyChart"></canvas></div>
    </article>
  </section>
</main>
<footer>RMP Robotics · ESP32 Universal Test Bench</footer>

<script>
  var activePage='config', holdTimer=null, stateTimer=null, cfg=null;
  var sensorHistoryA=[],sensorHistoryB=[],sensorHistoryLimit=100,sensorHistoryPaused=false,lastSensorState=null;
  var fields=['muxS0','muxS1','muxS2','muxS3','muxSig','muxEnable','led','button',
    'motor1A','motor1B','motor1Pwm','motor2A','motor2B','motor2Pwm','motorEnable',
    'motorMode','buttonPull','muxChannels','adcSamples','scanIntervalMs'];
  var checks=['ledActiveHigh','buttonActiveLow','motorEnableActiveHigh','motor1Invert','motor2Invert'];

  document.querySelectorAll('.tab').forEach(function(b){
    b.onclick=function(){
      document.querySelectorAll('.tab').forEach(function(x){x.classList.remove('active')});
      document.querySelectorAll('.page').forEach(function(x){x.classList.remove('active')});
      b.classList.add('active'); activePage=b.dataset.page; document.getElementById(activePage).classList.add('active');
      if(activePage!=='actuators') stopAll();
      if(activePage==='sensors'&&lastSensorState)updateSensorDashboard(lastSensorState,false);
    };
  });

  function api(url,opt){
    return fetch(url,opt).then(function(r){if(!r.ok)return r.text().then(function(t){throw new Error(t)});return r.json()});
  }
  function loadConfig(){
    api('/api/config').then(function(c){
      cfg=c;
      fields.forEach(function(k){document.getElementById(k).value=c[k]});
      checks.forEach(function(k){document.getElementById(k).checked=!!c[k]});
      buildSensors(c.muxChannels);
    }).catch(showOffline);
  }
  function saveConfig(){
    var body=new URLSearchParams();
    fields.forEach(function(k){body.set(k,document.getElementById(k).value)});
    checks.forEach(function(k){body.set(k,document.getElementById(k).checked?'1':'0')});
    var msg=document.getElementById('configMessage');
    api('/api/config',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:body.toString()})
      .then(function(){msg.className='notice';msg.innerHTML='<b>Configuración guardada.</b> El ESP32 se está reiniciando. Reconectando...';setTimeout(function(){location.reload()},3500)})
      .catch(function(e){msg.className='notice error';msg.textContent=e.message});
  }
  function setLed(v){api('/api/led?state='+v,{method:'POST'}).catch(showOffline)}
  function motorCommand(side,dir){
    var pwm=document.getElementById(side==='L'?'leftPwm':'rightPwm').value;
    api('/api/motor?side='+side+'&dir='+dir+'&pwm='+pwm,{method:'POST'}).catch(showOffline);
  }
  function motorStop(side){api('/api/motor?side='+side+'&dir=0&pwm=0',{method:'POST'}).catch(showOffline)}
  function moveCommand(cmd){
    var l=document.getElementById('leftPwm').value,r=document.getElementById('rightPwm').value;
    api('/api/move?cmd='+cmd+'&left='+l+'&right='+r,{method:'POST'}).catch(showOffline);
  }
  function stopAll(){clearInterval(holdTimer);holdTimer=null;fetch('/api/stop',{method:'POST'}).catch(function(){})}
  function startHold(fn){
    stopAll();fn();holdTimer=setInterval(fn,250);
  }
  function bindHold(el,fn){
    ['pointerdown'].forEach(function(ev){el.addEventListener(ev,function(e){e.preventDefault();startHold(fn)})});
    ['pointerup','pointercancel','pointerleave'].forEach(function(ev){el.addEventListener(ev,function(e){e.preventDefault();stopAll()})});
  }
  document.querySelectorAll('[data-hold]').forEach(function(el){
    var p=el.dataset.hold.split(',');bindHold(el,function(){motorCommand(p[0],p[1])});
  });
  document.querySelectorAll('[data-move]').forEach(function(el){
    bindHold(el,function(){moveCommand(el.dataset.move)});
  });
  ['left','right'].forEach(function(side){
    var s=document.getElementById(side+'Pwm');
    s.oninput=function(){document.getElementById(side+'PwmValue').textContent=s.value;document.getElementById(side+'PwmLabel').textContent='PWM '+s.value};
  });
  window.addEventListener('blur',stopAll);
  document.addEventListener('visibilitychange',function(){if(document.hidden)stopAll()});

  function buildSensors(n){
    var a=document.getElementById('historyChannelA'),b=document.getElementById('historyChannelB');
    var oldA=parseInt(a.value||0),oldB=parseInt(b.value||Math.min(1,n-1));
    a.innerHTML='';b.innerHTML='';
    for(var i=0;i<n;i++){
      var oa=document.createElement('option'),ob=document.createElement('option');
      oa.value=i;oa.textContent='CH '+i;ob.value=i;ob.textContent='CH '+i;
      a.appendChild(oa);b.appendChild(ob);
    }
    a.value=Math.min(oldA,n-1);
    b.value=Math.min(oldB,n-1);
    document.getElementById('sensorCount').textContent=n;
    clearSensorHistory();
  }
  function clearSensorHistory(){
    sensorHistoryA=[];sensorHistoryB=[];
    drawSensorHistory();
  }
  function toggleSensorHistory(){
    sensorHistoryPaused=!sensorHistoryPaused;
    var button=document.getElementById('historyPause');
    button.textContent=sensorHistoryPaused?'Reanudar':'Pausar';
    button.className=sensorHistoryPaused?'success':'';
  }
  function prepareCanvas(id){
    var canvas=document.getElementById(id),rect=canvas.getBoundingClientRect(),ratio=window.devicePixelRatio||1;
    var w=Math.max(280,Math.floor(rect.width)),h=Math.max(180,Math.floor(rect.height));
    if(canvas.width!==Math.floor(w*ratio)||canvas.height!==Math.floor(h*ratio)){
      canvas.width=Math.floor(w*ratio);canvas.height=Math.floor(h*ratio);
    }
    var ctx=canvas.getContext('2d');
    ctx.setTransform(ratio,0,0,ratio,0,0);
    ctx.clearRect(0,0,w,h);
    return {ctx:ctx,w:w,h:h};
  }
  function drawChartGrid(ctx,w,h,left,top,right,bottom){
    var plotW=w-left-right,plotH=h-top-bottom;
    ctx.font='11px Arial';ctx.textAlign='right';ctx.textBaseline='middle';
    for(var i=0;i<=4;i++){
      var y=top+plotH*i/4,value=Math.round(4095*(4-i)/4);
      ctx.strokeStyle='#2b2635';ctx.lineWidth=1;ctx.beginPath();ctx.moveTo(left,y);ctx.lineTo(w-right,y);ctx.stroke();
      ctx.fillStyle='#777080';ctx.fillText(value,left-8,y);
    }
  }
  function drawSensorProfile(values,n){
    if(activePage!=='sensors')return;
    var p=prepareCanvas('profileChart'),ctx=p.ctx,w=p.w,h=p.h,left=46,right=12,top=14,bottom=34;
    drawChartGrid(ctx,w,h,left,top,right,bottom);
    var plotW=w-left-right,plotH=h-top-bottom,slot=plotW/n,barW=Math.max(5,slot*.62);
    var gradient=ctx.createLinearGradient(0,top,0,top+plotH);
    gradient.addColorStop(0,'#d8b4fe');gradient.addColorStop(1,'#7c3aed');
    ctx.textAlign='center';ctx.textBaseline='top';ctx.font='10px Arial';
    for(var i=0;i<n;i++){
      var value=values[i]||0,barH=plotH*Math.min(4095,value)/4095,x=left+slot*i+(slot-barW)/2,y=top+plotH-barH;
      ctx.fillStyle=gradient;ctx.fillRect(x,y,barW,barH);
      ctx.fillStyle='#8f8998';ctx.fillText(i,left+slot*i+slot/2,top+plotH+9);
      if(slot>48){ctx.fillStyle='#f8f7fb';ctx.textBaseline='bottom';ctx.fillText(value,left+slot*i+slot/2,y-4);ctx.textBaseline='top';}
    }
  }
  function drawSensorHistory(){
    if(activePage!=='sensors')return;
    var p=prepareCanvas('historyChart'),ctx=p.ctx,w=p.w,h=p.h,left=46,right=12,top=14,bottom=26;
    drawChartGrid(ctx,w,h,left,top,right,bottom);
    var plotW=w-left-right,plotH=h-top-bottom;
    function line(data,color){
      if(data.length<2)return;
      ctx.strokeStyle=color;ctx.lineWidth=2.2;ctx.lineJoin='round';ctx.lineCap='round';ctx.beginPath();
      for(var i=0;i<data.length;i++){
        var x=left+plotW*i/Math.max(1,data.length-1),y=top+plotH*(1-Math.min(4095,data[i])/4095);
        if(i===0)ctx.moveTo(x,y);else ctx.lineTo(x,y);
      }
      ctx.stroke();
    }
    line(sensorHistoryA,'#a855f7');line(sensorHistoryB,'#f8f7fb');
  }
  function updateSensorDashboard(s,addHistory){
    lastSensorState=s;
    if(!s.channels)return;
    var min=s.sensors[0],max=s.sensors[0],minCh=0,maxCh=0,sum=0;
    for(var i=0;i<s.channels;i++){
      var v=s.sensors[i];sum+=v;
      if(v<min){min=v;minCh=i}if(v>max){max=v;maxCh=i}
    }
    document.getElementById('sensorMax').textContent=max;
    document.getElementById('sensorMaxCh').textContent='CH '+maxCh;
    document.getElementById('sensorMin').textContent=min;
    document.getElementById('sensorMinCh').textContent='CH '+minCh;
    document.getElementById('sensorAvg').textContent=Math.round(sum/s.channels);
    document.getElementById('sensorRange').textContent=max-min;
    document.getElementById('sensorCount').textContent=s.channels;
    document.getElementById('sensorPeriod').textContent=s.scanIntervalMs;
    var a=parseInt(document.getElementById('historyChannelA').value||0);
    var b=parseInt(document.getElementById('historyChannelB').value||Math.min(1,s.channels-1));
    document.getElementById('legendA').textContent='CH '+a;
    document.getElementById('legendB').textContent='CH '+b;
    if(addHistory&&!sensorHistoryPaused){
      sensorHistoryA.push(s.sensors[a]||0);sensorHistoryB.push(s.sensors[b]||0);
      if(sensorHistoryA.length>sensorHistoryLimit)sensorHistoryA.shift();
      if(sensorHistoryB.length>sensorHistoryLimit)sensorHistoryB.shift();
    }
    drawSensorProfile(s.sensors,s.channels);drawSensorHistory();
  }
  function pollState(){
    api('/api/state').then(function(s){
      document.getElementById('connection').textContent='ESP32 conectado';
      document.querySelector('.dot').style.background='var(--ok)';
      var ls=document.getElementById('ledStatus');ls.textContent=s.led?'ENCENDIDO':'APAGADO';ls.className='pill '+(s.led?'on':'off');
      var bs=document.getElementById('buttonStatus');bs.textContent=s.button?'PRESIONADO':'LIBRE';bs.className='pill '+(s.button?'on':'off');
      document.getElementById('buttonCount').textContent=s.buttonCount;
      document.getElementById('motor1State').textContent=s.motor1;
      document.getElementById('motor2State').textContent=s.motor2;
      document.getElementById('scanRate').textContent=s.scanIntervalMs+' ms';
      if(!cfg||cfg.muxChannels!==s.channels){cfg=cfg||{};cfg.muxChannels=s.channels;buildSensors(s.channels)}
      updateSensorDashboard(s,true);
    }).catch(showOffline);
  }
  function showOffline(){
    document.getElementById('connection').textContent='Sin respuesta';
    document.querySelector('.dot').style.background='var(--danger)';
  }
  window.addEventListener('resize',function(){if(lastSensorState)updateSensorDashboard(lastSensorState,false)});
  loadConfig();pollState();stateTimer=setInterval(pollState,180);
</script>
</body>
</html>
)RMPHTML";

// ---------------------------------------------------------------------------
// Preferencias y validacion
// ---------------------------------------------------------------------------

void loadConfig() {
  preferences.begin("rmp-test", true);
  config.muxS0 = preferences.getChar("s0", -1);
  config.muxS1 = preferences.getChar("s1", -1);
  config.muxS2 = preferences.getChar("s2", -1);
  config.muxS3 = preferences.getChar("s3", -1);
  config.muxSig = preferences.getChar("sig", -1);
  config.muxEnable = preferences.getChar("muxen", -1);
  config.led = preferences.getChar("led", -1);
  config.button = preferences.getChar("btn", -1);
  config.motor1A = preferences.getChar("m1a", -1);
  config.motor1B = preferences.getChar("m1b", -1);
  config.motor1Pwm = preferences.getChar("m1p", -1);
  config.motor2A = preferences.getChar("m2a", -1);
  config.motor2B = preferences.getChar("m2b", -1);
  config.motor2Pwm = preferences.getChar("m2p", -1);
  config.motorEnable = preferences.getChar("men", -1);
  config.motorMode = preferences.getUChar("mmode", MOTOR_IN_IN);
  config.buttonPull = preferences.getUChar("bpull", BUTTON_PULLUP);
  config.ledActiveHigh = preferences.getBool("ledhi", true);
  config.buttonActiveLow = preferences.getBool("btnlow", true);
  config.motorEnableActiveHigh = preferences.getBool("menhi", true);
  config.motor1Invert = preferences.getBool("m1inv", false);
  config.motor2Invert = preferences.getBool("m2inv", false);
  config.muxChannels = preferences.getUChar("channels", 16);
  config.adcSamples = preferences.getUChar("samples", 4);
  config.scanIntervalMs = preferences.getUShort("scanms", 100);
  preferences.end();
}

void saveConfig() {
  preferences.begin("rmp-test", false);
  preferences.putChar("s0", config.muxS0);
  preferences.putChar("s1", config.muxS1);
  preferences.putChar("s2", config.muxS2);
  preferences.putChar("s3", config.muxS3);
  preferences.putChar("sig", config.muxSig);
  preferences.putChar("muxen", config.muxEnable);
  preferences.putChar("led", config.led);
  preferences.putChar("btn", config.button);
  preferences.putChar("m1a", config.motor1A);
  preferences.putChar("m1b", config.motor1B);
  preferences.putChar("m1p", config.motor1Pwm);
  preferences.putChar("m2a", config.motor2A);
  preferences.putChar("m2b", config.motor2B);
  preferences.putChar("m2p", config.motor2Pwm);
  preferences.putChar("men", config.motorEnable);
  preferences.putUChar("mmode", config.motorMode);
  preferences.putUChar("bpull", config.buttonPull);
  preferences.putBool("ledhi", config.ledActiveHigh);
  preferences.putBool("btnlow", config.buttonActiveLow);
  preferences.putBool("menhi", config.motorEnableActiveHigh);
  preferences.putBool("m1inv", config.motor1Invert);
  preferences.putBool("m2inv", config.motor2Invert);
  preferences.putUChar("channels", config.muxChannels);
  preferences.putUChar("samples", config.adcSamples);
  preferences.putUShort("scanms", config.scanIntervalMs);
  preferences.end();
}

bool validPin(int pin, bool optional = false) {
  if (optional && pin == -1) return true;
  return pin >= 0 && pin < SOC_GPIO_PIN_COUNT;
}

bool addUniquePin(int pin, int *pins, uint8_t &count) {
  if (pin < 0) return true;
  for (uint8_t i = 0; i < count; i++) {
    if (pins[i] == pin) return false;
  }
  pins[count++] = pin;
  return true;
}

String validateConfig(const PinConfig &candidate) {
  if (!validPin(candidate.muxS0) || !validPin(candidate.muxS1) ||
      !validPin(candidate.muxS2) || !validPin(candidate.muxS3) ||
      !validPin(candidate.muxSig)) {
    return "MUX: S0, S1, S2, S3 y SIG deben tener GPIO validos.";
  }
  if (!validPin(candidate.muxEnable, true) || !validPin(candidate.motorEnable, true)) {
    return "Los pines opcionales deben ser -1 o un GPIO valido.";
  }
  if (!validPin(candidate.led) || !validPin(candidate.button)) {
    return "LED y boton deben tener GPIO validos.";
  }
  if (!validPin(candidate.motor1A) || !validPin(candidate.motor1B) ||
      !validPin(candidate.motor2A) || !validPin(candidate.motor2B)) {
    return "Los pines A y B de ambos motores son obligatorios.";
  }
  if (candidate.motorMode == MOTOR_IN_IN_PWM &&
      (!validPin(candidate.motor1Pwm) || !validPin(candidate.motor2Pwm))) {
    return "El modo TB6612 requiere un pin PWM para cada motor.";
  }
  if (candidate.motorMode > MOTOR_IN_IN_PWM) return "Modo de motor invalido.";
  if (candidate.muxChannels < 4 || candidate.muxChannels > 16) return "La cantidad de canales debe estar entre 4 y 16.";
  if (candidate.adcSamples < 1 || candidate.adcSamples > 32) return "Las muestras ADC deben estar entre 1 y 32.";
  if (candidate.scanIntervalMs < 20 || candidate.scanIntervalMs > 1000) return "El intervalo debe estar entre 20 y 1000 ms.";

  int pins[20];
  uint8_t count = 0;
  int requiredPins[] = {
    candidate.muxS0, candidate.muxS1, candidate.muxS2, candidate.muxS3,
    candidate.muxSig, candidate.muxEnable, candidate.led, candidate.button,
    candidate.motor1A, candidate.motor1B, candidate.motor2A, candidate.motor2B,
    candidate.motorEnable
  };
  for (uint8_t i = 0; i < sizeof(requiredPins) / sizeof(requiredPins[0]); i++) {
    if (!addUniquePin(requiredPins[i], pins, count)) return "Hay GPIO repetidos. Cada funcion necesita un pin diferente.";
  }
  if (candidate.motorMode == MOTOR_IN_IN_PWM) {
    if (!addUniquePin(candidate.motor1Pwm, pins, count) || !addUniquePin(candidate.motor2Pwm, pins, count)) {
      return "Hay GPIO repetidos en los pines PWM.";
    }
  }
  return "";
}

int argInt(const char *name) {
  return server.arg(name).toInt();
}

// ---------------------------------------------------------------------------
// PWM y motores
// ---------------------------------------------------------------------------

void attachPwmPin(int pin) {
  if (pin < 0) return;
  for (uint8_t i = 0; i < pwmPinCount; i++) {
    if (pwmPins[i] == pin) return;
  }
  if (pwmPinCount >= 6) return;
  pwmPins[pwmPinCount] = pin;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(pin, PWM_FREQUENCY, PWM_RESOLUTION);
#else
  ledcSetup(pwmPinCount, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(pin, pwmPinCount);
#endif
  pwmPinCount++;
}

void writePwmPin(int pin, uint8_t duty) {
  if (pin < 0) return;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(pin, duty);
#else
  for (uint8_t i = 0; i < pwmPinCount; i++) {
    if (pwmPins[i] == pin) {
      ledcWrite(i, duty);
      return;
    }
  }
#endif
}

void configurePwmPins() {
  pwmPinCount = 0;
  for (uint8_t i = 0; i < 6; i++) pwmPins[i] = -1;

  if (config.motorMode == MOTOR_IN_IN) {
    attachPwmPin(config.motor1A);
    attachPwmPin(config.motor1B);
    attachPwmPin(config.motor2A);
    attachPwmPin(config.motor2B);
  } else if (config.motorMode == MOTOR_PWM_DIR) {
    attachPwmPin(config.motor1A);
    attachPwmPin(config.motor2A);
  } else {
    attachPwmPin(config.motor1Pwm);
    attachPwmPin(config.motor2Pwm);
  }
}

void enableMotorDriver(bool enabled) {
  if (config.motorEnable < 0) return;
  bool level = enabled ? config.motorEnableActiveHigh : !config.motorEnableActiveHigh;
  digitalWrite(config.motorEnable, level ? HIGH : LOW);
}

void writeOneMotor(uint8_t motor, int16_t command) {
  command = constrain((int)command, -255, 255);
  int pinA = motor == 1 ? config.motor1A : config.motor2A;
  int pinB = motor == 1 ? config.motor1B : config.motor2B;
  int pinPwm = motor == 1 ? config.motor1Pwm : config.motor2Pwm;
  bool invert = motor == 1 ? config.motor1Invert : config.motor2Invert;
  if (invert) command = -command;

  uint8_t pwm = abs(command);
  bool forward = command >= 0;

  if (config.motorMode == MOTOR_IN_IN) {
    if (command == 0) {
      writePwmPin(pinA, 0);
      writePwmPin(pinB, 0);
    } else if (forward) {
      writePwmPin(pinA, pwm);
      writePwmPin(pinB, 0);
    } else {
      writePwmPin(pinA, 0);
      writePwmPin(pinB, pwm);
    }
  } else if (config.motorMode == MOTOR_PWM_DIR) {
    digitalWrite(pinB, forward ? HIGH : LOW);
    writePwmPin(pinA, pwm);
  } else {
    if (command == 0) {
      digitalWrite(pinA, LOW);
      digitalWrite(pinB, LOW);
      writePwmPin(pinPwm, 0);
    } else {
      digitalWrite(pinA, forward ? HIGH : LOW);
      digitalWrite(pinB, forward ? LOW : HIGH);
      writePwmPin(pinPwm, pwm);
    }
  }
}

void setMotors(int16_t left, int16_t right) {
  motor1Command = constrain((int)left, -255, 255);
  motor2Command = constrain((int)right, -255, 255);
  enableMotorDriver(motor1Command != 0 || motor2Command != 0);
  writeOneMotor(1, motor1Command);
  writeOneMotor(2, motor2Command);
  lastMotorCommandAt = millis();
}

void stopMotors() {
  motor1Command = 0;
  motor2Command = 0;
  if (!hardwareReady) return;
  writeOneMotor(1, 0);
  writeOneMotor(2, 0);
  enableMotorDriver(false);
}

// ---------------------------------------------------------------------------
// Hardware
// ---------------------------------------------------------------------------

void applyLed(bool enabled) {
  ledState = enabled;
  if (config.led < 0) return;
  bool level = enabled ? config.ledActiveHigh : !config.ledActiveHigh;
  digitalWrite(config.led, level ? HIGH : LOW);
}

void initHardware() {
  pinMode(config.muxS0, OUTPUT);
  pinMode(config.muxS1, OUTPUT);
  pinMode(config.muxS2, OUTPUT);
  pinMode(config.muxS3, OUTPUT);
  pinMode(config.muxSig, INPUT);
  if (config.muxEnable >= 0) {
    pinMode(config.muxEnable, OUTPUT);
    digitalWrite(config.muxEnable, LOW);
  }

  pinMode(config.led, OUTPUT);
  applyLed(false);

  if (config.buttonPull == BUTTON_PULLUP) pinMode(config.button, INPUT_PULLUP);
  else if (config.buttonPull == BUTTON_PULLDOWN) pinMode(config.button, INPUT_PULLDOWN);
  else pinMode(config.button, INPUT);

  pinMode(config.motor1A, OUTPUT);
  pinMode(config.motor1B, OUTPUT);
  pinMode(config.motor2A, OUTPUT);
  pinMode(config.motor2B, OUTPUT);
  if (config.motorMode == MOTOR_IN_IN_PWM) {
    pinMode(config.motor1Pwm, OUTPUT);
    pinMode(config.motor2Pwm, OUTPUT);
  }
  if (config.motorEnable >= 0) pinMode(config.motorEnable, OUTPUT);

  configurePwmPins();
  hardwareReady = true;
  stopMotors();
  analogReadResolution(12);
}

void selectMuxChannel(uint8_t channel) {
  digitalWrite(config.muxS0, bitRead(channel, 0));
  digitalWrite(config.muxS1, bitRead(channel, 1));
  digitalWrite(config.muxS2, bitRead(channel, 2));
  digitalWrite(config.muxS3, bitRead(channel, 3));
}

void scanMux() {
  if (millis() - lastMuxScanAt < config.scanIntervalMs) return;
  lastMuxScanAt = millis();

  for (uint8_t channel = 0; channel < config.muxChannels; channel++) {
    selectMuxChannel(channel);
    delayMicroseconds(8);
    uint32_t sum = 0;
    for (uint8_t sample = 0; sample < config.adcSamples; sample++) {
      sum += analogRead(config.muxSig);
    }
    muxValues[channel] = sum / config.adcSamples;
  }
}

void updateButton() {
  bool rawLevel = digitalRead(config.button);
  bool rawPressed = config.buttonActiveLow ? !rawLevel : rawLevel;
  if (rawPressed != lastButtonRaw) {
    lastButtonRaw = rawPressed;
    buttonChangedAt = millis();
  }
  if (millis() - buttonChangedAt >= 25 && buttonPressed != rawPressed) {
    buttonPressed = rawPressed;
    if (buttonPressed) buttonPressCount++;
  }
}

// ---------------------------------------------------------------------------
// API web
// ---------------------------------------------------------------------------

void sendJson(const String &body, int code = 200) {
  server.sendHeader("Cache-Control", "no-store");
  server.send(code, "application/json", body);
}

String boolJson(bool value) {
  return value ? "true" : "false";
}

void handleGetConfig() {
  String json = "{";
  json += "\"muxS0\":" + String(config.muxS0);
  json += ",\"muxS1\":" + String(config.muxS1);
  json += ",\"muxS2\":" + String(config.muxS2);
  json += ",\"muxS3\":" + String(config.muxS3);
  json += ",\"muxSig\":" + String(config.muxSig);
  json += ",\"muxEnable\":" + String(config.muxEnable);
  json += ",\"led\":" + String(config.led);
  json += ",\"button\":" + String(config.button);
  json += ",\"motor1A\":" + String(config.motor1A);
  json += ",\"motor1B\":" + String(config.motor1B);
  json += ",\"motor1Pwm\":" + String(config.motor1Pwm);
  json += ",\"motor2A\":" + String(config.motor2A);
  json += ",\"motor2B\":" + String(config.motor2B);
  json += ",\"motor2Pwm\":" + String(config.motor2Pwm);
  json += ",\"motorEnable\":" + String(config.motorEnable);
  json += ",\"motorMode\":" + String(config.motorMode);
  json += ",\"buttonPull\":" + String(config.buttonPull);
  json += ",\"ledActiveHigh\":" + boolJson(config.ledActiveHigh);
  json += ",\"buttonActiveLow\":" + boolJson(config.buttonActiveLow);
  json += ",\"motorEnableActiveHigh\":" + boolJson(config.motorEnableActiveHigh);
  json += ",\"motor1Invert\":" + boolJson(config.motor1Invert);
  json += ",\"motor2Invert\":" + boolJson(config.motor2Invert);
  json += ",\"muxChannels\":" + String(config.muxChannels);
  json += ",\"adcSamples\":" + String(config.adcSamples);
  json += ",\"scanIntervalMs\":" + String(config.scanIntervalMs);
  json += "}";
  sendJson(json);
}

void handleSaveConfig() {
  PinConfig candidate = config;
  candidate.muxS0 = argInt("muxS0");
  candidate.muxS1 = argInt("muxS1");
  candidate.muxS2 = argInt("muxS2");
  candidate.muxS3 = argInt("muxS3");
  candidate.muxSig = argInt("muxSig");
  candidate.muxEnable = argInt("muxEnable");
  candidate.led = argInt("led");
  candidate.button = argInt("button");
  candidate.motor1A = argInt("motor1A");
  candidate.motor1B = argInt("motor1B");
  candidate.motor1Pwm = argInt("motor1Pwm");
  candidate.motor2A = argInt("motor2A");
  candidate.motor2B = argInt("motor2B");
  candidate.motor2Pwm = argInt("motor2Pwm");
  candidate.motorEnable = argInt("motorEnable");
  candidate.motorMode = argInt("motorMode");
  candidate.buttonPull = argInt("buttonPull");
  candidate.ledActiveHigh = argInt("ledActiveHigh") == 1;
  candidate.buttonActiveLow = argInt("buttonActiveLow") == 1;
  candidate.motorEnableActiveHigh = argInt("motorEnableActiveHigh") == 1;
  candidate.motor1Invert = argInt("motor1Invert") == 1;
  candidate.motor2Invert = argInt("motor2Invert") == 1;
  candidate.muxChannels = argInt("muxChannels");
  candidate.adcSamples = argInt("adcSamples");
  candidate.scanIntervalMs = argInt("scanIntervalMs");

  String error = validateConfig(candidate);
  if (error.length()) {
    sendJson(String("{\"ok\":false,\"error\":\"") + error + "\"}", 400);
    return;
  }

  stopMotors();
  hardwareReady = false;
  config = candidate;
  saveConfig();
  sendJson("{\"ok\":true,\"restarting\":true}");
  restartAt = millis() + 700;
}

void handleState() {
  String json;
  json.reserve(360);
  json = "{";
  json += "\"led\":" + boolJson(ledState);
  json += ",\"button\":" + boolJson(buttonPressed);
  json += ",\"buttonCount\":" + String(buttonPressCount);
  json += ",\"motor1\":" + String(motor1Command);
  json += ",\"motor2\":" + String(motor2Command);
  json += ",\"channels\":" + String(config.muxChannels);
  json += ",\"scanIntervalMs\":" + String(config.scanIntervalMs);
  json += ",\"sensors\":[";
  for (uint8_t i = 0; i < config.muxChannels; i++) {
    if (i) json += ",";
    json += String(muxValues[i]);
  }
  json += "]}";
  sendJson(json);
}

void handleLed() {
  if (!hardwareReady) {
    sendJson("{\"ok\":false,\"error\":\"Primero guarda una configuracion de pines valida\"}", 409);
    return;
  }
  applyLed(server.arg("state").toInt() == 1);
  sendJson(String("{\"ok\":true,\"state\":") + boolJson(ledState) + "}");
}

void handleMotor() {
  if (!hardwareReady) {
    sendJson("{\"ok\":false,\"error\":\"Primero guarda una configuracion de pines valida\"}", 409);
    return;
  }
  String side = server.arg("side");
  int direction = constrain((int)server.arg("dir").toInt(), -1, 1);
  int pwm = constrain((int)server.arg("pwm").toInt(), 0, 255);
  int16_t command = direction * pwm;
  if (side == "L") setMotors(command, motor2Command);
  else if (side == "R") setMotors(motor1Command, command);
  else {
    sendJson("{\"ok\":false,\"error\":\"Lado invalido\"}", 400);
    return;
  }
  sendJson("{\"ok\":true}");
}

void handleMove() {
  if (!hardwareReady) {
    sendJson("{\"ok\":false,\"error\":\"Primero guarda una configuracion de pines valida\"}", 409);
    return;
  }
  String command = server.arg("cmd");
  int left = constrain((int)server.arg("left").toInt(), 0, 255);
  int right = constrain((int)server.arg("right").toInt(), 0, 255);
  if (command == "forward") setMotors(left, right);
  else if (command == "back") setMotors(-left, -right);
  else if (command == "left") setMotors(-left, right);
  else if (command == "right") setMotors(left, -right);
  else {
    sendJson("{\"ok\":false,\"error\":\"Movimiento invalido\"}", 400);
    return;
  }
  sendJson("{\"ok\":true}");
}

void setupRoutes() {
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });
  server.on("/api/config", HTTP_GET, handleGetConfig);
  server.on("/api/config", HTTP_POST, handleSaveConfig);
  server.on("/api/state", HTTP_GET, handleState);
  server.on("/api/led", HTTP_POST, handleLed);
  server.on("/api/motor", HTTP_POST, handleMotor);
  server.on("/api/move", HTTP_POST, handleMove);
  server.on("/api/stop", HTTP_POST, []() {
    stopMotors();
    sendJson("{\"ok\":true}");
  });
  server.onNotFound([]() {
    if (server.uri().startsWith("/api/")) sendJson("{\"ok\":false,\"error\":\"Ruta no encontrada\"}", 404);
    else server.sendHeader("Location", "/", true), server.send(302, "text/plain", "");
  });
}

// ---------------------------------------------------------------------------
// Arduino
// ---------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("RMP Robotics - Banco de pruebas ESP32");

  loadConfig();
  String configError = validateConfig(config);
  if (configError.length() == 0) {
    initHardware();
    Serial.println("Configuracion de pines cargada.");
  } else {
    Serial.println("Configuracion pendiente: " + configError);
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.print("Red: ");
  Serial.println(AP_SSID);
  Serial.print("Panel: http://");
  Serial.println(WiFi.softAPIP());

  setupRoutes();
  server.begin();
}

void loop() {
  server.handleClient();

  if (hardwareReady) {
    updateButton();
    scanMux();
    if ((motor1Command != 0 || motor2Command != 0) &&
        millis() - lastMotorCommandAt > MOTOR_TIMEOUT_MS) {
      stopMotors();
    }
  }

  if (restartAt != 0 && (int32_t)(millis() - restartAt) >= 0) {
    ESP.restart();
  }
  delay(1);
}
