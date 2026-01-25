#pragma once
#include <ESPAsyncWebServer.h>
#include "../interfaces/IBlindService.h"

namespace Blinds {
    namespace Infrastructure {
        class WebServerManager {
        public:
            WebServerManager(Interfaces::IBlindService& blindService, int port = 80);
            void begin();

        private:
            AsyncWebServer _server;
            Interfaces::IBlindService& _blindService;

            void setupRoutes();
        };
    }
}
