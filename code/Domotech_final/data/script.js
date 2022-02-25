$(document).ready(function(){
    $("#appliquerThermostat").click(function(){
        var valeur = $("#choixThermostat").val();
        $.post("changeThermostat",{
            valeurThermostat: valeur
        });
    });
});

function onButton(fonction) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "on/" + String(fonction), true);
    xhttp.send();
}
function offButton(fonction) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "off/" + String(fonction), true);
    xhttp.send();
}
function resetButton(fonction) {
    var xhttp = new XMLHttpRequest();
    xhttp.open("GET", "reset/" + String(fonction), true);
    xhttp.send();
}

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("boolean_chauffage").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "booleanChauffage", true);
    xhttp.send();
}, 2000
)

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("boolean_lumiere").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "booleanLumiere", true);
    xhttp.send();
}, 2000
)

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("boolean_fenetre").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "booleanFenetre", true);
    xhttp.send();
}, 2000
)

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("temperature").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "temperatureValue", true);
    xhttp.send();
}, 2000
)

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("lumiere_int").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "lumiereIntValue", true);
    xhttp.send();
}, 2000
)

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("lumiere_ext").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "lumiereExtValue", true);
    xhttp.send();
}, 2000
)

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("mq2").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "mq2Value", true);
    xhttp.send();
}, 2000
)

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("user_lumiere").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "userLumiereString", true);
    xhttp.send();
}, 2000
)

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("user_chauffage").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "userChauffageString", true);
    xhttp.send();
}, 2000
)

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("user_fenetre").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "userFenetreString", true);
    xhttp.send();
}, 2000
)

setInterval(function getData() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("thermostat_value").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "thermostatValue", true);
    xhttp.send();
}, 2000
)