window.onload = function () {
    greatView().then(function(done) {
        getAPList();
        process();
    });
}

var animatedCount = 0;

function animatedText(delay) {
    var elem = document.getElementsByClassName('animate');
    console.log(elem);
    for (var i = 0; i < elem.length; i++) {
        console.log(elem[i]);
        setTimeout(function (item) {
            item.hidden = false;
            animatedCount++;
        }, i * 1000, elem[i]);
    }
    return elem.length;
}

function greatView() {
    var delay = 1200;
    var count = animatedText(delay);
    var promise = new Promise(function (resolve, reject) {
        window.setTimeout(function () {
            resolve('done!');
        }, delay*count);
    });
    return promise;
}

function process() {
    document.getElementById("connect").onclick = function () {
        sendSelectedAP();
    }
    document.getElementById("link-box").onclick = function () {
        window.location.href = 'https://yandex.ru/time/';
    }
    document.getElementById("reload-AP").onclick = function () {
        getAPList();
    }
}

function getAPList() {
    makeGETRequest('../APlist.json', decodeAPlistJSON, function (request, path, func) {
        hiddenForm();
        waitGET(request, path, func);
    });
}

var repeatRequest;
var existRequest;

function waitGET(request, path, func) {
    if (request.status == 202 && request.readyState == 4) {
        repeatRequest = setInterval(function () {
            if (!existRequest) {
                existRequest = true;
                console.log("try to get");
                makeGETRequest(path, function (secondary_request) {
                    repeatRequest = clearInterval(repeatRequest);
                    func(secondary_request);
                }, function () { existRequest = false });
            }
        }, 1500);
    }
}

function hiddenForm() {
    console.log("hidden");
    document.getElementById("input-forms").hidden = true;
    document.getElementById("floatingBarsG").hidden = false;
}

function showForm() {
    document.getElementById("input-forms").hidden = false;
    document.getElementById("floatingBarsG").hidden = true;
}

function decodeAPlistJSON(handler) {
    showForm();
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

function waitConnecting(){
    document.getElementById("head-text").hidden = true;
    document.getElementById("input-forms").hidden = true;
    document.getElementById("load-text").hidden = false;
    document.getElementById("floatingBarsG").hidden = false;
}

function sendSelectedAP() {
    var selectList = document.getElementById("APlist");
    var passForm = document.getElementById("password");
    var sendAPjson = { "name": selectList.options[selectList.selectedIndex].text, "password": passForm.value };
    console.log(sendAPjson);
    makePOSTRequest("/selectedAp", sendAPjson, function () { }, waitConnecting);
    makeGETRequest('/selectedAp', showLinkIP, function (request, path, func) {
        waitGET(request, path, func);
    });
    //alert(sendAPjson["name"] + " " + sendAPjson["password"]);
}

function showLinkIP(handler) {
    var respJSON = JSON.parse(handler.responseText);
    var IPlink = respJSON["ip"];
    document.getElementById("link-box").onclick = function () {
        var link = 'https://' + IPlink;
        copyToClipboard(link);
        window.open(link, '_system');
        makeGETRequest('/redirected', function () { });
        window.location.href = link;
    }
    document.getElementById("input-forms").hidden = true;
    document.getElementById("load-text").hidden = true;
    document.getElementById("floatingBarsG").hidden = true;
    document.getElementById("connected-text").hidden = false;
    document.getElementById("IP-link").hidden = false;
}

function makeGETRequest(path, func, badFunc = function () { ; }) {
    var xmlHttp = CreateRequest();
    xmlHttp.open('GET', path, true);
    xmlHttp.onreadystatechange = function () {
        if (this.readyState == 4) {
            if (this.status == 200) {
                func(this);
            } else {
                console.log("Not 200, ", this.status);
                badFunc(this, path, func);
            }
        } else {
            console.log("not loaded");
            badFunc(this, path, func);
        }
    };
    xmlHttp.send(null);
}

function makePOSTRequest(path, json, func = function () { }, badFunc = function () { }) {
    var xmlHttp = CreateRequest();
    xmlHttp.open('POST', path, true); // Открываем асинхронное соединение
    xmlHttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded;charset=UTF-8'); // Отправляем кодировку
    xmlHttp.onreadystatechange = function () {
        if (this.readyState == 4) {
            if (this.status == 200) {
                console.log("POST sended!");
                func(this);
            } else {
                console.log("Not 200, ", this.status);
                badFunc(this);
            }
        } else {
            console.log("send not loaded");
            badFunc(this);
        }
    };
    xmlHttp.send("data=" + JSON.stringify(json)); // Отправляем POST-запрос
}


function copyToClipboard(str) {
    var area = document.createElement('textarea');

    document.body.appendChild(area);
    area.value = str;
    area.select();
    document.execCommand("copy");
    document.body.removeChild(area);
}

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