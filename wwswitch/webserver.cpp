#include "webserver.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <FS.h>

FS *fsControl = &LittleFS;

static constexpr unsigned long COMET_EVENTS_LIFE_TIME = 3000; // 3 sec
static constexpr char COMET_CONTENT_START_STRING[] = "{\"events\": [";
static constexpr char COMET_CONTENT_END_STRING[] = "]}";


WWSwitchWebServer::WWSwitchWebServer(uint16_t port, uint16_t updaterPort)
    : m_webServer(port)
    , m_updaterServer(updaterPort)
    , m_cometEventsLastClearTimestamp(millis())
{
}

void WWSwitchWebServer::setupFS(void)
{
    fsControl->begin();
    auto root = fsControl->openDir("/");

    while(root.next())
        Serial.printf("FS File: %s, size: %u\n", root.fileName().c_str(), root.fileSize());

    Serial.println();
}

void WWSwitchWebServer::setupWebServer(void)
{
    m_updaterServerAdapter.setup(&m_updaterServer);
    m_updaterServer.begin();

    m_webServer.on("/subscribe", [this](AsyncWebServerRequest *request) {
        request->client()->setRxTimeout(0);

        request->onDisconnect([this, request]() {
            m_cometRequests.remove(request);
        });

        request->client()->write("HTTP/1.1 200 OK\r\n");
        request->client()->write("Content-Type: text/json\r\n");
        // request->client()->write("Connection: close\r\n");

        if (std::find(m_cometRequests.begin(), m_cometRequests.end(), request) == m_cometRequests.end())
            m_cometRequests.push_back(request);

        cometFinalizeAndSend();
    });

    m_webServer.onNotFound([this](AsyncWebServerRequest *request) {
        if (!serveGetFile(request)) {
            request->send(404, "text/plain", "FileNotFound");
        }
    });

    m_webServer.begin();
}

void WWSwitchWebServer::on(const char *uri, const ArRequestHandlerFunction &handler)
{
    m_webServer.on(uri, handler);
}

void WWSwitchWebServer::setup(void)
{
    setupFS();
    setupWebServer();

    m_cometEventsLastClearTimestamp = millis();
}

void WWSwitchWebServer::work(void)
{
    m_updaterServer.handleClient();
    clearOldCometEvents();
}

bool WWSwitchWebServer::serveGetFile(AsyncWebServerRequest *request)
{
    auto url = request->url();
    if (url.endsWith("/"))
        url += "index.html";

    if (!fsControl->exists(url))
        return false;

    request->send(*fsControl, url);
    return true;
}

void WWSwitchWebServer::postCometEvent(const String &rawJson)
{
    m_cometEvents.emplace_back(millis(), rawJson);
    cometFinalizeAndSend();
}

void WWSwitchWebServer::postCometEvent(const String &event, const String &value)
{
    postCometEvent("{\"event\": \"" + event + "\", \"value\": \"" + value + "\"}");
}

void WWSwitchWebServer::clearOldCometEvents()
{
    // Нет смысла делать перебор протухших событий слишком часто
    if ((millis() - m_cometEventsLastClearTimestamp) < (COMET_EVENTS_LIFE_TIME / 2))
        return;

    m_cometEventsLastClearTimestamp = millis();

    if (m_cometEvents.empty())
        return;

    // Находим первое НЕпротухшее событие (поэтому <=), и чистим от начала и до него
    for (auto iter = m_cometEvents.begin(); iter != m_cometEvents.end(); ++iter)
        if ((millis() - iter->first) <= COMET_EVENTS_LIFE_TIME)
        {
            if (iter != m_cometEvents.begin())
                m_cometEvents.erase(m_cometEvents.begin(), iter);

            return;
        }

    // Не нашли ни одного НЕпротухшего, значит чистим всё
    m_cometEvents.clear();
}

void WWSwitchWebServer::cometFinalizeAndSend(void)
{
    if (m_cometEvents.empty())
        return;

    if (m_cometRequests.empty())
        return;

    String jsonArrayStr(COMET_CONTENT_START_STRING);
    for (const auto &evt: m_cometEvents)
    {
        jsonArrayStr += evt.second;
        if (evt != m_cometEvents.back())
            jsonArrayStr += ",";
    }
    jsonArrayStr += COMET_CONTENT_END_STRING;

    for (const auto &req: m_cometRequests)
    {
        req->client()->write("Content-Length: ");
        req->client()->write(String(jsonArrayStr.length()).c_str());
        req->client()->write("\r\n");

        req->client()->write("\r\n");

        req->client()->write(jsonArrayStr.c_str());
        req->client()->write("\r\n");
    }

    m_cometEvents.clear();
    m_cometRequests.clear();
}
