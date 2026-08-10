#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "Provisioning.h"

Provisioning::Provisioning(
    ConfigurationManager& configuration)
    :
    _configuration(configuration)
{
}
bool Provisioning::Begin()
{
    auto& config = _configuration.GetConfiguration();

    if (config.IsProvisioned &&
        !config.WifiSSID.isEmpty())
    {
        WiFi.mode(WIFI_STA);

        WiFi.begin(
            config.WifiSSID.c_str(),
            config.WifiPassword.c_str());

        const unsigned long start = millis();

        while (WiFi.status() != WL_CONNECTED &&
               millis() - start < 15000)
        {
            delay(250);
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            _running = false;
            return true;
        }

        WiFi.disconnect(true);
    }

    StartAccessPoint();
    ConfigureRoutes();
    _server.begin();

    _running = true;
    return true;
}
void Provisioning::Loop()
{
    if(!_running)
        return;

    _dns.processNextRequest();

    _server.handleClient();
}
bool Provisioning::IsRunning() const
{
    return _running;
}
void Provisioning::ConfigureRoutes()
{
    _server.on(
        "/",
        HTTP_GET,
        std::bind(
            &Provisioning::HandleRoot,
            this));

    _server.on(
        "/style.css",
        HTTP_GET,
        std::bind(
            &Provisioning::HandleStyle,
            this));

    _server.on(
        "/app.js",
        HTTP_GET,
        std::bind(
            &Provisioning::HandleScript,
            this));

    _server.on(
        "/api/scan",
        HTTP_GET,
        std::bind(
            &Provisioning::HandleScan,
            this));

    _server.on(
        "/api/provision",
        HTTP_POST,
        std::bind(
            &Provisioning::HandleProvision,
            this));

    _server.onNotFound(
        std::bind(
            &Provisioning::HandleNotFound,
            this));
}


void Provisioning::StartAccessPoint()
{
    WiFi.mode(WIFI_AP);

    // Keep the provisioning network deterministic so the user can always
    // reach the controller even when no router is available.
    IPAddress apIp(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.softAPConfig(apIp, gateway, subnet);
    WiFi.softAP("Home-Security-Setup");

    // Captive-portal DNS: every hostname resolves to the controller.
    _dns.setErrorReplyCode(DNSReplyCode::NoError);
    _dns.start(53, "*", apIp);
}

void Provisioning::HandleRoot()
{
    const char* page = R"HSHHTML(
<!DOCTYPE html>

<html lang=\"en\">

<head>

    <meta charset=\"UTF-8\">

    <meta
        name=\"viewport\"
        content=\"width=device-width, initial-scale=1.0\">

    <title>Home Security Controller Setup</title>

    <link
        rel=\"stylesheet\"
        href=\"style.css\">

</head>

<body>

<div class=\"container\">

<div class=\"card\">

<!-- ========================================= -->
<!-- Header                                    -->
<!-- ========================================= -->

<h1>Home Security Controller</h1>

<p>
Configure your room controller before
connecting it to your Home Security server.
</p>

<!-- ========================================= -->
<!-- Stepper                                   -->
<!-- ========================================= -->

<div class=\"stepper\">

    <div class=\"step active\">

        <div class=\"step-circle\">1</div>

        <div class=\"step-title\">
            Welcome
        </div>

    </div>

    <div class=\"step\">

        <div class=\"step-circle\">2</div>

        <div class=\"step-title\">
            Device
        </div>

    </div>

    <div class=\"step\">

        <div class=\"step-circle\">3</div>

        <div class=\"step-title\">
            WiFi
        </div>

    </div>

    <div class=\"step\">

        <div class=\"step-circle\">4</div>

        <div class=\"step-title\">
            Server
        </div>

    </div>

    <div class=\"step\">

        <div class=\"step-circle\">5</div>

        <div class=\"step-title\">
            Review
        </div>

    </div>

</div>

<!-- ========================================= -->
<!-- Welcome                                   -->
<!-- ========================================= -->

<section
    id=\"welcomePage\"
    class=\"page active\">

<h2>
Welcome
</h2>

<p>

This controller has not been configured.

Before continuing make sure you have:

</p>

<ul>

<li>Hardware ID</li>

<li>Authentication Key</li>

<li>WiFi Credentials</li>

<li>Backend URL</li>

</ul>

<div class=\"buttons\">

<button
    class=\"full-width\"
    data-next>

Get Started

</button>

</div>

</section>

<!-- ========================================= -->
<!-- Device Page                               -->
<!-- ========================================= -->

<section
    id=\"devicePage\"
    class=\"page\">

<h2>

Device Information

</h2>

<div class=\"form-group\">

<label>

Hardware ID

</label>

<input

id=\"hardwareId\"

type=\"text\"

placeholder=\"Hardware ID\"

autocomplete=\"off\"

spellcheck=\"false\">

</div>

<div class=\"form-group\">

<label>

Authentication Key

</label>

<input

id=\"authKey\"

type=\"password\"

placeholder=\"Authentication Key\"

autocomplete=\"off\"

spellcheck=\"false\">

</div>

<div class=\"buttons\">

<button
class=\"secondary\"
data-back>

Back

</button>

<button

onclick=\"validateDevice().then(r=>{

if(r)nextPage();

})\">

