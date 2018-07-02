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

#include "ZML_Mask.h"

#include "router_config.h"
#include "wifi_host_config.h"
#include "leds_layout.h"

#define USE_SERIAL Serial

#define MASK_CMD_PIN D2

#define BIG_TICK 42

ZML_Mask mask(MASK_NB_LED_GROUPS, MASK_NB_LED_MAX_PER_GROUP,
              &MASK_LEDS_LAYOUT[0][0], MASK_NUM_PIXELS, MASK_CMD_PIN,
              NEO_GRB | NEO_KHZ800);

//ESP8266WiFiMulti WiFiMulti;

WebSocketsServer webSocket = WebSocketsServer(81);

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
            } else if (text == "black" || text == "blackout") {
                mask.doBlackOut();
                USE_SERIAL.print("all leds should be turned off now...\n");
            } else if (text == "continuous") {
                mask.doContinuous();
            } else if (text == "color:purple") {
                mask.setColor(mask.COLOR_PURPLE);
            } else if (text == "color:orange") {
                mask.setColor(mask.COLOR_ORANGE);
            } else if (text == "blink") {
                mask.doBlink();
            } else if (text == "chase") {
                mask.doChase();
            } else if (text == "doublechase") {
                mask.doDoubleChase();
            } else if (text == "heart") {
                mask.doHeart();
            } else if (text == "random") {
                USE_SERIAL.print("receive random command\n");
                mask.paintRandomColors();
                mask.setDelay(-1);
                USE_SERIAL.print("random pixel colors should be painted...\n");
            } else if (text_length == 12 || text_length == 13) {
                USE_SERIAL.print("text_length = 12 or 13\n");
                res = sscanf(chars_payload, "color:#%02x%02x%02x", &r, &g, &b);
                if (res == 3) {
                    USE_SERIAL.printf("found: %u, r: %u, g: %u, b: %u\n",
                                      res, r, g, b);
                    mask.setColor((uint8_t) r, (uint8_t) g, (uint8_t) b);
                } else {
                    res = sscanf(chars_payload, "blinkspeed:%d", &s);
                    if (res == 1) {
                        USE_SERIAL.printf("blinkspeed: %d\n", s);
                        mask.setBlinkSpeed(s);
                    } else {
                        res = sscanf(chars_payload, "chasespeed:%d", &s);
                        if (res == 1) {
                            USE_SERIAL.printf("chasespeed: %d\n", s);
                            mask.setChaseSpeed(s);
                        } else {
                            res = sscanf(chars_payload, "dchsespeed:%d", &s);
                            if (res == 1) {
                                USE_SERIAL.printf("dchsespeed: %d\n", s);
                                mask.setDoubleChaseSpeed(s);
                            } else {
                                res = sscanf(chars_payload, "heartspeed:%d", &s);
                                if (res == 1) {
                                    USE_SERIAL.printf("heartspeed: %d\n", s);
                                    // we want in fact the opposite of s
                                    // setHeartStepFactor(-s);
                                    mask.setHeartStepFactor(-s);
                                }
                            }
                        }
                    }
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
    mask.helloPixels();
    
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
    mask.helloPixels();
    
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.print("hostname: ");
    Serial.println(MY_HOSTNAME);
    Serial.printf("Gateway IP: %s\n", WiFi.gatewayIP().toString().c_str());
    Serial.print("subnet mask: ");
    Serial.println(WiFi.subnetMask());
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
    mask.loop();
}

