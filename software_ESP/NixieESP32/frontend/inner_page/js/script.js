var xmlHttp = CreateRequest();


// window.onload = function() {
//     process();
// }


function CreateRequest() {
    var Request = false;

    if (window.XMLHttpRequest) {
        //Gecko-совместимые браузеры, Safari, Konqueror
        Request = new XMLHttpRequest();
    }
    else if (window.ActiveXObject) {
        //Internet explorer
        try {
            Request = new ActiveXObject("Microsoft.XMLHTTP");
        }
        catch (CatchException) {
            Request = new ActiveXObject("Msxml2.XMLHTTP");
        }
    }

    if (!Request) {
        alert("Невозможно создать XMLHttpRequest");
    }

    return Request;
}


function response() {
    if (xmlHttp.readyState == 4) {
        if (xmlHttp.status == 200) {
            xmlResponse = xmlHttp.responseXML;
            var xmldoc = xmlResponse.getElementsByTagName('data');
            message = xmldoc[0].firstChild.nodeValue;
            document.getElementById('time').innerHTML = message;
            console.log(message);
        } else {
            console.log("Not 200");
        }
    } else {
        console.log("not loaded");
    }
}

function process() {
    xmlHttp.open('PUT', 'data.xml', true);
    xmlHttp.onreadystatechange = response;
    xmlHttp.send(null);
    setTimeout('process()', 100);
}

function press(a) {
    xmlHttp.open('POST', '/postform/', true); // Открываем асинхронное соединение
    xmlHttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded'); // Отправляем кодировку
    xmlHttp.send("a=" + encodeURIComponent(a)); // Отправляем POST-запрос
}

function pr() {
    var switchID = document.getElementsByClassName('switch-btn')[0];
    switchID.classList.toggle('switch-on');
    //console.log("click");
    var requ;
    if (switchID.classList.contains("switch-on")) {
        requ = "on";
        console.log("on");
        document.getElementById('state').innerHTML = "ON";
    } else {
        requ = "off";
        console.log("off");
        document.getElementById('state').innerHTML = "OFF";
    }
    press(requ);
}
//document.getElementsByClassName('switch-btn')[0].onclick = pr;