Next

</button>

</div>

</section>

<!-- ========================================= -->
<!-- WiFi                                      -->
<!-- ========================================= -->

<section

id=\"wifiPage\"

class=\"page\">

<h2>

Network Configuration

</h2>

<div

id=\"wifiNetworks\"

class=\"wifi-list\">

Scanning...

</div>

<div class=\"divider\"></div>

<label>

Manual SSID

</label>

<div class=\"manual-network active\">

<div class=\"form-group\">

<label>

SSID

</label>

<input

id=\"wifiSsid\"

type=\"text\"

placeholder=\"WiFi Name\"

autocomplete=\"off\">

</div>

<div class=\"form-group\">

<label>

Password

</label>

<input

id=\"wifiPassword\"

type=\"password\"

placeholder=\"WiFi Password\"

autocomplete=\"off\">

</div>

</div>

<div class=\"buttons\">

<button

class=\"secondary\"

data-back>

Back

</button>

<button

onclick=\"validateWiFi().then(r=>{

if(r)nextPage();

})\">

Next

</button>

</div>

</section>

</div>

</div>

<!-- ========================================= -->
<!-- Loading Overlay                           -->
<!-- ========================================= -->

<div

id=\"loadingOverlay\"

class=\"loading\">

<div

class=\"loading-content\">

<div

class=\"spinner\">

</div>

<p

id=\"loadingText\"

class=\"loading-text\">

Loading...

</p>

</div>

</div>

<!-- ========================================= -->
<!-- Toast                                     -->
<!-- ========================================= -->

<div

id=\"toast\"

class=\"toast\">

Ready

</div>
<!-- ========================================= -->
<!-- Server Page                               -->
<!-- ========================================= -->

<section
    id=\"serverPage\"
    class=\"page\">

<h2>

Server Configuration

</h2>

<div class=\"form-group\">

<label>

Backend URL

</label>

<input

id=\"backendUrl\"

type=\"url\"

placeholder=\"https://api.example.com\"

autocomplete=\"off\"

spellcheck=\"false\"

autocorrect=\"off\"

autocapitalize=\"off\">

<p class=\"help\">

The backend will automatically provide the MQTT configuration.

</p>

</div>

<div class=\"buttons\">

<button
class=\"secondary\"
data-back>

Back

</button>

<button

onclick=\"validateServer().then(r=>{

if(r){

populateReview();

nextPage();

}

})\">

Next

</button>

</div>

</section>

<!-- ========================================= -->
<!-- Review Page                               -->
<!-- ========================================= -->

<section

id=\"reviewPage\"

class=\"page\">

<h2>

Review Configuration

</h2>

<p>

Please verify the information below before saving.

</p>

<div class=\"review\">

<div class=\"review-title\">

Hardware ID

</div>

<div

id=\"reviewHardware\"

class=\"review-value\">

-

</div>

<div class=\"review-title\">

WiFi Network

</div>

<div

id=\"reviewWifi\"

class=\"review-value\">

-

</div>

<div class=\"review-title\">

Backend URL

</div>

<div

id=\"reviewServer\"

class=\"review-value\">

-

</div>

</div>

