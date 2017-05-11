#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN     D4 
#define PIXEL_COUNT   60
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

int ledPin = 2; // GPIO2 of ESP8266
WiFiServer server(80);//Service Port

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect();
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    
    server.begin();
    Serial.println("Server started");
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    Serial.println("Strip Initialized");
}

void loop() {
  // put your main code here, to run repeatedly:
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
   
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
   
  // Match the request
  processRequest(request);
     
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html><head><style>.button {text-align: center; text-decoration: none; color: white; display: block; border: 2px solid #0088CC; padding: 10px; background-color: purple; font-weight: bold;}</style></head>");
  client.println("<body style='font-family: sans-serif; font-size: 12px'> <h1 style='text-align: center;display: block'>LED Functions</h1>");
  client.println("<a class='button ' href='/WPO'>Wipe OFF</a><br><a class='button ' href='/WPR'>Wipe RED</a><br> ");
  client.println("<a class='button ' href='/WPG'>Wipe Green</a><br> <a class='button ' href='/WPB'>Wipe BLUE</a><br>");
  client.println("<a class='button ' href='/TCW'>TheaterChase WHITE</a><br> ");
  client.println("<a class='button ' href='/TCR'>TheaterChase RED</a><br> ");
  client.println("<a class='button ' href='/TCG'>TheaterChase GREEN</a><br>");
  client.println("<a class='button ' href='/TCB'>TheaterChase BLUE</a><br> ");
  client.println("<a class='button ' href='/RBO'>Rainbow</a><br> ");
  client.println("<a class='button ' href='/RBC'>Rainbow Cycle</a><br>");
  client.println("<a class='button ' href='/TCR'>TheaterChase Rainbow</a><br> </body></html>");

 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");    
}
void processRequest(String request){
    if (request.indexOf("/WPO") != -1){
      colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
    }
    else if (request.indexOf("/WPR") != -1){
      colorWipe(strip.Color(255, 0, 0), 50);  // Red
    }
    else if (request.indexOf("/WPG") != -1){
      colorWipe(strip.Color(0, 255, 0), 50);  // Green
    }
    else if (request.indexOf("/WPB") != -1){
      colorWipe(strip.Color(0, 0, 255), 50);  // Blue
    }
    else if (request.indexOf("/TCW") != -1){
      theaterChase(strip.Color(127, 127, 127), 50); // White
    }
    else if (request.indexOf("/TCR") != -1){
      theaterChase(strip.Color(127,   0,   0), 50); // Red
    }
    else if (request.indexOf("/TCB") != -1){
      theaterChase(strip.Color(  0,   0, 127), 50); // Blue
    }
    else if (request.indexOf("/TCG") != -1){
      theaterChase(strip.Color(  0,   255, 0), 50); // Green
    }
    else if (request.indexOf("/RBO") != -1){
      rainbow(20);
    }
    else if (request.indexOf("/RBC") != -1){
      rainbowCycle(20);
    }
    else if (request.indexOf("/TCR") != -1){
      theaterChaseRainbow(50);
    }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

