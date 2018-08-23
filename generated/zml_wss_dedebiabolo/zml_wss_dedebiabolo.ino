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
#include "ZML_Matrix.h"

#include "router_config.h"
#include "wifi_host_config.h"
#include "mask_layout.h"
#include "matrix_layout.h"

#if MASK_NUM_PIXELS
#define USE_MASK 1
#endif

#if MATRIX_WIDTH
#define USE_MATRIX 1
#endif

#define USE_SERIAL Serial

#ifndef MASK_CMD_PIN
#define MASK_CMD_PIN D2
#endif
#ifndef MATRIX_CMD_PIN
#define MATRIX_CMD_PIN D8
#endif
#ifndef MATRIX_EMPTY_PIN
#define MATRIX_EMPTY_PIN D0
#endif

#define MATRIX_BRIGHTNESS 50

#define MAX_RECEIVED_CMD_LENGTH 256

#if USE_MASK
ZML_Mask mask(MASK_NB_LED_GROUPS, MASK_NB_LED_MAX_PER_GROUP,
              &MASK_LEDS_LAYOUT[0][0], MASK_NUM_PIXELS, MASK_CMD_PIN,
              NEO_GRB | NEO_KHZ800);
#endif

#if USE_MATRIX
ZML_Matrix matrix(MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_CMD_PIN,
                  MATRIX_LAYOUT_FLAGS, MATRIX_LED_TYPE, MATRIX_BRIGHTNESS);
#endif

//ESP8266WiFiMulti WiFiMulti;

WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
                IPAddress ip = webSocket.remoteIP(num);
                USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n",
                                  num, ip[0], ip[1], ip[2], ip[3], payload);
                webSocket.sendTXT(num, "Connected");
            break;
        }
        case WStype_TEXT: {
            // String text = String((char *) &payload[0]);
            // const char *chartext = (const char *) payload;
            char *chartext = (char *) payload;
            int text_length = strlen(chartext);
            if (text_length > MAX_RECEIVED_CMD_LENGTH)
                break;
            
            USE_SERIAL.printf("[%u] get command: %s\n", num, payload);
            // send message to client
            char msg[] = "received command: ";
            webSocket.sendTXT(num, strcat(msg, chartext));
            
            int r, g, b;
            int s;
            int res;
            
            // if (text == "ping") {
            if (strcmp(chartext, "ping") == 0) {
                webSocket.sendTXT(num, "pong");
            }
#if USE_MASK
            else if (strncmp(chartext, "mask:", 5) == 0) {
                chartext += 5;
                text_length = strlen(chartext);
                USE_SERIAL.printf("Received mask command: %s\n", chartext);
                
                if (strcmp(chartext, "flash") == 0) {
                    mask.flash();
                } else if (strcmp(chartext, "black") == 0
                           || strcmp(chartext, "blackout") == 0) {
                    mask.doBlackOut();
                    USE_SERIAL.print("all leds should be turned off now...\n");
                } else if (strcmp(chartext, "continuous") == 0) {
                    mask.doContinuous();
                } else if (strcmp(chartext, "color:purple") == 0) {
                    mask.setColor(mask.COLOR_PURPLE);
                } else if (strcmp(chartext, "color:orange") == 0) {
                    mask.setColor(mask.COLOR_ORANGE);
                } else if (strcmp(chartext, "blink") == 0) {
                    mask.doBlink();
                } else if (strcmp(chartext, "chase") == 0) {
                    mask.doChase();
                } else if (strcmp(chartext, "doublechase") == 0) {
                    mask.doDoubleChase();
                } else if (strcmp(chartext, "heart") == 0) {
                    mask.doHeart();
                } else if (strcmp(chartext, "random") == 0) {
                    USE_SERIAL.print("receive random command\n");
                    mask.paintRandomColors();
                    // mask.setDelay(-1);
                    USE_SERIAL.print("random pixel colors should be painted...\n");
                } else if (text_length == 12 || text_length == 13) {
                    USE_SERIAL.print("text_length = 12 or 13\n");
                    res = sscanf(chartext, "color:#%02x%02x%02x", &r, &g, &b);
                    if (res == 3) {
                        USE_SERIAL.printf("found: %u, r: %u, g: %u, b: %u\n",
                                        res, r, g, b);
                        mask.setColor((uint8_t) r, (uint8_t) g, (uint8_t) b);
                    } else {
                        res = sscanf(chartext, "blinkspeed:%d", &s);
                        if (res == 1) {
                            USE_SERIAL.printf("blinkspeed: %d\n", s);
                            mask.setBlinkSpeed(s);
                        } else {
                            res = sscanf(chartext, "chasespeed:%d", &s);
                            if (res == 1) {
                                USE_SERIAL.printf("chasespeed: %d\n", s);
                                mask.setChaseSpeed(s);
                            } else {
                                res = sscanf(chartext, "dchsespeed:%d", &s);
                                if (res == 1) {
                                    USE_SERIAL.printf("dchsespeed: %d\n", s);
                                    mask.setDoubleChaseSpeed(s);
                                } else {
                                    res = sscanf(chartext, "heartspeed:%d", &s);
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
            }
#endif
#if USE_MATRIX
            else if (strncmp(chartext, "matrix:", 7) == 0) {
                chartext += 7;
                text_length = strlen(chartext);
                USE_SERIAL.printf("Received matrix command: %s\n", chartext);
                
                if (strcmp(chartext, "black") == 0
                           || strcmp(chartext, "blackout") == 0) {
                    matrix.blackOut();
                } else if (strcmp(chartext, "demo") == 0) {
                    matrix.demo();
                } else if (strcmp(chartext, "stopdemo") == 0) {
                    matrix.stopDemo();
                } else if (strcmp(chartext, "scrolltext") == 0) {
                    matrix.scrollText();
                } else if (strcmp(chartext, "fillrandom") == 0) {
                    matrix.fillRandom();
                } else if (strcmp(chartext, "circles") == 0) {
                    matrix.fillWithCircles();
                } else if (strcmp(chartext, "fixedzigzag") == 0) {
                    matrix.fixedZigzag();
                } else if (strcmp(chartext, "zigzag") == 0) {
                    matrix.zigzag();
                } else if (strcmp(chartext, "rainbow") == 0) {
                    matrix.drawRainbowVLines();
                } else if (strcmp(chartext, "rainbowh") == 0) {
                    matrix.drawRainbowHLines();
                } else if (strcmp(chartext, "fire") == 0) {
                    matrix.fire();
                } else if (strncmp(chartext, "setscrolledtext:", 16)) {
                    chartext += 16;
                    text_length = strlen(chartext);
                    matrix.setScrolledText(chartext);
                }
            }
#endif
            
            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        }
        case WStype_BIN:
            USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
            hexdump(payload, length);

            // send message to client
            // webSocket.sendBIN(num, payload, length);
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
    
    randomSeed(analogRead(MATRIX_EMPTY_PIN));
    
    WiFi.disconnect();
#if USE_MASK
#ifdef MASK_BRIGHTNESS
    mask.setBrightness(MASK_BRIGHTNESS);
#endif
    mask.helloPixels();
#endif
    
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
#if USE_MASK
    mask.helloPixels();
#endif
    
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
    
#if USE_MATRIX
    matrix.setFirePosition(MATRIX_FIRE_X0, MATRIX_FIRE_Y0);
    matrix.pixelRoute();
    matrix.blackOut();
    // matrix.demo();
#endif
}

void loop() {
    webSocket.loop();
#if USE_MASK
    mask.loop();
#endif
#if USE_MATRIX
    matrix.loop();
#endif
}