<div class=\"divider\"></div>

<div class=\"buttons\">

<button

class=\"secondary\"

data-back>

Back

</button>

<button

onclick=\"provision()\">

Save Configuration

</button>

</div>

</section>

<!-- ========================================= -->
<!-- Success Page                              -->
<!-- ========================================= -->

<section

id=\"successPage\"

class=\"page\">

<div class=\"success\">

<div class=\"success-icon\">

✓

</div>

<h2>

Configuration Saved

</h2>

<p>

Your Home Security Controller has been configured successfully.

</p>

<p>

The controller will restart automatically in

</p>

<div

id=\"countdown\"

class=\"countdown\">

5

</div>

<p>

Please wait while the device restarts.

</p>

</div>

</section>

</div>

</div>

<!-- ========================================= -->
<!-- JavaScript                                -->
<!-- ========================================= -->

<script src=\"app.js\"></script>

</body>

</html>
)HSHHTML";
    _server.send(200, "text/html", page);
}

void Provisioning::HandleStyle()
{
    const char* css = R"HSHCSS(
/*==================================================
    HOME SECURITY PROVISIONING
==================================================*/

:root{
    --primary:#22c55e;
    --primary-hover:#16a34a;

    --danger:#ef4444;

    --background:#f5f5f5;
    --surface:#ffffff;

    --text:#1e293b;
    --text-light:#64748b;

    --border:#d4d4d4;

    --shadow:
        0 10px 30px rgba(0,0,0,.08);

    --transition:.25s ease;
}

@media(prefers-color-scheme:dark){

:root{

    --background:#0f172a;

    --surface:#1e293b;

    --text:#f8fafc;

    --text-light:#94a3b8;

    --border:#475569;

    --shadow:
        0 15px 40px rgba(0,0,0,.4);

}

}

/*==================================================
    RESET
==================================================*/

*{

    margin:0;

    padding:0;

    box-sizing:border-box;

    font-family:
        Segoe UI,
        Arial,
        sans-serif;

}

html{

    scroll-behavior:smooth;

}

body{

    background:var(--background);

    color:var(--text);

    min-height:100vh;

    display:flex;

    justify-content:center;

    align-items:center;

    padding:30px;

}

/*==================================================
    CARD
==================================================*/

.container{

    width:100%;

    max-width:700px;

}

.card{

    background:var(--surface);

    box-shadow:var(--shadow);

    padding:40px;

    animation:fade .35s;

}

.page{

    display:none;

    animation:slide .3s ease;

}

.page.active{

    display:block;

}

h1{

    margin-bottom:10px;

    font-size:32px;

    font-weight:600;

}

h2{

    margin-bottom:25px;

    font-size:24px;

}

h3{

    margin-bottom:10px;

}

p{

    color:var(--text-light);

    line-height:1.7;

    margin-bottom:20px;

}

/*==================================================
    STEPPER
==================================================*/

.stepper{

    display:flex;

    justify-content:space-between;

    margin-bottom:40px;

    gap:15px;

}

.step{

    flex:1;

    text-align:center;

    position:relative;

}

.step::after{

    content:\"\";

    position:absolute;

    top:14px;

    left:60%;

    width:80%;

    height:2px;

    background:var(--border);

}

.step:last-child::after{

    display:none;

}

.step-circle{

    width:28px;

    height:28px;

    margin:auto;

    border:2px solid var(--border);

    display:flex;

    justify-content:center;

    align-items:center;

    transition:var(--transition);

}

.step-title{

    margin-top:10px;

    font-size:13px;

}

.step.active .step-circle{

    background:var(--primary);

    border-color:var(--primary);

    color:white;

}

.step.completed .step-circle{

    background:var(--primary);

    border-color:var(--primary);

    color:white;

}

.step.completed::after{

    background:var(--primary);

}

/*==================================================
    FORM
==================================================*/

.form-group{

    margin-bottom:35px;

}

label{

    display:block;

    margin-bottom:10px;

    font-size:15px;

    color:var(--text-light);

}

input{

    width:100%;

    background:transparent;

    border:none;

    border-bottom:2px solid var(--border);

    outline:none;

    border-radius:0;

    box-shadow:none;

    color:var(--text);

    padding:12px 0;

    font-size:16px;

    transition:var(--transition);

}

input::placeholder{

    color:#999;

}

input:focus{

    outline:none;

    box-shadow:none;

}

input.valid{

    border-bottom:2px solid var(--primary);

}

input.invalid{

    border-bottom:2px solid var(--danger);

}

/*==================================================
    BUTTONS
==================================================*/

.buttons{

    margin-top:30px;

    display:flex;

    justify-content:space-between;

    gap:15px;

}

button{

    padding:14px 28px;

    background:var(--primary);

    color:white;

    border:none;

    cursor:pointer;

    transition:var(--transition);

    font-size:15px;

}

button:hover{

    background:var(--primary-hover);

}

button:active{

    transform:scale(.98);

}

button.secondary{

    background:transparent;

    border:1px solid var(--border);

    color:var(--text);

}

/*==================================================
    REVIEW
==================================================*/

.review{

    display:grid;

    grid-template-columns:170px auto;

    gap:18px;

    margin-top:20px;

}

.review-title{

    font-weight:600;

}

.review-value{

    color:var(--text-light);

}

/*==================================================
    ANIMATIONS
==================================================*/

@keyframes fade{

from{

opacity:0;

}

to{

opacity:1;

}

}

@keyframes slide{

from{

opacity:0;

transform:translateX(25px);

}

to{

opacity:1;

transform:translateX(0);

}

}

/*==================================================
    RESPONSIVE
==================================================*/

@media(max-width:768px){

.card{

padding:25px;

}

.step-title{

display:none;

}

.buttons{

flex-direction:column;

}

button{

width:100%;

}

.review{

grid-template-columns:1fr;

}

}
/*==================================================
    WIFI NETWORK LIST
==================================================*/

.wifi-list{

    display:flex;

    flex-direction:column;

    gap:10px;

    margin:25px 0;

    max-height:260px;

    overflow-y:auto;

}

.wifi-item{

    display:flex;

    align-items:center;

    justify-content:space-between;

    gap:15px;

    padding:14px 18px;

    cursor:pointer;

    transition:var(--transition);

    border-left:3px solid transparent;

}

.wifi-item:hover{

    background:rgba(34,197,94,.08);

}

.wifi-item.selected{

    border-left-color:var(--primary);

    background:rgba(34,197,94,.15);

}

.wifi-item input{

    display:none;

}

.wifi-name{

    flex:1;

    font-weight:500;

}

.wifi-strength{

    color:var(--text-light);

    font-size:14px;

}

/*==================================================
    MANUAL SSID
==================================================*/

.manual-network{

    margin-top:25px;

    display:none;

}

.manual-network.active{

    display:block;

    animation:fade .25s;

}

/*==================================================
    LOADING OVERLAY
==================================================*/

.loading{

    position:fixed;

    inset:0;

    background:rgba(0,0,0,.55);

    display:none;

    justify-content:center;

    align-items:center;

    z-index:9999;

}

.loading-content{

    background:var(--surface);

    padding:40px;

    min-width:280px;

    text-align:center;

    box-shadow:var(--shadow);

}

.loading-text{

    margin-top:20px;

    color:var(--text-light);

}

/*==================================================
    SPINNER
==================================================*/

.spinner{

    width:55px;

    height:55px;

    margin:auto;

    border:4px solid rgba(34,197,94,.15);

    border-top:4px solid var(--primary);

    border-radius:50%;

    animation:spin 1s linear infinite;

}

@keyframes spin{

from{

transform:rotate(0deg);

}

to{

transform:rotate(360deg);

}

}

/*==================================================
    SUCCESS PAGE
==================================================*/

.success{

    text-align:center;

    padding:30px 0;

}

.success-icon{

    width:90px;

    height:90px;

    margin:auto;

    margin-bottom:25px;

    background:var(--primary);

    color:white;

    font-size:46px;

    display:flex;

    justify-content:center;

    align-items:center;

}

.success h2{

    margin-bottom:10px;

}

.success p{

    margin-bottom:30px;

}

.countdown{

    font-size:72px;

    color:var(--primary);

    font-weight:bold;

    margin:20px 0;

}

/*==================================================
    STATUS MESSAGE
==================================================*/

.status{

    margin-top:15px;

    text-align:center;

    font-size:15px;

    color:var(--text-light);

}

.status.success{

    color:var(--primary);

}

.status.error{

    color:var(--danger);

}

/*==================================================
    TOAST
==================================================*/

.toast{

    position:fixed;

    bottom:25px;

    right:25px;

    background:var(--surface);

    color:var(--text);

    padding:16px 22px;

    box-shadow:var(--shadow);

    opacity:0;

    pointer-events:none;

    transform:translateY(20px);

    transition:.3s;

    z-index:999;

}

.toast.show{

    opacity:1;

    transform:translateY(0);

}

.toast.success{

    border-left:4px solid var(--primary);

}

.toast.error{

    border-left:4px solid var(--danger);

}

/*==================================================
    BADGES
==================================================*/

.badge{

    display:inline-block;

    padding:5px 10px;

    font-size:12px;

    background:rgba(34,197,94,.15);

    color:var(--primary);

}

.badge.error{

    background:rgba(239,68,68,.15);

    color:var(--danger);

}

/*==================================================
    HORIZONTAL DIVIDER
==================================================*/

.divider{

    height:1px;

    background:var(--border);

    margin:30px 0;

}

/*==================================================
    HELP TEXT
==================================================*/

.help{

    margin-top:6px;

    font-size:13px;

    color:var(--text-light);

}

/*==================================================
    SCROLLBAR
==================================================*/

::-webkit-scrollbar{

    width:8px;

}

::-webkit-scrollbar-track{

    background:transparent;

}

::-webkit-scrollbar-thumb{

    background:var(--primary);

}

::-webkit-scrollbar-thumb:hover{

    background:var(--primary-hover);

}

/*==================================================
    UTILITIES
==================================================*/

.hidden{

    display:none!important;

}

.center{

    text-align:center;

}

.full-width{

    width:100%;

}

.mt-10{

    margin-top:10px;

}

.mt-20{

    margin-top:20px;

}

.mt-30{

    margin-top:30px;

}

.mb-10{

    margin-bottom:10px;

}

.mb-20{

    margin-bottom:20px;

}

.mb-30{

    margin-bottom:30px;

}

/*==================================================
    PAGE TRANSITIONS
==================================================*/

.fade-in{

    animation:fade .25s ease;

}

.slide-left{

    animation:slide .25s ease;

}

/*==================================================
    MOBILE
==================================================*/

@media(max-width:600px){

.loading-content{

    width:90%;

    min-width:auto;

}

.success-icon{

    width:70px;

    height:70px;

    font-size:34px;

}

.countdown{

    font-size:56px;

}

.toast{

    left:15px;

    right:15px;

    bottom:15px;

}

.wifi-item{

    flex-direction:column;

    align-items:flex-start;

}

}

/*==================================================
    PRINT
==================================================*/

@media print{

body{

display:none;

}

}
)HSHCSS";
    _server.send(200, "text/css", css);
}

