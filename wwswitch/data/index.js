function ParseCometEvent(evt, showDebug = true)
{
    let el = document.getElementById(evt.event);
    if (el)
        el.src = el.dataset["src" + evt.value];

    if (!showDebug)
        return;

    let dbg = document.getElementById("debug-textarea");
    if (dbg)
    {
        dbg.innerHTML += "{event: " + evt.event + ", value: " + evt.value + "}\n";
        dbg.scrollTop = dbg.scrollHeight;
    }
}

function GetChain(url, errorFunction = null, successFunction = null, nextFunction = null)
{
    let xh = new XMLHttpRequest();
    xh.onreadystatechange = function() {
        if (xh.readyState != 4)
            return;

        if (xh.status != 200)
        {
            if (!errorFunction)
                alert("GET error: " + xh.responseText + " (" + xh.status +")");

            errorFunction(xh);
        }
        else
        {
            if (successFunction)
                successFunction(xh);
        }

        if (nextFunction)
            nextFunction();
    };

    xh.open("GET", url, true);
    xh.send();
}

function SubscribeToComet()
{
    GetChain("subscribe",
        function(xh) {
            alert("SubscribeToComet error: " + xh.responseText + " (" + xh.status +")");
            RequestSwitch("status", "all", SubscribeToComet);
        },
        function(xh) {
            let resp = JSON.parse(xh.responseText);
            resp.events.forEach(function(evt) { ParseCometEvent(evt); });
            SubscribeToComet();
        });
}

function RequestSwitch(action, name, nextFunction = null)
{
    GetChain(action + "?switch=" + name,
        function(xh) {
            alert("RequestSwitch(" + action + ", " + name + ") error: " + xh.responseText + " (" + xh.status +")");
        },
        function(xh) {
            if (name == "all")
            {
                let resp = JSON.parse(xh.responseText);
                resp.events.forEach(function(evt) { ParseCometEvent(evt, false); });
            }
            else
            {
                ParseCometEvent({"event": name, "value": xh.responseText}, false);
            }
        },
        nextFunction
    );
}

function BodyMain()
{
    Array.from(document.getElementsByClassName("button-img")).forEach(function(buttonImg) {
        buttonImg.onclick = function() {
            RequestSwitch("toggle", buttonImg.id);
        };
    });

    document.getElementsByName("systeminfo")[0].onclick = function() {
        GetChain("systeminfo",
            function(xh) { alert("GetSystemInfo error: " + xh.responseText + " (" + xh.status +")"); },
            function(xh) { ParseCometEvent({"event": "systeminfo", "value": xh.responseText}); }
        );
    };

    RequestSwitch("status", "all", SubscribeToComet);
}
