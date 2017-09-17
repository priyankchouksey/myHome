#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"
 
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
 
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
 
FASTLED_USING_NAMESPACE
 
// FastLED "100-lines-of-code" demo reel, showing just a few
// of the kinds of animation patterns you can quickly and easily
// compose using FastLED. 
//
// This example also shows one easy way to define multiple
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014
 
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif
 
#define DATA_PIN    D4
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];
 
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120
 
WiFiServer server(80);//Service Port
 
void setup() {
    Serial.begin(115200);
    delay(3000); // 3 second delay for recovery
    Serial.println("Starting Wifi");
 
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    wifiManager.autoConnect();
 
        //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
   
    server.begin();
    Serial.println("Server started");
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
   
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
 
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}
 
 
// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
 
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
 
void loop()
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    if (client.available()){
      String request = client.readStringUntil('\r');
      Serial.println("Request data:");
      Serial.print(request);
      processRequest(request);
 
      // Return the response
      webResponse(client, request);
 
    }
  }
 
 
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();
 
  // send the 'leds' array out to the actual LED strip
  FastLED.show(); 
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND);
 
  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}
 
void webResponse(WiFiClient client, String request){
  if (request.indexOf("/STATUS") != -1){
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println(""); //  do not forget this one
      client.println("{");
      client.println("currentPattern:5");
      client.println("}");   
  }
  else{
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println(""); //  do not forget this one
      client.println("<!DOCTYPE HTML>");
      client.println("<html><head><style>.button {text-align: center; text-decoration: none; color: white; display: block; border: 2px solid #0088CC; padding: 10px; background-color: purple; font-weight: bold;}</style></head>");
      client.println("<body style='font-family: sans-serif; font-size: 12px'> <h1 style='text-align: center;display: block'>LED Functions</h1>");
      client.println("<a class='button ' href='/WPO'>Wipe OFF</a><br><a class='button ' href='/WPR'>Wipe RED</a><br> ");
      client.println("<a class='button ' href='/WPG'>Wipe Green</a><br> <a class='button ' href='/WPB'>Wipe BLUE</a><br>");
      client.println("<a class='button ' href='/RBO'>Rainbow</a><br> ");
      client.println("<a class='button ' href='/RBG'>Rainbow with Glitter</a><br> ");
      client.println("<a class='button ' href='/CNF'>Confetti</a><br>");
      client.println("<a class='button ' href='/SLN'>Sinelon</a><br> ");
      client.println("<a class='button ' href='/JGL'>Juggle</a><br> ");
      client.println("<a class='button ' href='/BPM'>BPM</a><br><br><a class='button' href='/STATUS'>Get Current Status</a> </body></html>");
  }
 
}
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
 
void processRequest(String request){
    if (request.indexOf("/WPO") != -1){
      //colorWipe(strip.Color(0, 0, 0), 50);    // Black/off
    }
    else if (request.indexOf("/WPR") != -1){
      //colorWipe(strip.Color(255, 0, 0), 50);  // Red
    }
    else if (request.indexOf("/WPG") != -1){
      //colorWipe(strip.Color(0, 255, 0), 50);  // Green
    }
    else if (request.indexOf("/WPB") != -1){
      //colorWipe(strip.Color(0, 0, 255), 50);  // Blue
    }
    else if (request.indexOf("/RBO") != -1){
      gCurrentPatternNumber = 0;
    }
    else if (request.indexOf("/RBG") != -1){
      gCurrentPatternNumber = 1;
    }
    else if (request.indexOf("/CNF") != -1){
      gCurrentPatternNumber = 2;
    }
    else if (request.indexOf("/SLN") != -1){
      gCurrentPatternNumber = 3;
    }
    else if (request.indexOf("/JGL") != -1){
      gCurrentPatternNumber = 4;
    }
    else if (request.indexOf("/BPM") != -1){
      gCurrentPatternNumber = 5;
    }
 
}
void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}
 
void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}
 
void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}
 
void addGlitter( fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}
 
void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}
 
void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}
 
void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}
 
void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
