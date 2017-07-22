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
#include "wifi_host_config.h"
#include "leds_layout.h"

#define USE_SERIAL Serial

#define CMD_PIN D2

#define BIG_TICK 42

#define HEART_TICK 42

#define MAX_SPEED_DIVISOR 50

//ESP8266WiFiMulti WiFiMulti;

WebSocketsServer webSocket = WebSocketsServer(81);

Adafruit_NeoPixel pixels(NUM_PIXELS, CMD_PIN, NEO_GRB | NEO_KHZ800);

uint8_t *gLAST_LED_OF_GROUP;

void (*gCurrentAction)();

long gNextActionTime = -1;

unsigned int gCurStep = 0;

const uint32_t COLOR_BLACK = pixels.Color(0, 0, 0);
const uint32_t COLOR_PURPLE = pixels.Color(169, 0, 255);
const uint32_t COLOR_ORANGE = pixels.Color(255, 130, 0);

//uint8_t gColorR, gColorG, gColorB;
//uint32_t gColor = pixels.Color(0, 0, 0);
uint32_t gColor = COLOR_PURPLE;

// gLastColor MUST be different than gColor
uint32_t gLastColor = COLOR_BLACK;


const uint16_t MAX_VARIABLE_DELAY = 2000; // ms
uint16_t gVariableBlinkDelay = 1000; //ms
uint16_t gVariableChaseDelay = 2000;
uint16_t gVariableDChaseDelay = 2000;

void initLedLayoutData() {
    gLAST_LED_OF_GROUP = new uint8_t[NB_LED_GROUPS];
    
    uint8_t iled = -1;
    for (uint8_t i = 0; i < NB_LED_GROUPS; i++) {
        iled = -1;
        uint8_t j = 0;
        while (j < NB_LED_MAX_PER_GROUP && LEDS_LAYOUT[i][j] >= 0) {
            iled++;
            j++;
        }
        gLAST_LED_OF_GROUP[i] = iled;
    } 
}

void printLedLayoutData() {
    USE_SERIAL.printf("led layout: number of group: %d\n", NB_LED_GROUPS);
    USE_SERIAL.printf("led layout - max number of led by group: %d\n",
                      NB_LED_MAX_PER_GROUP);
    USE_SERIAL.println("LEDS_LAYOUT: ");
    for (uint8_t i = 0; i < NB_LED_GROUPS; i++) {
        USE_SERIAL.print("{");
        for (uint8_t j = 0; j < NB_LED_MAX_PER_GROUP; j++) {
            USE_SERIAL.printf("%d", LEDS_LAYOUT[i][j]);
            if (j < NB_LED_MAX_PER_GROUP - 1)
                USE_SERIAL.print(", ");
        }
        USE_SERIAL.println("}");
    }
    USE_SERIAL.print("gLAST_LED_OF_GROUP: {");
    for (uint8_t i = 0; i < NB_LED_GROUPS; i++) {
        USE_SERIAL.printf("%d", gLAST_LED_OF_GROUP[i]);
        if (i < NB_LED_GROUPS - 1)
            USE_SERIAL.print(", ");
    }
    USE_SERIAL.println("}");
    USE_SERIAL.println();
}

void showAllPixels(uint32_t aColor) {
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        pixels.setPixelColor(i, aColor);
    }
    pixels.show();
}

void showAllPixels(uint8_t aR, uint8_t aG, uint8_t aB) {
    showAllPixels(pixels.Color(aR, aG, aB));
}

void blackOut() {
    showAllPixels(0);
}

void setDelay(long aDelay) {
    if (aDelay < 0)
        gNextActionTime = -1;
    else
        gNextActionTime = millis() + aDelay;
}

void doContinuous() {
    gCurrentAction = &continuous;
    continuous();
}

void continuous() {
    if (gColor != gLastColor) {
        showAllPixels(gColor);
        gLastColor = gColor;
    }
    setDelay(BIG_TICK);
}

int setSpeedDivisor(uint8_t aDivisor) {
    if (aDivisor > MAX_SPEED_DIVISOR)
        aDivisor = MAX_SPEED_DIVISOR;
    
    return aDivisor;
}

