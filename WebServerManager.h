#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include <ESP8266WebServer.h>
#include "ButtonManager.h"
#include "IRModule.h"

class WebServerManager {
public:
    WebServerManager(ButtonManager& buttonManager, IRModule& irModule);
    void begin();
    void handleClient();

private:
    ESP8266WebServer _server;
    ButtonManager& _buttonManager;
    IRModule& _irModule;
    String _pendingButtonName;
    bool _isWaitingForIR;

    void handleRoot();
    void handleAdd();
    void handleCheckIR();
};

#endif