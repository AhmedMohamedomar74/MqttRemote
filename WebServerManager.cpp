#include "WebServerManager.h"

WebServerManager::WebServerManager(ButtonManager& buttonManager, IRModule& irModule)
    : _buttonManager(buttonManager), _irModule(irModule), _isWaitingForIR(false) {}

void WebServerManager::begin() {
    _server.on("/", HTTP_GET, [this]() { handleRoot(); });
    _server.on("/add", HTTP_POST, [this]() { handleAdd(); });
    _server.on("/check_ir", HTTP_GET, [this]() { handleCheckIR(); });
    _server.begin();
}

void WebServerManager::handleClient() {
    _server.handleClient();
}

void WebServerManager::handleRoot() {
    _server.send(200, "text/html", R"rawliteral(
        <html>
        <head>
            <script>
              function addButton() {
                  var buttonName = document.getElementById("button_name").value;
                  var xhr = new XMLHttpRequest();
                  xhr.open("POST", "/add", true);
                  xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
                  xhr.onreadystatechange = function() {
                      if (xhr.readyState == 4) {
                          if (xhr.status == 200) {
                              var response = JSON.parse(xhr.responseText);
                              if (response.message.includes("press")) {
                                  pollIRStatus();
                              } else {
                                  document.getElementById("response").innerHTML = response.message;
                              }
                          }
                      }
                  };
                  xhr.send("button_name=" + encodeURIComponent(buttonName));
              }

              function pollIRStatus() {
                  var checkXhr = new XMLHttpRequest();
                  checkXhr.open("GET", "/check_ir", true);
                  checkXhr.onreadystatechange = function() {
                      if (checkXhr.readyState == 4 && checkXhr.status == 200) {
                          var response = JSON.parse(checkXhr.responseText);
                          document.getElementById("response").innerHTML = response.message;
                          if (response.message.includes("Waiting")) {
                              setTimeout(pollIRStatus, 1000);
                          }
                      }
                  };
                  checkXhr.send();
              }
          </script>
        </head>
        <body>
            <h2>Enter Button Name</h2>
            <form id="button_form" onsubmit="event.preventDefault(); addButton();">
                <input type="text" id="button_name" name="button_name" required>
                <input type="submit" value="Add Button">
            </form>
            <div id="response"></div>
        </body>
        </html>
    )rawliteral");
}

void WebServerManager::handleAdd() {
    if (_server.hasArg("button_name")) {
        _pendingButtonName = _server.arg("button_name");
        _irModule.startListening();
        _isWaitingForIR = true;

        StaticJsonDocument<200> jsonResponse;
        jsonResponse["message"] = "Please press the IR remote button now...";
        String response;
        serializeJson(jsonResponse, response);
        _server.send(200, "application/json", response);
    } else {
        _server.send(400, "application/json", "{\"message\":\"Missing button name\"}");
    }
}

void WebServerManager::handleCheckIR() {
    if (_isWaitingForIR && !_irModule.isListeningModeActive()) {
        String payload = _irModule.getJsonCommand();
        if (payload.length() > 0) {
            _buttonManager.registerButton(_pendingButtonName, payload);
            _isWaitingForIR = false;
            StaticJsonDocument<200> jsonResponse;
            jsonResponse["message"] = "Button registered: " + _pendingButtonName;
            String response;
            serializeJson(jsonResponse, response);
            _server.send(200, "application/json", response);
        } else {
            _server.send(500, "application/json", "{\"message\":\"No IR command received\"}");
        }
    } else {
        StaticJsonDocument<200> jsonResponse;
        jsonResponse["message"] = "Waiting for IR signal...";
        String response;
        serializeJson(jsonResponse, response);
        _server.send(200, "application/json", response);
    }
}