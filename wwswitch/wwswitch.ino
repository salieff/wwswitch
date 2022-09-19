#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#include "debounced_button.h"
#include "webserver.h"
#include "wwswitchstatistics.h"

#define LEFT_RELAY_PIN_NUMBER 4
#define RIGHT_RELAY_PIN_NUMBER 14
#define GREEN_LED_PIN_NUMBER 13

WiFiManager wifiManager;
WWSwitchWebServer webServer;
WWSwitchStatistics statistics;

static constexpr bool TOGGLE = true;
static constexpr bool COMET = true;
static constexpr bool NO_TOGGLE = false;
static constexpr bool NO_COMET = false;

bool waitingForWifi = true;


String toggleSwitch(uint8_t pin, bool toggle = TOGGLE)
{
    auto state = digitalRead(pin);

    if (toggle)
    {
        state = !state;
        digitalWrite(pin, state);
    }

    return state == HIGH ? "on" : "off";;
}

String toggleSwitchByName(const String &name, bool toggle = TOGGLE, bool comet = COMET)
{
    auto state = toggleSwitch(name == "right" ? RIGHT_RELAY_PIN_NUMBER : LEFT_RELAY_PIN_NUMBER, toggle);

    if (comet)
        webServer.postCometEvent(name, state);

    return state;
}

String switchStateByName(const String &name, bool comet = NO_COMET)
{
    if (name == "all")
    {
        String jsonStr = "{\"events\": [";

        jsonStr += "{\"event\": \"left\", \"value\": \"";
        jsonStr += toggleSwitch(LEFT_RELAY_PIN_NUMBER, NO_TOGGLE);
        jsonStr += "\"},";

        jsonStr += "{\"event\": \"right\", \"value\": \"";
        jsonStr += toggleSwitch(RIGHT_RELAY_PIN_NUMBER, NO_TOGGLE);
        jsonStr += "\"}";

        jsonStr += "]}";

        return jsonStr;
    }

    return toggleSwitchByName(name, NO_TOGGLE, comet);
}

void doReboot(AsyncWebServerRequest *request = nullptr)
{
    webServer.postCometEvent("debug", "Going to reboot");

    if (request)
    {
        request->send(200, "text/plain", "Going to reboot\r\n");
        request->client()->stop();
    }

    ESP.restart();
}

void doReset(AsyncWebServerRequest *request = nullptr)
{
    webServer.postCometEvent("debug", "Going to reset & restart");

    if (request)
    {
        request->send(200, "text/plain", "Going to reset & restart\r\n");
        request->client()->stop();
    }

    wifiManager.resetSettings();
    ESP.restart();
}

WWSwitchDebouncedButton leftButton(
        LEFT_BUTTON_PIN_NUMBER,
        []() {
            // Press callback
            Serial.printf("[%lu] Hardware left button press\n", millis());
            toggleSwitchByName("left");
        },
        []() {
            // 10 sec hold callback
            Serial.printf("[%lu] Hardware left button hold\n", millis());
            doReboot();
        }
);

WWSwitchDebouncedButton rightButton(
        RIGHT_BUTTON_PIN_NUMBER,
        []() {
            // Press callback
            Serial.printf("[%lu] Hardware right button press\n", millis());
            toggleSwitchByName("right");
        },
        []() {
            // 10 sec hold callback
            Serial.printf("[%lu] Hardware right button hold\n", millis());
            doReset();
        }
);

void setupWebServer()
{
    webServer.setup();

    webServer.on("/status", [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", switchStateByName(request->arg("switch")));
    });

    webServer.on("/toggle", [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", toggleSwitchByName(request->arg("switch")));
    });

    webServer.on("/reboot", doReboot);
    webServer.on("/reset", doReset);

    webServer.on("/systeminfo", [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", statistics.getStatistics());
    });

    // Посылаем off/off после перезагрузки, если во время перезагрузки был активный comet-клиент
    switchStateByName("left", COMET);
    switchStateByName("right", COMET);
}

void setup()
{
    Serial.begin(74880);
    Serial.flush();
    Serial.printf("\r\nWWSwitch Setup\r\n");
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.flush();

    pinMode(GREEN_LED_PIN_NUMBER, OUTPUT);
    digitalWrite(GREEN_LED_PIN_NUMBER, HIGH);

    pinMode(LEFT_RELAY_PIN_NUMBER, OUTPUT);
    digitalWrite(LEFT_RELAY_PIN_NUMBER, LOW);

    pinMode(RIGHT_RELAY_PIN_NUMBER, OUTPUT);
    digitalWrite(RIGHT_RELAY_PIN_NUMBER, LOW);

    leftButton.setup();
    rightButton.setup();

    wifiManager.setHostname("wwswitch");
    wifiManager.setConfigPortalBlocking(false);
    // wm.setConfigPortalTimeout(60);
    // wm.resetSettings();

    if (wifiManager.autoConnect("WWSwitch"))
    {
        waitingForWifi = false;
        Serial.println("connected...yeey :)");
        digitalWrite(GREEN_LED_PIN_NUMBER, LOW);
        // setWifiCallbacks();
        setupWebServer();
    }
    else
    {
        Serial.println("Configportal running");
    }
}

void loop() {
    leftButton.work();
    rightButton.work();

    if (waitingForWifi && wifiManager.process())
    {
        // ESP.restart(); // Без этого не выставляется правильный hostname
        waitingForWifi = false;
        Serial.println("connected...yeey :)");
        digitalWrite(GREEN_LED_PIN_NUMBER, LOW);
        // setWifiCallbacks();
        setupWebServer();
    }

    webServer.work();
    statistics.work();
}