int setVariableBlinkSpeed(uint8_t aDivisor) {
    aDivisor = setSpeedDivisor(aDivisor);
    if (aDivisor == 0)
        gVariableBlinkDelay = -1;
    else
        gVariableBlinkDelay = MAX_VARIABLE_DELAY / aDivisor;
}

int setVariableChaseSpeed(uint8_t aDivisor) {
    aDivisor = setSpeedDivisor(aDivisor);
    if (aDivisor == 0)
        gVariableChaseDelay = -1;
    else
        gVariableChaseDelay = MAX_VARIABLE_DELAY / aDivisor;
}

int setVariableDoubleChaseSpeed(uint8_t aDivisor) {
    aDivisor = setSpeedDivisor(aDivisor);
    if (aDivisor == 0)
        gVariableDChaseDelay = -1;
    else
        gVariableDChaseDelay = MAX_VARIABLE_DELAY / aDivisor;
}

void helloPixels() {
    showAllPixels(100, 100, 100);
    delay(100);
    blackOut();
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
            showAllPixels(gColor);
            setDelay(gVariableBlinkDelay);
            break;
        case 1:
            blackOut();
            setDelay(gVariableBlinkDelay);
            break;
    }
    gCurStep++;
    if (gCurStep > 1)
        gCurStep = 0;
}

//int16_t gChaseLastLedOn = -1;
int8_t *gChaseLastILed;

void doChase() {
    blackOut();
    gCurrentAction = &chase;
    //gChaseLastLedOn = -1;
    for (uint8_t i = 0; i < NB_LED_GROUPS; i++) {
        gChaseLastILed[i] = -1;
    }
    chase();
}

void chase() {
    for (uint8_t i = 0; i < NB_LED_GROUPS; i++) {
        if (gChaseLastILed[i] >= 0)
            pixels.setPixelColor(LEDS_LAYOUT[i][gChaseLastILed[i]], 0);
        
        gChaseLastILed[i]++;
        if (gChaseLastILed[i] >= gLAST_LED_OF_GROUP[i])
            gChaseLastILed[i] = 0;
        pixels.setPixelColor(LEDS_LAYOUT[i][gChaseLastILed[i]], gColor);
    }
    
    pixels.show();
    setDelay(gVariableChaseDelay);
}

uint8_t *gDoubleChaseDir; // 0: up, black1: down

void doDoubleChase() {
    blackOut();
    gCurrentAction = &doubleChase;
    for (uint8_t i = 0; i < NB_LED_GROUPS; i++) {
        gChaseLastILed[i] = -1;
        gDoubleChaseDir[i] = 0;
    }
    doubleChase();
}

void doubleChase() {
    for (uint8_t i = 0; i < NB_LED_GROUPS; i++) {
        if (gChaseLastILed[i] >= 0)
            pixels.setPixelColor(LEDS_LAYOUT[i][gChaseLastILed[i]], 0);
        
        if (gDoubleChaseDir[i] == 0) {
            gChaseLastILed[i]++;
            if (gChaseLastILed[i] >= gLAST_LED_OF_GROUP[i]) {
                gDoubleChaseDir[i] = -1;
                gChaseLastILed[i]++;
                if (gLAST_LED_OF_GROUP[i] > 1)
                    gChaseLastILed[i]++;
            }
        } else {
            gChaseLastILed[i]--;
            if (gChaseLastILed[i] < 0) {
                gDoubleChaseDir[i] = 0;
                if (gLAST_LED_OF_GROUP[i] > 1)
                    gChaseLastILed[i] = 1;
                else
                    gChaseLastILed[i] = 0;
            }
        }
        pixels.setPixelColor(LEDS_LAYOUT[i][gChaseLastILed[i]], gColor);
    }
    pixels.show();
    setDelay(gVariableDChaseDelay);
}

#define MAX_STEP_FACTOR_GB4 100

void setStepFactorGB4(uint8_t aFactor, uint8_t &aStepFactor) {
    if (aFactor == aStepFactor)
        return;
    
    if (aFactor < 1)
        aStepFactor = 1;
    else if (aFactor > MAX_STEP_FACTOR_GB4)
        aStepFactor = MAX_STEP_FACTOR_GB4;
    else
        aStepFactor = aFactor;
}

