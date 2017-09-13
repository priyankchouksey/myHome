#include <ESP8266WiFi.h>
 
const char* ssid = "";
const char* password = "";
 
WiFiServer server(80);
 
void setup() {
  Serial.begin(115200);
  delay(10);
 
 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
 
  // Connect to WiFi network
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  // Start the server
  server.begin();
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  client.flush();
 
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(LED_BUILTIN, LOW);
    value = HIGH;
  } 
  if (request.indexOf("/LED=OFF") != -1){
    digitalWrite(LED_BUILTIN, HIGH);
    value = LOW;
  }
  if(value == HIGH) {
    client.print("On");  
  } else {
    client.print("Off");
  }
 
  delay(1);
 
}

