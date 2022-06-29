// Load Wi-Fi library
#include <WiFi.h>
#include <HTTPClient.h>

//Declaration of PIN
const int departReceiver = 4;  // the number of the pin
const int LED_BUILTIN = 2;

// Replace with your network credentials
const char* ssid = "ESP32-ARRIVE";
const char* password = "123456789";

// Set web server port number to 80
WiFiServer server(80);
WiFiClient client = server.available();

//Your IP address or domain name with URL path
const char* serverNameArrive = "http://192.168.4.1/arrive";

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

TaskHandle_t Task1;
TaskHandle_t Task2;

void setup() {
  Serial.begin(115200);
  pinMode(departReceiver, INPUT);
  
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  client = server.available();

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    6,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    7,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500);
}





//CHRONO VAR
unsigned int departStatus = 0;
unsigned int arriveStatus = 0;
unsigned int start_time = 0;
unsigned int stop_time = 0;
unsigned int total_time = 0;
const unsigned int OFF = 0;
const unsigned int ON = 1;


String httpGETRequest(const char* serverName) {
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "--";

  if (httpResponseCode > 0) {
    payload = http.getString();
  }
  else {
  }
  // Free resources
  http.end();

  return payload;
}

String ChronoLog = "";
int reset = 0;
void Chrono () {
  if (reset == 0) {
    reset = 1;
    ChronoLog = "<p>ALGO START</p>";
    Serial.println("ALGO START");
    while (departStatus == OFF) {
      departStatus = digitalRead(departReceiver);
    }
    ChronoLog += "<p>Borne de début prête</p>";
    Serial.println("Borne de début prête");
    while (arriveStatus == OFF) {
      arriveStatus = httpGETRequest(serverNameArrive).toInt();
    }
    ChronoLog += "<p>Borne de fin prête</p>";
    ChronoLog += "<p>En attente du courreur</p>";
    Serial.println("Borne de fin prête");
    Serial.println("En attente du courreur");
    while (departStatus == ON) {
      departStatus = digitalRead(departReceiver);
    }
    ChronoLog += "<p>Courreur en place</p>";
    Serial.println("Courreur en place");
    digitalWrite(LED_BUILTIN, HIGH);

    while (departStatus == OFF) {
      departStatus = digitalRead(departReceiver);
    }
    ChronoLog += "<p>Chrono lancé</p>";
    Serial.println("Chrono lancé");
    start_time = millis();
    while (arriveStatus == ON) {
      arriveStatus = httpGETRequest(serverNameArrive).toInt();
    }
    stop_time = millis();
    total_time = stop_time - start_time;
    ChronoLog += "<p>Courru en </p>";
    float timeSe = total_time * 0.001;
    //ChronoLog += "<p><b>" + String(timeSe) + "</b></p>" ;
    Serial.println("Courru en");
    Serial.println(timeSe);
  }
}

void ResetChrono () {
  reset = 0;
  departStatus = 0;
  arriveStatus = 0;
  start_time = 0;
  stop_time = 0;
  total_time = 0;
}

void Webserver() {
  // Listen for incoming clients
  client = server.available();
  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            //RESET CHRONO
            if (header.indexOf("GET /RESET") >= 0) {
              ResetChrono();
            }

            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            //Chrono(client);
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>CHRONO</h1>");
            client.println(ChronoLog);


            client.println("<p><a href=\"/RESET\"><button class=\"button\">RESET</button></a></p>");

            // Display current state, and ON/OFF buttons for GPIO 26
            client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button
            if (output26State == "off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for GPIO 27
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button
            if (output27State == "off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

//Task1code: Chrono Task
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
    Chrono();
}

//Task2code: Webserver
void Task2code( void * Parameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    Webserver();
  }
}

void loop() {
}