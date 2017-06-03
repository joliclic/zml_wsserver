/*
 * ZML WebSocket Server
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <Adafruit_NeoPixel.h>

#include "router_config.h"

#define USE_SERIAL Serial

#define MY_HOSTNAME "PifLeChien"
#define USE_STATIC_IP 0
//#define NUM_PIXELS 144
#define NUM_PIXELS 20
#define CMD_PIN D2

#define MAX_SPEED_DIVISOR 20

//ESP8266WiFiMulti WiFiMulti;

WebSocketsServer webSocket = WebSocketsServer(81);

#if USE_STATIC_IP
IPAddress staticIP(192,168,0,42);
IPAddress gateway(192,168,0,255);
IPAddress subnet(255,255,255,0);
#endif

Adafruit_NeoPixel pixels(NUM_PIXELS, CMD_PIN, NEO_GRB | NEO_KHZ800);

void (*gCurrentAction)();

long gNextActionTime = -1;

unsigned int gCurStep = 0;

//uint8_t gColorR, gColorG, gColorB;
//uint32_t gCurrentColor = pixels.Color(0, 0, 0);
uint32_t gCurrentColor = pixels.Color(169, 0, 255);

const uint32_t COLOR_PURPLE = pixels.Color(169, 0, 255);
const uint32_t COLOR_ORANGE = pixels.Color(255, 130, 0);

const uint16_t MAX_VARIABLE_DELAY = 2000; // ms
uint16_t gVariableDelay = 1000; //ms

void blackLeds() {
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        pixels.setPixelColor(i, 0);
    }
    pixels.show();
}

void myBigLoop() {
    USE_SERIAL.print("*");
    setDelay(500);
}

void setDelay(long aDelay) {
    if (aDelay < 0)
        gNextActionTime = -1;
    else
        gNextActionTime = millis() + aDelay;
}

int setSpeedDivisor(uint8_t aDivisor) {
    if (aDivisor > MAX_SPEED_DIVISOR)
        aDivisor = MAX_SPEED_DIVISOR;
    
    return aDivisor;
}

int setVariableSpeed(uint8_t aDivisor) {
    aDivisor = setSpeedDivisor(aDivisor);
    if (aDivisor == 0)
        gVariableDelay = -1;
    else
        gVariableDelay = MAX_VARIABLE_DELAY / aDivisor;
}

void helloPixels() {
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(100, 100, 100));
    }
    pixels.show();
    delay(100);
    blackLeds();
    USE_SERIAL.print("Leds should have blinked\n");
}

void doBlink() {
    gCurStep = 0;
    gCurrentAction = &blink;
    gCurrentAction();
}

void blink() {
    switch (gCurStep) {
        case 0:
            for (uint8_t i = 0; i < NUM_PIXELS; i++) {
                pixels.setPixelColor(i, gCurrentColor);
            }
            pixels.show();
            setDelay(gVariableDelay);
            break;
        case 1:
            blackLeds();
            setDelay(gVariableDelay);
            break;
    }
    gCurStep++;
    if (gCurStep > 1)
        gCurStep = 0;
}

int16_t gChaseLastLedOn = -1;

void doChase() {
    gCurrentAction = &chase;
    gChaseLastLedOn = -1;
    chase();
}

void chase() {
    if (gChaseLastLedOn >= 0) 
        pixels.setPixelColor(gChaseLastLedOn, 0);
    
    gChaseLastLedOn++;
    if (gChaseLastLedOn >= NUM_PIXELS)
        gChaseLastLedOn = 0;
    
    pixels.setPixelColor(gChaseLastLedOn, gCurrentColor);
    pixels.show();
    setDelay(gVariableDelay);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
    
    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);                webSocket.sendTXT(num, "Connected");
            break;
        }
        case WStype_TEXT: {
            String text = String((char *) &payload[0]);
            const char*  chars_payload = (const char *) payload;
            int text_length = strlen(chars_payload);
            
            USE_SERIAL.printf("[%u] get command: %s\n", num, payload);
            // send message to client
            webSocket.sendTXT(num, "received command: " + text);
            
            int r, g, b;
            int s;
            int res;
            
            if (text == "ping") {
                webSocket.sendTXT(num, "pong");
            } else if (text == "black") {
                gCurrentAction = &blackLeds;
                blackLeds();
                setDelay(-1);
                USE_SERIAL.print("all leds should be turned off now...\n");
            } else if (text == "color:purple") {
                gCurrentColor = COLOR_PURPLE;
            } else if (text == "color:orange") {
                gCurrentColor = COLOR_ORANGE;
            } else if (text == "bigloop") {
                gCurrentAction = &myBigLoop;
                gCurrentAction();
            } else if (text == "blink") {
                doBlink();
            } else if (text == "chase") {
                doChase();
            } else if (text_length == 7 || text_length == 8) {
                USE_SERIAL.print("text_length = 7 or 8\n");
                res = sscanf(chars_payload, "speed:%d", &s);
                if (res == 1) {
                    USE_SERIAL.printf("speed: %d\n", s);
                    setVariableSpeed(s);
                }
            } else if (text_length == 12) {
                USE_SERIAL.print("text_length = 12\n");
                res = sscanf(chars_payload, "color#%02x%02x%02x", &r, &g, &b);
                if (res == 3) {
                    USE_SERIAL.printf("found: %u, r: %u, g: %u, b: %u\n",
                                      res, r, g, b);
                    gCurrentColor = pixels.Color((uint8_t) r,
                                                 (uint8_t) g,
                                                 (uint8_t) b);
                }
            }
            
            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        }
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary lenght: %u\n", num, lenght);
            hexdump(payload, lenght);

            // send message to client
            // webSocket.sendBIN(num, payload, lenght);
            break;
    }

}

void setup() {
    gCurrentAction = &blackLeds;
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(115200);
    //USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.disconnect();
    pixels.begin();
    helloPixels();
    
    WiFi.hostname(MY_HOSTNAME);
    WiFi.mode(WIFI_STA);
    Serial.printf("Wi-Fi mode set to WIFI_STA %s\n", WiFi.mode(WIFI_STA) ? "" : "Failed!");
#if USE_STATIC_IP
    // Doc says it should be faster
    WiFi.config(staticIP, gateway, subnet);
#endif
    
    WiFi.begin(MY_SSID, MY_PASSWORD);

    while(WiFi.status() != WL_CONNECTED) {
        delay(100);
        Serial.print(".");
    }
    Serial.println();
    helloPixels();
    
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
    Serial.printf("Gateway IP: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.printf("Connection status: %d\n", WiFi.status());
    
//    WiFiMulti.addAP("MY_SSID", "MY_PASSWORD");
//    while(WiFiMulti.run() != WL_CONNECTED) {
//        delay(100);
//    }

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
    
    if (gNextActionTime != -1 && gNextActionTime <= millis()) {
        gCurrentAction();
    }
}

