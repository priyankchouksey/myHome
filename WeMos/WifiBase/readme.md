# ESP8266 base wifi sketch

This is a base sketch that you can use to create various implementation of your esp6266 module. The sketch includes complete end to end implementation of features, using the led on/off sample. You can customise the implementations of commands to change the modes of pins you want to work with.

## Features: 

  - Creates an **AP** for the first time the board is started, so it can be easily configured to connect a wifi router.
  - **Saves** the wifi settings provided, so next time it can directly connect to wifi.
  - Has a built-in **ping back** mechanish. Configure a rest server somewhere to handle the post requests and set the api URL to board, the board will call the api with data everytime a state changes.
  - Has an in-built **webserver** with common features
  - The webserver is also able to handle **REST requests**. Following routes are provided.
    - "/" is the default route which provides a web page as a response. This could be customized as per your board implementation.
    - "/whoAmI" provides json with detailed information about the board and what it  could do. This could be useful if you want to provide a discovery options in your apps (like mobile app).
    - "/setConfig" accepts configuration data that you want to store in the device e.g. pingBackURL.
    - "/set" will accept post data and will perform actions. You can modify the ProcessRequest() function to handle data and perform pin actions.
    - "/getStatus" if you are not configuring any pingback server, you can call this api to get current status of the pins.

### How to Install... 
- This document consiters that you already have Arduino IDE configured to upload sketch in esp boards. If not, follow [this][sparkfun/boardManager] or [this][esp8266/boardManager] guide.
- In addition to [esp8266 core][esp8266/core], this sketch also depends on [WifiManager]. Follow [this][WifiManager] guide to setup the library.
- Open the sketch in arduino IDE and upload to the esp board.
 
### How to (End) use...
- When the board boots, it creates an AP with a name 'ESP<ChipID>', where <chipID> is a unique id of the board based on the mac address.
- From your PC/phone connect to that access point, it should automatically open a configuration page where you can save the wifi configuration of your router. If the configuration page does not appear, open http://192.168.4.1 in your browser.
- Once saved the board will auto reboot and try to connect to your wifi router with saved configuration. You may need to find new ip of the esp board in your router (showing the connected IP is still in process of development).
- Once connected open the http://<new IP> in your browser, it should now show a page to control the esp on board LED.
- You have another server, which should be notified on each action esp performs? Add the pingback API url by calling http://<newIP>/setConfig?pingBackAPI=<APIurl>

### What to Modify...
This sketch can be used as a baseline project for your further development. Download the sketch and modify as below.
- Add more server.on routes in setup function if you need more APIs in the esp webserver. The first argument is path and the second is the function name to call when this route/API is called from client.
- Modify the defualt html in the handleRoot function per your implementaiton in the board.
- If you are adding different implementation of pins, modify the processRequest function to handle api parameters and perform specific tasks.
- Modify the writeFS and readFS  method to save/read additional parameters per your implementation.
- Modify the pingBack function to send the data as needed to the pingBack server.  

   [esp8266/boardManager]: <https://github.com/esp8266/Arduino/blob/master/README.md#installing-with-boards-manager>
   [sparkfun/boardManager]: <https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/installing-the-esp8266-arduino-addon>
   [WifiManager]: <https://github.com/tzapu/WiFiManager#install-through-library-manager>
   
