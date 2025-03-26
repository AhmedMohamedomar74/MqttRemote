
#include "web_server.h"



// const char* ssid = "Redmi Note 8 Pro";  // Change to your WiFi SSID
// const char* password = "Sasaseseawdr1234";  // Change to your WiFi password

ESP8266WebServer server(80);

bool buttonRegistered = false;
bool executionInProgress = false;
String registeredButtonName = "";
String executionState = "Waiting"; // Possible states: Waiting, Executing, Done

bool testFlag = false;
bool yesFlag = false;
bool noFlag = false;

void handleRoot() {
    // executionState = "Task Completed"; // Temporary hardcode for testing
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
          function updateState() {
          fetch('/state')
          .then(response => response.text())
          .then(data => {
              console.log("Received state:", data);
              document.getElementById("state").innerText = data;
              
              // Uncomment and fix this line:
              if (data.trim() === "Task Completed") {  // ✅ Use .trim() and strict comparison
                  showNotification();
              }
          })
          .catch(error => {
              console.error("Error fetching state:", error);
              document.getElementById("state").innerText = "Error!";
          });
      }

      function showNotification() {
      let popup = document.createElement("div");
      popup.innerHTML = `
          <div style="position:fixed; top:50%; left:50%; transform:translate(-50%, -50%);
                      background:white; padding:20px; box-shadow:0px 0px 10px rgba(0,0,0,0.2);
                      text-align:center; border-radius:10px; z-index: 1000;">
              <p>Click test to test the new button. If it works right click YES, otherwise click NO</p>
              <button onclick="handleTest()">Test</button>
              <button onclick="handleYes()">Yes</button>
              <button onclick="handleNo()">No</button>
          </div>`;
      document.body.appendChild(popup);
  }
  function handleTest() {
    fetch('/test', { method: 'POST' })
    .then(response => response.text())
    .then(data => console.log("Test result:", data))
    .catch(error => console.error("Test failed:", error));
}

function handleYes() {
    fetch('/yes', { method: 'POST' })
    .then(response => response.text())
    .then(data => console.log("Yes response:", data))
    .catch(error => console.error("Yes failed:", error));
}

function handleNo() {
    fetch('/no', { method: 'POST' })
    .then(response => response.text())
    .then(data => console.log("No response:", data))
    .catch(error => console.error("No failed:", error));
}
          function sendButtonName() {
            const buttonName = document.getElementById("buttonName").value;
            fetch('/register', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: 'button_name=' + encodeURIComponent(buttonName)
            })
            .then(response => response.text())
            .then(data => {
                document.getElementById("state").innerText = data;
            })
            .catch(error => {
                console.error("Error:", error);
                document.getElementById("state").innerText = "Error!";
            });
          }

          setInterval(updateState, 1000);
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
        
        EEPROM_Status status = storeStringIfNotExists(registeredButtonName);
        
        if (status == EEPROM_SUCCESS) {
            buttonRegistered = true;
            executionState = "Waiting For IR";
            server.send(200, "text/plain", "Button Registered: " + registeredButtonName);
        } else if (status == EEPROM_DUPLICATE) {
            executionState = "Error: Duplicate button name";
            server.send(200, "text/plain", executionState);
        } else if (status == EEPROM_NO_SPACE) {
            executionState = "Error: EEPROM full";
            server.send(200, "text/plain", executionState);
        } else {
            executionState = "Error: Unknown error";
            server.send(500, "text/plain", executionState);
        }
    } else {
        executionState = "Error: Missing button name";
        server.send(400, "text/plain", executionState);
    }
}

void handleState() {
    server.send(200, "text/plain", executionState);
}



// Add these handler functions
void handleTest() {
    testFlag = true;
    server.send(200, "text/plain", "Test triggered");
}

void handleYes() {
    yesFlag = true;
    server.send(200, "text/plain", "Yes recorded");
}

void handleNo() {
    noFlag = true;
    server.send(200, "text/plain", "No recorded");
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
    server.on("/test", HTTP_POST, handleTest);
    server.on("/yes", HTTP_POST, handleYes);
    server.on("/no", HTTP_POST, handleNo);

    server.begin();
    Serial.println("HTTP server started");
}

void handleWebRequests() {
    server.handleClient();
}