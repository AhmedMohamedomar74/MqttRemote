
#include "web_server.h"

// const char* ssid = "Redmi Note 8 Pro";  // Change to your WiFi SSID
// const char* password = "Sasaseseawdr1234";  // Change to your WiFi password

ESP8266WebServer server(80);

bool buttonRegistered = false;
bool executionInProgress = false;
String registeredButtonName = "";
String executionState = "Waiting"; // Possible states: Waiting, Executing, Done

void handleRoot() {
    String html = R"rawliteral(
      <!DOCTYPE html>
      <html lang="en">
      <head>
          <meta charset="UTF-8">
          <meta name="viewport" content="width=device-width, initial-scale=1.0">
          <title>ESP8266 Button Registration</title>
          <style>
              body { font-family: Arial, sans-serif; }
              h3 { color: blue; }
              #state { font-weight: bold; color: red; }
          </style>
      </head>
      <body>
          <h2>Register Button</h2>
          <input type="text" id="buttonName" placeholder="Enter button name">
          <button onclick="sendButtonName()">Register</button>
          
          <h3>State: <span id="state">Waiting</span></h3>

          <script>
              function sendButtonName() {
                  let buttonName = document.getElementById("buttonName").value.trim();
                  if (!buttonName) {
                      document.getElementById("state").innerText = "Please enter a button name!";
                      return;
                  }

                  // Set state to Executing before sending request
                  document.getElementById("state").innerText = "Waiting IR Signal...";
                  
                  fetch('/register', {
                      method: 'POST',
                      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                      body: 'button_name=' + encodeURIComponent(buttonName)
                  })
                  .then(response => response.text())
                  .then(data => {
                      console.log("Server Response:", data);
                      updateState(); // Immediately update state
                  })
                  .catch(error => {
                      console.error("Error:", error);
                      document.getElementById("state").innerText = "Error!";
                  });
              }

              function updateState() {
                  fetch('/state')
                  .then(response => response.text())
                  .then(data => {
                      console.log("Received State:", data);
                      document.getElementById("state").innerText = data;
                  })
                  .catch(error => {
                      console.error("Error fetching state:", error);
                      document.getElementById("state").innerText = "Error!";
                  });
              }

              setInterval(updateState, 1000); // Update state every second
          </script>
      </body>
      </html>
)rawliteral";

  server.send(200, "text/html", html);
}

void handleRegister() {
    if (server.hasArg("button_name")) {
        registeredButtonName = server.arg("button_name");
        Serial.println("Received button name: " + registeredButtonName);
        
        buttonRegistered = true;
        executionState = "Waiting For IR"; // Update state to Executing

        server.send(200, "text/plain", "Button Registered: " + registeredButtonName);
    } else {
        server.send(400, "text/plain", "Missing button_name");
    }
}

void handleState() {
    server.send(200, "text/plain", executionState);
}

void setupWebServer() {
    // WiFi.begin(ssid, password);

    // Serial.print("Connecting to WiFi...");
    // while (WiFi.status() != WL_CONNECTED) {
    //     delay(500);
    //     Serial.print(".");
    // }
    // Serial.println("\nConnected to WiFi!");
    Serial.print("ESP8266 IP Address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/register", HTTP_POST, handleRegister);
    server.on("/state", HTTP_GET, handleState);

    server.begin();
    Serial.println("HTTP server started");
}

void handleWebRequests() {
    server.handleClient();
}
