#include "WebServerManager.h"
#include "WebPage.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>

namespace Blinds {
    namespace Infrastructure {

        WebServerManager::WebServerManager(Interfaces::IBlindService& blindService, int port)
            : _server(port), _blindService(blindService) {
            setupRoutes();
        }

        void WebServerManager::begin() {
            _server.begin();
        }

        void WebServerManager::setupRoutes() {
            // UI
            _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
                request->send_P(200, "text/html", index_html);
            });

            // API
            _server.on("/health", HTTP_GET, [](AsyncWebServerRequest *request){
                request->send(200, "application/json", "{\"status\":\"ok\"}");
            });

            _server.on("/state", HTTP_GET, [this](AsyncWebServerRequest *request){
                request->send(200, "application/json", _blindService.getStateJson().c_str());
            });

            // POST /move -> {"cmd":"up"|"down"|"stop"}
            AsyncCallbackJsonWebHandler* moveHandler = new AsyncCallbackJsonWebHandler("/move",
                [this](AsyncWebServerRequest *request, JsonVariant &json) {
                    JsonObject jsonObj = json.as<JsonObject>();
                    const char* cmd = jsonObj["cmd"];

                    if (strcmp(cmd, "up") == 0) _blindService.moveUp();
                    else if (strcmp(cmd, "down") == 0) _blindService.moveDown();
                    else if (strcmp(cmd, "stop") == 0) _blindService.stop();
                    else {
                        request->send(400, "application/json", "{\"error\":\"Invalid command\"}");
                        return;
                    }
                    request->send(200, "application/json", _blindService.getStateJson().c_str());
            });
            _server.addHandler(moveHandler);

            // POST /goto -> {"percent": 50}
            AsyncCallbackJsonWebHandler* gotoHandler = new AsyncCallbackJsonWebHandler("/goto",
                [this](AsyncWebServerRequest *request, JsonVariant &json) {
                    JsonObject jsonObj = json.as<JsonObject>();
                    if (jsonObj.containsKey("percent")) {
                        int percent = jsonObj["percent"];
                        _blindService.setPosition(percent);
                        request->send(200, "application/json", _blindService.getStateJson().c_str());
                    } else {
                        request->send(400, "application/json", "{\"error\":\"Missing percent\"}");
                    }
            });
            _server.addHandler(gotoHandler);

            // POST /home
            _server.on("/home", HTTP_POST, [this](AsyncWebServerRequest *request){
                _blindService.home();
                request->send(200, "application/json", _blindService.getStateJson().c_str());
            });

            // CORS (Optional)
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
        }
    }
}
