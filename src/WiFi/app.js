const pages = [
    "welcomePage",
    "devicePage",
    "wifiPage",
    "serverPage",
    "reviewPage",
    "successPage"
];

let currentPage = 0;

const configuration = {
    hardwareId: "",
    authKey: "",
    wifiSsid: "",
    wifiPassword: "",
    backendUrl: ""
};

document.addEventListener("DOMContentLoaded", () => {

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
            page.style.display = "none";
    });

    const current = document.getElementById(pages[index]);

    if(current)
        current.style.display = "block";

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
    document.querySelectorAll("[data-next]").forEach(button =>
    {
        button.addEventListener("click", nextPage);
    });

    document.querySelectorAll("[data-back]").forEach(button =>
    {
        button.addEventListener("click", previousPage);
    });
}

function updateStepper()
{
    const steps = document.querySelectorAll(".step");

    steps.forEach((step,index)=>
    {
        step.classList.remove("active");
        step.classList.remove("completed");

        if(index < currentPage)
            step.classList.add("completed");

        else if(index === currentPage)
            step.classList.add("active");
    });
}

function initializeValidation()
{
    const inputs = document.querySelectorAll("input");

    inputs.forEach(input =>
    {
        input.setAttribute("autocomplete","off");
        input.setAttribute("spellcheck","false");
        input.setAttribute("autocorrect","off");
        input.setAttribute("autocapitalize","off");

        input.addEventListener("blur",()=>{

            validateField(input);

        });

        input.addEventListener("keyup",(e)=>{

            if(e.key==="Enter")
                validateField(input);

        });

    });
}

function validateField(input)
{
    input.classList.remove("valid");
    input.classList.remove("invalid");

    if(input.value.trim()==="")
    {
        input.classList.add("invalid");
        return false;
    }

    input.classList.add("valid");

    return true;
}

async function loadNetworks()
{
    const container = document.getElementById("wifiNetworks");

    if(!container)
        return;

    container.innerHTML = "Scanning...";

    try
    {
        const response = await fetch("/api/scan");

        const networks = await response.json();

        container.innerHTML = "";

        networks.forEach(network=>{

            const item = document.createElement("div");

            item.className = "wifi-item";

            item.innerHTML = `
                <input
                    type="radio"
                    name="ssid"
                    value="${network.ssid}"
                >

                <span>${network.ssid}</span>

                <small>${network.rssi} dBm</small>
            `;

            item.onclick = ()=>{

                document.getElementById("wifiSsid").value =
                    network.ssid;

            };

            container.appendChild(item);

        });

    }
    catch
    {
        container.innerHTML =
            "Unable to scan WiFi.";
    }
}

async function validateDevice()
{
    configuration.hardwareId =
        document.getElementById("hardwareId").value;

    configuration.authKey =
        document.getElementById("authKey").value;

    if(configuration.hardwareId==="" ||
       configuration.authKey==="")
    {
        return false;
    }

    showLoading("Validating device...");

    try
    {
        const response = await fetch("/api/device",{

            method:"POST",

            headers:{
                "Content-Type":"application/json"
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
        document.getElementById("wifiSsid").value;

    configuration.wifiPassword =
        document.getElementById("wifiPassword").value;

    showLoading("Connecting to WiFi...");

    try
    {
        const response = await fetch("/api/wifi",{

            method:"POST",

            headers:{
                "Content-Type":"application/json"
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
        document.getElementById("backendUrl").value;

    showLoading("Contacting server...");

    try
    {
        const response = await fetch("/api/server",{

            method:"POST",

            headers:{
                "Content-Type":"application/json"
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
    document.getElementById("reviewHardware").innerText =
        configuration.hardwareId;

    document.getElementById("reviewWifi").innerText =
        configuration.wifiSsid;

    document.getElementById("reviewServer").innerText =
        configuration.backendUrl;
}

async function saveConfiguration()
{
    showLoading("Saving configuration...");

    try
    {
        const response = await fetch("/api/save",{

            method:"POST",

            headers:{
                "Content-Type":"application/json"
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
        document.getElementById("countdown");

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
        document.getElementById("loadingOverlay");

    const text =
        document.getElementById("loadingText");

    if(text)
        text.innerText = message;

    if(overlay)
        overlay.style.display = "flex";
}

function hideLoading()
{
    const overlay =
        document.getElementById("loadingOverlay");

    if(overlay)
        overlay.style.display = "none";
}