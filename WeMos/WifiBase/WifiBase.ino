#include <ESP8266WiFi.h>    // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>     // https://github.com/tzapu/WiFiManager

WiFiClient client;
ESP8266WebServer server(80); // Start server on port 80 

String SETTINGS_FILE = "/config.txt";

String pingBackAPI;
int LED_PIN = LED_BUILTIN;
int LED_PIN_STATE = 0;

void setup() {
  Serial.begin(115200);
  delay(3000); //Is required to let the board boot completly and SPIFFS ready, ignoring this may cause to always boot in AP mode.
  //WiFiManager intialisation. Once completed there is no need to repeat the process on the current board
  WiFiManager wifiManager;
  // New OOB ESP8266 has no Wi-Fi credentials so will connect. Below will create an AP with name ESP<ChipID>.
  wifiManager.autoConnect();
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
 
  server.begin();
  Serial.println("Server started");
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  
  // Next define what the server should do when a client connects
  server.on("/", handleRoot); // The client connected with no arguments e.g. http:192.160.0.40/
  server.on("/whoAmI", whoAmI);
  server.on("/setConfig", setConfig);
  server.on("/set", ProcessRequest);
  server.on("/getStatus", getStatus);
  
  //Custom code starts here
  SPIFFS.begin();
  readFS();
  delay(500); //let read the FS completly
  pinMode(LED_PIN, OUTPUT);
  setLED();
}

void handleRoot() {
  String response;
  response =  "<html>";
  response += "<head><title>ESP8266 Control</title>";
  response += "<style>";
  response += "body { background-color: #E6E6FA; font-family: Arial, Helvetica, Sans-Serif; Color: blue;}";
  response += ".button {text-align: center; text-decoration: none; color: white; display: block; border: 2px solid #0088CC; padding: 10px; background-color: purple; font-weight: bold;}";
  response += "</style>";
  response += "</head>";
  response += "<body>";
  response += "<h1><br>ESP8266 Server - Getting input from a client</h1>";  
  response += "<a class='button' href='/set?pin=2&state=1'>Set On</a><br>";
  response += "<a class='button' href='/set?pin=2&state=0'>Set Off</a><br>";
  response += "</body>";
  response += "</html>";
  server.send(200, "text/html", response); // Send a response to the client asking for input
}

void ProcessRequest(){
    if (server.args() > 0 ) { // Arguments were received
      //request came as '/set?pin=4&state=1'

      if ((u8_t)server.arg("pin").toInt() == LED_PIN){
        LED_PIN_STATE = server.arg("state").toInt();
        setLED();
      }
      String response = "{'TYPE':'STATE_CHANGE', 'PIN': '" + LED_PIN;
      response += "', 'VALUE': '" + LED_PIN_STATE;
      response += "', 'STATUS': 'SUCCESSFUL'}";
      pingBack(response);
    }
  //server.send(200, "text/html", "Successful...");
  handleRoot();
}

void getStatus() {
  String response = "{'PIN': '" + LED_PIN;
  response += "', 'VALUE': '" + LED_PIN_STATE;
  response += "'}";
  server.send(200, "text/json", response); // Send a response to the client asking for input
}

void whoAmI(){
  Serial.println("whoAmI requested... Sending following data:");
  String response;
  response =  "{ \"i_am\":\"ESP" ; 
  response += ESP.getChipId();
  response += "\",   \"hosted_at\":\"";
  response += WiFi.localIP().toString();
  response += "\", \"i_have\": { \"SWITCH\": { \"at_url\":\"/set?pin=D1\", ";
  response += "\"which_can\":[{ \"turn\":\"On\", \"via\":\"value=on\" }, ";
  response += "{ \"turn\":\"Off\", \"via\":\"value=off\" }]} ";
  response += ",\"BRIDGE\":{ \"THROUGH\":\"<HOST_NAME>\",";
  response += "\"CONNECTSTO\":\"SSID\",";
  response += "\"CONNECTIONIS\":\"DIRECT\"}}}";

  Serial.println(response);
  server.send(200, "text/json", response); // Send a response to the client asking for input  
}

void setConfig(){
  //request came as '/setConfig?pingBackAPI=http://1.1.1.1/API/acceptPings'
  if (server.args() > 0){
    if (server.hasArg("pingBackAPI"))
      pingBackAPI = server.arg("pingBackAPI");
    // Add More config variables here

    //Save the configuration to SPIFFS.
    writeFS(false);
  }
}
void writeFS(bool clearSettings){
  if (!clearSettings){
    Serial.println("opening file:" + SETTINGS_FILE);
    File f = SPIFFS.open(SETTINGS_FILE, "w");
    if (!f){
      Serial.println("Cannot open file for writing");      
    }else{
      f.println("PINGA:" + pingBackAPI); //Save ping back url
      
      f.println("LEDST:" + LED_PIN_STATE); //Save LED current Status
      //Add More lines to save configuration here.
      
      Serial.println("Written to file.");
      //f.close();
    }
    f.close();
  } else {
    if (SPIFFS.exists(SETTINGS_FILE)){
      SPIFFS.remove(SETTINGS_FILE);
    }
  }
}

void readFS(){
    File f = SPIFFS.open(SETTINGS_FILE, "r");
    if (f){
      while(f.available()){
        String s=f.readStringUntil('\n');
        Serial.println("Line:" + s);
        Serial.println("Command:" + s.substring(0,5) + " - Value:" + s.substring(6));
        if (s.substring(0,5) == "PINGA"){
            pingBackAPI = s.substring(6);
        } else if (s.substring(0,5) == "LEDST"){
            LED_PIN_STATE = s.substring(6).toInt();
        }

//        switch (s.substring(0,5).toString()){
//          case "PINGA":
//            pingBackAPI = s.substring(6);
//            break;
//          case "LEDST":
//            LED_PIN_STATE = s.substring(6).toInt();
//            break;
//        }
      }
    }
    else {
      Serial.println("File does not exist");
    }
    f.close();
}

void resetESP(bool fullFormat){
  if (fullFormat){
    Serial.println("Please wait 30 secs for SPIFFS to be formatted");
    SPIFFS.format();
    Serial.println("Spiffs formatted");
  } else {
    writeFS(true);
  }
}

void pingBack(String message){
  if(pingBackAPI){
    String response;
    Serial.println("sending post");
    HTTPClient http;    //Declare object of class HTTPClient
    http.begin(pingBackAPI);      //Specify request destination
    http.addHeader("Content-Type", "text/plain");  //Specify content-type header

    int httpCode = http.POST(message);   //Send the request
    
    String payload = http.getString();                  //Get the response payload
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
    
    http.end();  //Close connection
  }
}
void loop() {

    server.handleClient();
}

// Custom methods go below

void setLED(){
  if (LED_PIN_STATE == 0){
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