void gradientsBy4
(uint32_t aColor1, uint8_t aNbSteps1, uint32_t aColor2, uint8_t aNbSteps2,
 uint32_t aColor3, uint8_t aNbSteps3, uint32_t aColor4, uint8_t aNbSteps4,
 uint16_t &aLastStep, uint8_t &aStepFactor, uint8_t &aLastStepFactor) {
    
    if (aStepFactor != aLastStepFactor) {
        aLastStep = aLastStep * aStepFactor / aLastStepFactor;
        aLastStepFactor = aStepFactor;
    }
    
    aLastStep++;
    uint16_t step = aLastStep;
    uint32_t color, color1, color2;
    uint8_t nbSteps;
    if (step >= (aNbSteps1 + aNbSteps2 + aNbSteps3 + aNbSteps4) * aStepFactor) {
        step = 0;
        aLastStep = 0;
    }
    if (step < aNbSteps1 * aStepFactor) {
        color1 = aColor1;
        color2 = aColor2;
        nbSteps = aNbSteps1 * aStepFactor;
    } else if (step < (aNbSteps1 + aNbSteps2) * aStepFactor) {
        color1 = aColor2;
        color2 = aColor3;
        step = step - aNbSteps1 * aStepFactor;
        nbSteps = aNbSteps2 * aStepFactor;
    } else if (step < (aNbSteps1 + aNbSteps2 + aNbSteps3) * aStepFactor) {
        color1 = aColor3;
        color2 = aColor4;
        step = step - (aNbSteps1 + aNbSteps2) * aStepFactor;
        nbSteps = aNbSteps3 * aStepFactor;
    } else if (step < (aNbSteps1 + aNbSteps2 + aNbSteps3 + aNbSteps4) * aStepFactor) {
        color1 = aColor4;
        color2 = aColor1;
        step = step - (aNbSteps1 + aNbSteps2 + aNbSteps3) * aStepFactor;
        nbSteps = aNbSteps4 * aStepFactor;
    }
    
    if (step == 0) {
        color = color1;
    } else {
        uint8_t r1 = (uint8_t) (color1 >> 16);
        uint8_t g1 = (uint8_t) (color1 >> 8);
        uint8_t b1 = (uint8_t) color1;
        uint8_t r2 = (uint8_t) (color2 >> 16);
        uint8_t g2 = (uint8_t) (color2 >> 8);
        uint8_t b2 = (uint8_t) color2;
        
        uint8_t r = r1 + step * (r2 - r1) / (nbSteps + 2);
        uint8_t g = g1 + step * (g2 - g1) / (nbSteps + 2);
        uint8_t b = b1 + step * (b2 - b1) / (nbSteps + 2);
        color = pixels.Color(r, g, b);
    }
    showAllPixels(color);
}

uint16_t gLastStepHeart = 0;
uint8_t gStepFactorHeart = 1;
uint8_t gLastStepFactorHeart = 1;
uint32_t gColorBlack = pixels.Color(0, 0, 0);
uint32_t gColorHeart1 = pixels.Color(153, 0, 36);
uint32_t gColorHeart2 = pixels.Color(204, 0, 48);

void setHeartStepFactor(uint8_t aFactor) {
    setStepFactorGB4(aFactor, gStepFactorHeart);
}

void doHeart() {
    gLastStepHeart = 0;
    gCurrentAction = &heart;
    heart();
}

void heart() {
    gradientsBy4(gColorBlack, 1, gColorHeart1, 1, gColorBlack, 1, gColorHeart2,
                 3, gLastStepHeart, gStepFactorHeart, gLastStepFactorHeart);
    setDelay(HEART_TICK);
}

double h2rgb(double aV1, double aV2, uint8_t aH) {
    if (aH < 0) aH += 6;
    if (aH > 6) aH -= 6;
    
    if (aH < 1)
        return aV1 + (aV2 - aV1) * aH;
    if (aH < 3)
        return aV2;
    if (aH < 4)
        return aV1 + (aV2 - aV1) * (4 - aH);
    
    return aV1;
}

