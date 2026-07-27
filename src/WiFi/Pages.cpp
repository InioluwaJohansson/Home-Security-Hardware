#include "Pages.h"

String Pages::SuccessPage()
{
    return R"rawliteral(
<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<title>Configuration Saved</title>

<style>

body{

    background:#0d1117;

    color:white;

    font-family:Arial;

    display:flex;

    justify-content:center;

    align-items:center;

    height:100vh;

}

.card{

    width:420px;

    text-align:center;

}

.count{

    font-size:72px;

    color:#4CAF50;

}

</style>

</head>

<body>

<div class="card">

<h2>Configuration Saved Successfully</h2>

<p>Your Home Security Controller is restarting.</p>

<div id="count" class="count">5</div>

</div>

<script>

let seconds = 5;

setInterval(()=>{

seconds--;

document.getElementById("count").innerHTML = seconds;

},1000);

</script>

</body>

</html>

)rawliteral";
}

String Pages::SuccessPage()
{