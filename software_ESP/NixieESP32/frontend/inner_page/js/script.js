var xmlHttp = CreateRequest();


window.onload = function () {
    getAPList();
    process();
}

function process(){
    document.getElementById("connect").onclick = function(){
        sendSelectedAP();
    }
}


function getAPList() {
    makeGETRequest('../APlist.json', decodeJSONlist);
}

function decodeJSONlist(handler) {
    console.log("content-type: ", handler.getResponseHeader('Content-Type'));
    var respJSON = JSON.parse(handler.responseText);
    console.log(respJSON["name"]);
    var APlist = respJSON["name"];
    pasteInListOnPage(APlist);
}

function pasteInListOnPage(list) {
    if (list.length > 0) {
        document.getElementById("not-found").remove();
    }
    var selectList = document.getElementById("APlist");
    for (var i = 0; i < list.length; i++) {
        var option = document.createElement('option');
        option.innerHTML = list[i];
        option.value = list[i];
        selectList.appendChild(option);
    }
}

function sendSelectedAP(){
    var selectList = document.getElementById("APlist");
    var passForm = document.getElementById("password");
    var sendedAP = {"name": selectList.options[selectList.selectedIndex].text, "password": passForm.value};
    console.log(sendedAP);
    alert(sendedAP["name"] + " " + sendedAP["password"]);
}

function makeGETRequest(path, func) {
    xmlHttp.open('GET', path, true);
    xmlHttp.onreadystatechange = function () {
        if (this.readyState == 4) {
            if (this.status == 200) {
                func(this);
            } else {
                console.log("Not 200");
            }
        } else {
            console.log("not loaded");
        }
    };
    xmlHttp.send(null);
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