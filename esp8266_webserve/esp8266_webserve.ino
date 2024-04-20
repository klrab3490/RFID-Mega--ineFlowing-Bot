#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "kl.rab_3490";
const char* password = "kl.rab3490";

ESP8266WebServer server(80);

// HTML content to be served
String htmlContent = "<!DOCTYPE html><html><head><title>ESP8266 Web Server</title></head><body><h1>Select an option:</h1><form action=\"/\" method=\"post\"><select name=\"uid\"><option value=\"F3982C11\">UID: F3982C11</option><option value=\"F399DDC9\">UID: F399DDC9</option><option value=\"839AADC9\">UID: 839AADC9</option><option value=\"034FCE0D\">UID: 034FCE0D</option><option value=\"40E5BF55\">UID: 40E5BF55</option><option value=\"812D650C\">UID: 812D650C</option></select><input type=\"submit\" value=\"Submit\"></form>";

void handleRoot() {
  server.send(200, "text/html", htmlContent);
}

void handleForm() {
  if (server.hasArg("uid")) {
    String selectedUID = server.arg("uid");
    // Send the selected UID to Arduino Mega via serial
    Serial.println("UID: " + selectedUID);
  }
  server.send(200, "text/html", htmlContent);
}

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  // Print local IP address
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/", HTTP_POST, handleForm);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
