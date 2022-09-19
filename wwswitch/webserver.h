#ifndef WWSWITCH_WEBSERVER_H
#define WWSWITCH_WEBSERVER_H

#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include <deque>
#include <list>


class WWSwitchWebServer
{
public:
    WWSwitchWebServer(uint16_t port = 80, uint16_t updaterPort = 5000);

    void setup(void);
    void work(void);

    void on(const char *uri, const ArRequestHandlerFunction &handler);

    void postCometEvent(const String &rawJson);
    void postCometEvent(const String &event, const String &value);

private:
    void setupFS(void);
    void setupWebServer(void);
    bool serveGetFile(AsyncWebServerRequest *request);
    void clearOldCometEvents(void);
    void cometFinalizeAndSend(void);

    AsyncWebServer m_webServer;

    ESP8266WebServer m_updaterServer;
    ESP8266HTTPUpdateServer m_updaterServerAdapter;

    unsigned long m_cometEventsLastClearTimestamp;
    std::deque<std::pair<unsigned long, String> > m_cometEvents;
    std::list<AsyncWebServerRequest *> m_cometRequests;
};

#endif // WWSWITCH_WEBSERVER_H