void Provisioning::HandleScript()
{
    const char* js = R"HSHJS(
const pages = [
    \"welcomePage\",
    \"devicePage\",
    \"wifiPage\",
    \"serverPage\",
    \"reviewPage\",
    \"successPage\"
];

let currentPage = 0;

const configuration = {
    hardwareId: \"\",
    authKey: \"\",
    wifiSsid: \"\",
    wifiPassword: \"\",
    backendUrl: \"\"
};

document.addEventListener(\"DOMContentLoaded\", () => {

    showPage(0);

    initializeValidation();

    initializeButtons();

    loadNetworks();

});

function showPage(index)
{
    currentPage = index;

    pages.forEach(id =>
    {
        const page = document.getElementById(id);

        if(page)
            page.style.display = \"none\";
    });

    const current = document.getElementById(pages[index]);

    if(current)
        current.style.display = \"block\";

    updateStepper();
}

function nextPage()
{
    if(currentPage < pages.length - 1)
    {
        showPage(currentPage + 1);
    }
}

function previousPage()
{
    if(currentPage > 0)
    {
        showPage(currentPage - 1);
    }
}

function initializeButtons()
{
    document.querySelectorAll(\"[data-next]\").forEach(button =>
    {
        button.addEventListener(\"click\", nextPage);
    });

    document.querySelectorAll(\"[data-back]\").forEach(button =>
    {
        button.addEventListener(\"click\", previousPage);
    });
}

function updateStepper()
{
    const steps = document.querySelectorAll(\".step\");

    steps.forEach((step,index)=>
    {
        step.classList.remove(\"active\");
        step.classList.remove(\"completed\");

        if(index < currentPage)
            step.classList.add(\"completed\");

        else if(index === currentPage)
            step.classList.add(\"active\");
    });
}

function initializeValidation()
{
    const inputs = document.querySelectorAll(\"input\");

    inputs.forEach(input =>
    {
        input.setAttribute(\"autocomplete\",\"off\");
        input.setAttribute(\"spellcheck\",\"false\");
        input.setAttribute(\"autocorrect\",\"off\");
        input.setAttribute(\"autocapitalize\",\"off\");

        input.addEventListener(\"blur\",()=>{

            validateField(input);

        });

        input.addEventListener(\"keyup\",(e)=>{

            if(e.key===\"Enter\")
                validateField(input);

        });

    });
}

function validateField(input)
{
    input.classList.remove(\"valid\");
    input.classList.remove(\"invalid\");

    if(input.value.trim()===\"\")
    {
        input.classList.add(\"invalid\");
        return false;
    }

    input.classList.add(\"valid\");

    return true;
}

async function loadNetworks()
{
    const container = document.getElementById(\"wifiNetworks\");

    if(!container)
        return;

    container.innerHTML = \"Scanning...\";

    try
    {
        const response = await fetch(\"/api/scan\");

        const networks = await response.json();

        container.innerHTML = \"\";

        networks.forEach(network=>{

            const item = document.createElement(\"div\");

            item.className = \"wifi-item\";

            item.innerHTML = `
                <input
                    type=\"radio\"
                    name=\"ssid\"
                    value=\"${network.ssid}\"
                >

                <span>${network.ssid}</span>

                <small>${network.rssi} dBm</small>
            `;

            item.onclick = ()=>{

                document.getElementById(\"wifiSsid\").value =
                    network.ssid;

            };

            container.appendChild(item);

        });

    }
    catch
    {
        container.innerHTML =
            \"Unable to scan WiFi.\";
    }
}

async function validateDevice()
{
    configuration.hardwareId =
        document.getElementById(\"hardwareId\").value;

    configuration.authKey =
        document.getElementById(\"authKey\").value;

    if(configuration.hardwareId===\"\" ||
       configuration.authKey===\"\")
    {
        return false;
    }

    showLoading(\"Validating device...\");

    try
    {
        const response = await fetch(\"/api/device\",{

            method:\"POST\",

            headers:{
                \"Content-Type\":\"application/json\"
            },

            body:JSON.stringify(configuration)

        });

        hideLoading();

        return response.ok;
    }
    catch
    {
        hideLoading();

        return false;
    }
}

async function validateWiFi()
{
    configuration.wifiSsid =
        document.getElementById(\"wifiSsid\").value;

    configuration.wifiPassword =
        document.getElementById(\"wifiPassword\").value;

    showLoading(\"Connecting to WiFi...\");

    try
    {
        const response = await fetch(\"/api/wifi\",{

            method:\"POST\",

            headers:{
                \"Content-Type\":\"application/json\"
            },

            body:JSON.stringify(configuration)

        });

        hideLoading();

        return response.ok;
    }
    catch
    {
        hideLoading();

        return false;
    }
}

async function validateServer()
{
    configuration.backendUrl =
        document.getElementById(\"backendUrl\").value;

    showLoading(\"Contacting server...\");

    try
    {
        const response = await fetch(\"/api/server\",{

            method:\"POST\",

            headers:{
                \"Content-Type\":\"application/json\"
            },

            body:JSON.stringify(configuration)

        });

        hideLoading();

        return response.ok;
    }
    catch
    {
        hideLoading();

        return false;
    }
}

function populateReview()
{
    document.getElementById(\"reviewHardware\").innerText =
        configuration.hardwareId;

    document.getElementById(\"reviewWifi\").innerText =
        configuration.wifiSsid;

    document.getElementById(\"reviewServer\").innerText =
        configuration.backendUrl;
}

async function saveConfiguration()
{
    showLoading(\"Saving configuration...\");

    try
    {
        const response = await fetch(\"/api/save\",{

            method:\"POST\",

            headers:{
                \"Content-Type\":\"application/json\"
            },

            body:JSON.stringify(configuration)

        });

        hideLoading();

        if(!response.ok)
            return;

        showPage(5);

        startCountdown();
    }
    catch
    {
        hideLoading();
    }
}

function startCountdown()
{
    let seconds = 5;

    const label =
        document.getElementById(\"countdown\");

    label.innerText = seconds;

    const timer = setInterval(()=>{

        seconds--;

        label.innerText = seconds;

        if(seconds===0)
        {
            clearInterval(timer);
        }

    },1000);
}

function showLoading(message)
{
    const overlay =
        document.getElementById(\"loadingOverlay\");

    const text =
        document.getElementById(\"loadingText\");

    if(text)
        text.innerText = message;

    if(overlay)
        overlay.style.display = \"flex\";
}

function hideLoading()
{
    const overlay =
        document.getElementById(\"loadingOverlay\");

    if(overlay)
        overlay.style.display = \"none\";
}
)HSHJS";
    _server.send(200, "application/javascript", js);
}

void Provisioning::HandleScan()
{
    int count = WiFi.scanNetworks(false, true);

    JsonDocument response;
    JsonArray networks = response.to<JsonArray>();

    for (int i = 0; i < count; ++i)
    {
        JsonObject network = networks.add<JsonObject>();
        network["ssid"] = WiFi.SSID(i);
        network["rssi"] = WiFi.RSSI(i);
        network["secure"] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
    }

    String body;
    serializeJson(response, body);
    _server.send(200, "application/json", body);

    WiFi.scanDelete();
}

void Provisioning::HandleNotFound()
{
    // Captive portals commonly request /generate_204, /hotspot-detect.html,
    // /connecttest.txt, etc. Returning the setup page makes those requests
    // useful instead of producing a dead-end 404.
    if (_server.method() == HTTP_GET)
    {
        HandleRoot();
        return;
    }

    _server.send(404, "text/plain", "Not Found");
}

void Provisioning::HandleProvision()
{
    JsonDocument request;

    auto error =
        deserializeJson(
            request,
            _server.arg("plain"));

    if(error)
    {
        SendResponse(
            false,
            "Invalid request");

        return;
    }

    String message;

    bool success =
        Provision(
            request,
            message);

    SendResponse(
        success,
        message);
}
bool Provisioning::Provision(
    const JsonDocument& request,
    String& message)
{
    auto& config =
        _configuration.GetConfiguration();

    config.HardwareId =
        request["hardwareId"].as<String>();

    config.AuthKey =
        request["authKey"].as<String>();

    config.WifiSSID =
        request["wifiSsid"].as<String>();

    config.WifiPassword =
        request["wifiPassword"].as<String>();

    config.BackendUrl =
        request["backendUrl"].as<String>();

    WiFi.disconnect(true);

    WiFi.begin(
        config.WifiSSID.c_str(),
        config.WifiPassword.c_str());

    unsigned long start =
        millis();

    while(WiFi.status()!=WL_CONNECTED)
    {
        if(millis()-start>15000)
        {
            message =
                "Unable to connect to WiFi";

            return false;
        }

        delay(500);
    }

    if(!DownloadConfiguration(
        config.BackendUrl,
        config.HardwareId,
        config.AuthKey,
        config,
        message))
    {
        return false;
    }

    config.IsProvisioned = true;

    _configuration.Save();

    _running = false;

    message =
        "Provision successful";

    return true;
}
bool Provisioning::RefreshMqttConfiguration()
{
    auto& config = _configuration.GetConfiguration();

    if (config.BackendUrl.isEmpty() ||
        config.HardwareId.isEmpty() ||
        config.AuthKey.isEmpty())
    {
        return false;
    }

    String message;

    return DownloadConfiguration(
        config.BackendUrl,
        config.HardwareId,
        config.AuthKey,
        config,
        message);
}

bool Provisioning::DownloadConfiguration(
    const String& backendUrl,
    const String& hardwareId,
    const String& authKey,
    Config& config,
    String& message)
{
    HTTPClient http;

    String url = backendUrl;
    while (url.endsWith("/"))
        url.remove(url.length() - 1);

    // Exact backend route supplied for MQTT bootstrap. If the user stored
    // only the server origin, add the Home_Security route segment.
    if (url.indexOf("/Home_Security") < 0)
        url += "/Home_Security";

    url += "/MicroController/GetMicroControllerMQTTConfig";

    auto encode = [](const String& value) -> String
    {
        const char hex[] = "0123456789ABCDEF";
        String out;

        for (size_t i = 0; i < value.length(); ++i)
        {
            const uint8_t c = static_cast<uint8_t>(value[i]);

            if ((c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                c == '-' || c == '_' || c == '.' || c == '~')
            {
                out += static_cast<char>(c);
            }
            else
            {
                out += '%';
                out += hex[(c >> 4) & 0x0F];
                out += hex[c & 0x0F];
            }
        }

        return out;
    };

    url += "?hardWareId=";
    url += encode(hardwareId);
    url += "&AuthKey=";
    url += encode(authKey);

    Serial.print("[PROVISION] MQTT config GET: ");
    Serial.println(url);

    WiFiClientSecure secureClient;
    WiFiClient plainClient;

    bool begun = false;

    if (url.startsWith("https://"))
    {
        // The controller must work with development tunnels whose public
        // certificate is not installed on the ESP32. Production deployments
        // should replace setInsecure() with a pinned/root CA certificate.
        secureClient.setInsecure();
        begun = http.begin(secureClient, url);
    }
    else
    {
        begun = http.begin(plainClient, url);
    }

    if (!begun)
    {
        message = "Unable to initialize backend HTTP client.";
        return false;
    }

    http.addHeader("Accept", "*/*");

    const int statusCode = http.GET();

    if (statusCode <= 0)
    {
        message = http.errorToString(statusCode);
        http.end();
        return false;
    }

    const String responseBody = http.getString();
    http.end();

    if (statusCode != HTTP_CODE_OK)
    {
        message = "MQTT configuration request failed.";
        Serial.printf(
            "[PROVISION] MQTT config HTTP status: %d\n",
            statusCode);
        return false;
    }

    JsonDocument response;

    const auto error =
        deserializeJson(response, responseBody);

    if (error)
    {
        message = "Invalid MQTT configuration response.";
        return false;
    }

    const bool status =
        response["status"] | false;

    if (!status)
    {
        message =
            response["message"] |
            "Backend rejected MQTT configuration request.";
        return false;
    }

    JsonObject data = response["data"];

    if (data.isNull())
    {
        message = "MQTT configuration data is missing.";
        return false;
    }

    config.MqttHost =
        data["mqttHost"] | "";

    if (config.MqttHost.isEmpty())
        config.MqttHost = data["MqttHost"] | "";

    if (config.MqttHost.isEmpty())
        config.MqttHost = data["host"] | "";

    config.MqttPort =
        data["mqttPort"] | 0;

    if (config.MqttPort == 0)
        config.MqttPort = data["MqttPort"] | 0;

    if (config.MqttPort == 0)
        config.MqttPort = data["port"] | 1883;

    config.MqttUsername =
        data["mqttUsername"] | "";

    if (config.MqttUsername.isEmpty())
        config.MqttUsername = data["MqttUsername"] | "";

    if (config.MqttUsername.isEmpty())
        config.MqttUsername = data["username"] | "";

    config.MqttPassword =
        data["mqttPassword"] | "";

    if (config.MqttPassword.isEmpty())
        config.MqttPassword = data["MqttPassword"] | "";

    if (config.MqttPassword.isEmpty())
        config.MqttPassword = data["password"] | "";

    if (config.MqttHost.isEmpty())
    {
        message = "Backend returned an empty MQTT host.";
        return false;
    }

    _configuration.Save();

    message =
        response["message"] |
        "MQTT configuration refreshed.";

    return true;
}

void Provisioning::SendResponse(
    bool status,
    const String& message)
{
    JsonDocument json;

    json["status"] = status;

    json["message"] = message;

    String response;

    serializeJson(
        json,
        response);

    _server.send(
        200,
        "application/json",
        response);
}