const double TINT2RGB_S = 1;
const double TINT2RGB_L = 0.5;
double TINT2RGB_V1, TINT2RGB_V2;

void initTint2rgb() {
    if (TINT2RGB_L < 0.5)
        TINT2RGB_V2  = TINT2RGB_L * (1 + TINT2RGB_S);
    else
        TINT2RGB_V2  = TINT2RGB_L + TINT2RGB_S - (TINT2RGB_L * TINT2RGB_S);
    
    TINT2RGB_V1 = (2 * TINT2RGB_L - TINT2RGB_V2);
}

uint32_t tint2rgb(uint16_t aTint) {
    if (aTint > 360)
        aTint = 360;
    
    // simplified hsl to rgb conversion, because s and l are fixed and > 0
    
    uint8_t hr = aTint / 60;
    uint8_t r = (uint8_t) (255 * h2rgb(TINT2RGB_V1, TINT2RGB_V2, (hr + 2)));
    uint8_t g = (uint8_t) (255 * h2rgb(TINT2RGB_V1, TINT2RGB_V2, hr));
    uint8_t b = (uint8_t) (255 * h2rgb(TINT2RGB_V1, TINT2RGB_V2, (hr - 2)));
    
    Serial.printf("r: %d, g: %d, b: %d", r, g, b);
    return pixels.Color(r, g, b);
}

void paintRandomColors() {
    uint32_t color;
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        color = tint2rgb(random(0, 360));
        pixels.setPixelColor(i, color);
    }
    pixels.show();
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
            } else if (text == "black" || text == "blackout") {
                gCurrentAction = &blackOut;
                blackOut();
                setDelay(-1);
                USE_SERIAL.print("all leds should be turned off now...\n");
            } else if (text == "continuous") {
                doContinuous();
            } else if (text == "color:purple") {
                gColor = COLOR_PURPLE;
            } else if (text == "color:orange") {
                gColor = COLOR_ORANGE;
            } else if (text == "blink") {
                doBlink();
            } else if (text == "chase") {
                doChase();
            } else if (text == "doublechase") {
                doDoubleChase();
            } else if (text == "heart") {
                doHeart();
            } else if (text == "random") {
                USE_SERIAL.print("receive random command\n");
                paintRandomColors();
                setDelay(-1);
                USE_SERIAL.print("random pixel colors should be painted...\n");
            } else if (text_length == 12 || text_length == 13) {
                USE_SERIAL.print("text_length = 12 or 13\n");
                res = sscanf(chars_payload, "color:#%02x%02x%02x", &r, &g, &b);
                if (res == 3) {
                    USE_SERIAL.printf("found: %u, r: %u, g: %u, b: %u\n",
                                      res, r, g, b);
                    gColor = pixels.Color((uint8_t) r,
                                                 (uint8_t) g,
                                                 (uint8_t) b);
                } else {
                    res = sscanf(chars_payload, "blinkspeed:%d", &s);
                    if (res == 1) {
                        USE_SERIAL.printf("blinkspeed: %d\n", s);
                        setVariableBlinkSpeed(s);
                    } else {
                        res = sscanf(chars_payload, "chasespeed:%d", &s);
                        if (res == 1) {
                            USE_SERIAL.printf("chasespeed: %d\n", s);
                            setVariableChaseSpeed(s);
                        } else {
                            res = sscanf(chars_payload, "dchsespeed:%d", &s);
                            if (res == 1) {
                                USE_SERIAL.printf("dchsespeed: %d\n", s);
                                setVariableDoubleChaseSpeed(s);
                            } else {
                                res = sscanf(chars_payload, "heartspeed:%d", &s);
                                if (res == 1) {
                                    USE_SERIAL.printf("heartspeed: %d\n", s);
                                    // we want in fact the opposite of s
                                    setHeartStepFactor(-s);
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

    gCurrentAction = &blackOut;
    initLedLayoutData();
    gChaseLastILed = new int8_t[NB_LED_GROUPS];
    gDoubleChaseDir = new uint8_t[NB_LED_GROUPS];

    //printLedLayoutData();
    
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
    
    initTint2rgb();
    
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
    
    if (gNextActionTime != -1 && gNextActionTime <= millis()) {
        gCurrentAction();
    }
}

