#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>

ESP8266WebServer server(80);
WiFiManager wm;

void driveMotor(int l, int r) {
    Serial.printf("JOY:%d:%d\n", l, r);
}

void initWifi() {
    // ONLY RUN that sh once unless you changing wifi
    //   wm.resetSettings();
    if (!wm.autoConnect("RC-Car-Network")) {
    Serial.println("Couldn't connect: Restarting...");
    ESP.restart();
    }

    if (MDNS.begin("ruckus")) {
    Serial.println("Open at: http://ruckus.local"); // RUCKUS!!!
    }

    server.on("/", []() {
    String html = R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1">
            <title>RC Car Web Server</title>
            <script src="https://cdn.jsdelivr.net/npm/nipplejs@0.10.2/dist/nipplejs.min.js"></script>
            <style>
                @import url(https://fonts.googleapis.com/css?family=VT323);
                html, body {
                    height: 100%;
                    margin: 0;
                    box-sizing: border-box;
                }
                *, *::before, *::after {
                    box-sizing: inherit;
                }
                body {
                    padding:50px;
                    font:14px/1.5 'VT323', "Helvetica Neue", Helvetica, Arial, sans-serif;
                    font-weight:300;
                    overflow-y: hidden;
                }
                h1, h2, h3 {
                    font-size: 2rem;
                }

                h1 {
                    display: block;
                    margin-block-start: 0.67em; /* Like 67 */
                    margin-block-end: 0.67em;
                    margin-inline-start: 0;
                    margin-inline-end: 0;
                    font-weight: bold;
                    unicode-bidi: isolate;
                }

                li {
                    display: list-item;
                    unicode-bidi: isolate;
                }

                ul {
                    list-style-type: disc;
                }
                #zone_joystick {
                    position: relative;
                    background: silver;
                    box-sizing: content-box;
                    min-height: calc(100vh - 100px);
                    border-radius: 12px;
                }
                #debug {
                    position: absolute;
                    top: 0;
                    right: 0;
                    padding: 5px 0 5px 0;
                    width: 300px;
                    height: 450px;
                    box-sizing:border-box;
                    color: white;
                }
                #debug ul {
                    list-style: none;
                }
                #debug > ul {
                    padding-left: 0;
                }
                #debug .data {
                    color: #333333;
                    font-weight: 800;
                }
                .zone.static {
                    display: block;
                    background: rgba(255, 0, 0, 0.1);
                    border-radius: 12px;
                }
                .zone {
                    display: none;
                    position: absolute;
                    width: 100%;
                    height: 100%;
                    left: 0;
                    touch-action: none;
                }
                .zone > h1 {
                    position: absolute;
                    padding: 10px 10px;
                    margin: 0;
                    color: white;
                    right: 5px;
                    bottom: 0;
                }
            </style>
        </head>
        <body>
            <div id="zone_joystick">
                <div id="debug">
                    <ul>
                    <li class="force">force : <span class="data"></span></li>
                    <li class="pressure">pressure : <span class="data"></span></li>
                    <li class="distance">distance : <span class="data"></span></li>
                    <li class="angle">
                        angle :
                        <ul>
                        <li class="radian">radian : <span class="data"></span></li>
                        <li class="degree">degree : <span class="data"></span></li>
                        </ul>
                    </li>
                    <li class="direction">
                        direction :
                        <ul>
                        <li class="x">x : <span class="data"></span></li>
                        <li class="y">y : <span class="data"></span></li>
                        <li class="angle">angle : <span class="data"></span></li>
                        </ul>
                    </li>
                    </ul>
                </div>
                <div id="zone_static" class="zone static"><h1>ruckus v1.0</h1></div>
            </div>
            <script>
                const url = "http://192.168.1.13";
                let joystick = null;
                let lastRequest = 0;
                let stopTimer = null;
                let lastMoveTime = 0;
                const STOP_DELAY = 200;
                const SEND_MIN_INTERVAL = 30;

                var s = function(sel) {
                    return document.querySelector(sel);
                }
                var sId = function(sel) {
                    return document.getElementById(sel)
                }

                var elDebug = sId('debug');
                var els = {
                    force: elDebug.querySelector('.force .data'),
                    pressure: elDebug.querySelector('.pressure .data'),
                    distance: elDebug.querySelector('.distance .data'),
                    angle: {
                        radian: elDebug.querySelector('.angle .radian .data'),
                        degree: elDebug.querySelector('.angle .degree .data')
                    },
                    direction: {
                        x: elDebug.querySelector('.direction .x .data'),
                        y: elDebug.querySelector('.direction .y .data'),
                        angle: elDebug.querySelector('.direction .angle .data')
                    }
                };

                const send = (l, r) => {
                    const now = Date.now();
                    if (now - lastRequest < SEND_MIN_INTERVAL) return;
                    lastRequest = now;

                    fetch(`${url}/api/drive`, {
                        method: "POST",
                        headers: {"Content-Type": "application/json"},
                        body: JSON.stringify({
                            l: Math.round(l),
                            r: Math.round(r)}),
                        keepalive: true
                    }).catch(() => {/* if ther are errors im quitting */ });
                };

                function autoStop() {
                    if (stopTimer) clearTimeout(stopTimer);
                    stopTimer = setTimeout(() => {
                        send(0, 0);
                        stopTimer = null;
                    }, STOP_DELAY);
                }

                function cancelAutoStop() {
                    if (stopTimer) {
                        clearTimeout(stopTimer);
                        stopTimer = null;
                    }
                }

                function drive(force, angle) {
                    const throttle = Math.sin(angle);
                    const steering = Math.cos(angle) * 0.9;

                    let left = throttle + steering;
                    let right = throttle - steering;

                    const max = Math.max(Math.abs(left), Math.abs(right), 1);
                    
                    left = (left / max) * force * 127;
                    right = (right / max) * force * 127;

                    return {
                        left: Math.round(left),
                        right: Math.round(right)
                    };
                }

                function bind() {
                    joystick.on('start', function() {
                        cancelAutoStop();
                    })
                    joystick.on('move', function(evt, data) {
                        debug(data);
                        const MARGIN = 0.1;
                        let force = data.force < MARGIN ? 0 : data.force;
                        let angle = data.angle.radian;
                        const { left, right } = drive(force, angle);
                        send(left, right);
                        autoStop();
                        lastMoveTime = Date.now();
                    }).on('end', function() {
                        cancelAutoStop();
                        send(0, 0);
                    });
                }

                function getJoystickSize() {
                    if (window.innerWidth <= 480) return 75;
                    else if (window.innerWidth <= 1000) return 100;
                    return 250;
                }

                function getJoystickPos() {
                    if (window.innerWidth <= 480) return { left: '50%', bottom: '45%' };
                    return { left: '25%', bottom: '50%' };
                }

                function createJoystick(evt) {
                    if (joystick) {
                        joystick.destroy();
                        joystick = null;
                    }

                    const zoneEl = document.getElementById('zone_static');
                    if (!zoneEl) return;

                    joystick = nipplejs.create({
                        zone: zoneEl,
                        mode: 'static',
                        position: getJoystickPos(),
                        color: 'black',
                        size: getJoystickSize(),
                        shape: 'circle',
                    });
                    bind();
                    
                }
                
                createJoystick('static');

                function immediateStop() {
                cancelAutoStop();
                send(0, 0);
                }

                window.addEventListener('mouseup', immediateStop, {passive: true});
                window.addEventListener('touchend', immediateStop, {passive: true});
                window.addEventListener('pointerup', immediateStop, {passive: true});
                window.addEventListener('pointercancel', immediateStop, {passive: true});
                window.addEventListener('touchcancel', immediateStop, {passive: true});

                document.addEventListener('visibilitychange', function(){
                    if (document.hidden) immediateStop();
                });

                window.addEventListener('resize', function() {
                    setTimeout(function () {
                        createJoystick('static');
                    }, 120);
                });

                window.addEventListener('orientationchange', function() {
                    setTimeout(function () {
                        createJoystick('static');
                    }, 120);
                });

                function debug(obj) {
                    function parseObj(sub, el) {
                        for (var i in sub) {
                            if (typeof sub[i] === 'object' && el) {
                                parseObj(sub[i], el[i]);
                            } else if (el && el[i]) {
                                el[i].innerHTML = sub[i];
                            }
                        }
                    }
                    setTimeout(function() {
                        parseObj(obj, els);
                    }, 0);
                }

                setInterval(function() {
                if (lastMoveTime === 0) return;
                if (Date.now() - lastMoveTime > 350) {
                    lastMoveTime = 0;
                    send(0, 0);
                }
                }, 150);
            </script>
        </body>
        </html>
    )rawliteral";
    html.replace("const url = \"http://192.168.1.13\";", 
                "const url = \"http://\" + location.hostname;");
    server.send(200, "text/html", html);
    });

    server.on("/api/drive", HTTP_POST, []() {
        if (!server.hasArg("plain")) {
            server.send(400, "text/plain", "Missing body");
            return;
        }

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, server.arg("plain"));
        if (err) {
            server.send(400, "text/plain", "Bad JSON");
            return;
        }

        int l = doc["l"] | 0;
        int r = doc["r"] | 0;

        driveMotor(l, r);
        server.send(200, "application/json", "{\"ok\":true}");
    });
    server.begin();
    Serial.println("Wemos is ONLINE!");
}

void setup() {
    Serial.begin(115200);
    delay(500);
    initWifi();
}

void loop() {
    server.handleClient();
    MDNS.update();
}