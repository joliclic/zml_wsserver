#include "ZML_Mask.h"

ZML_Mask::ZML_Mask
(uint8_t aLayoutRows, uint8_t aLayoutWidth, const int8_t *aLedLayout,
 uint16_t aNbPixel, uint8_t aPin, neoPixelType aPixelType) :
    _MAX_VARIABLE_DELAY(2000), // ms
    _ledLayout(aLedLayout),
    _pixels(aNbPixel, aPin, NEO_GRB | NEO_KHZ800),
    COLOR_BLACK(_pixels.Color(0, 0, 0)),
    COLOR_PURPLE(_pixels.Color(169, 0, 255)),
    COLOR_ORANGE(_pixels.Color(255, 130, 0)),
    COLOR_DARK_ORANGE(_pixels.Color(128, 75, 0)),
    COLOR_HEART1(_pixels.Color(153, 0, 15)),
    COLOR_HEART2(_pixels.Color(184, 0, 18)),
    _NB_PIXELS(aNbPixel),
    _NB_LED_GROUPS(aLayoutRows),
    _NB_LED_MAX_PER_GROUP(aLayoutWidth),
    _last_led_of_group(new uint8_t[_NB_LED_GROUPS]),
    _nextActionTime(-1),
    _nextSupraTime(-1),
    _curStep(0),
    _currentColor(COLOR_PURPLE),
    _lastColor(COLOR_BLACK), // MUST be different than _currentColor
    _TINT2RGB_S(1),
    _TINT2RGB_L(0.5),
    _stepFactorHeart(10),
    _lastStepFactorHeart(10),
    _lastStepHeart(0),
    _blinkDelay(400),
    _chaseDelay(166),
    _doubleChaseDelay(166)
    // _chaseLastILed(new int8_t[_NB_LED_GROUPS]),
    // _doubleChaseDir(new uint8_t[_NB_LED_GROUPS])
    {
        _chaseLastILed = new int8_t[_NB_LED_GROUPS];
        _doubleChaseDir = new uint8_t[_NB_LED_GROUPS];
        _last_led_of_group = new uint8_t[_NB_LED_GROUPS];
        uint8_t iled = -1;
        for (uint8_t i = 0; i < _NB_LED_GROUPS; i++) {
            iled = -1;
            uint8_t j = 0;
            while (j < _NB_LED_MAX_PER_GROUP && _getLedLayoutValue(i, j) >= 0) {
                iled++;
                j++;
            }
            _last_led_of_group[i] = iled;
        }
        
        _currentAction = &ZML_Mask::blackOut;
        _currentSupraAction = &ZML_Mask::_nullAction;
        _pixels.begin();
        _initTint2rgb();
    }

ZML_Mask::~ZML_Mask() {}

uint32_t ZML_Mask::Color(uint8_t aR, uint8_t aG, uint8_t aB) {
    return Adafruit_NeoPixel::Color(aR, aG, aB);
}

void ZML_Mask::yo(void) const {
    ZML_MASK_SERIAL.println("ZML_Mask says Yo !");
}

void ZML_Mask::loop(void) {
    if (_nextSupraTime != -1) {
        if (_nextSupraTime <= millis()) {
            (this->*_currentSupraAction)();
        }
    } else {
        if (_nextActionTime != -1 && _nextActionTime <= millis()) {
            (this->*_currentAction)();
        }
    }
}

void ZML_Mask::_nullAction(void) {}

long ZML_Mask::setDelay(long aDelay) {
    if (aDelay < 0)
        _nextActionTime = -1;
    else
        _nextActionTime = millis() + aDelay;
    
    return _nextActionTime;
}

long ZML_Mask::setSupraDelay(long aDelay) {
    if (aDelay < 0)
        _nextSupraTime = -1;
    else
        _nextSupraTime = millis() + aDelay;
    
    return _nextSupraTime;
}

long ZML_Mask::getNextActionTime(void) const {
    return _nextActionTime;
}

int8_t ZML_Mask::_getLedLayoutValue(uint8_t aX, uint8_t aY) const {
    return _ledLayout[aX * _NB_LED_MAX_PER_GROUP + aY];
}

void ZML_Mask::_printLedLayoutData() const {
    ZML_MASK_SERIAL.printf("led layout: number of group: %d\n", _NB_LED_GROUPS);
    ZML_MASK_SERIAL.printf("led layout - max number of led by group: %d\n",
                           _NB_LED_MAX_PER_GROUP);
    ZML_MASK_SERIAL.println("_ledLayout: ");
    for (uint8_t i = 0; i < _NB_LED_GROUPS; i++) {
        ZML_MASK_SERIAL.print("{");;;
        
        for (uint8_t j = 0; j < _NB_LED_MAX_PER_GROUP; j++) {
            ZML_MASK_SERIAL.printf("%d", _getLedLayoutValue(i, j));
            if (j < _NB_LED_MAX_PER_GROUP - 1)
                ZML_MASK_SERIAL.print(", ");
        }
        ZML_MASK_SERIAL.println("}");
    }
    ZML_MASK_SERIAL.print("_last_led_of_group: {");
    for (uint8_t i = 0; i < _NB_LED_GROUPS; i++) {
        ZML_MASK_SERIAL.printf("%d", _last_led_of_group[i]);
        if (i < _NB_LED_GROUPS - 1)
            ZML_MASK_SERIAL.print(", ");
    }
    ZML_MASK_SERIAL.println("}");
    ZML_MASK_SERIAL.println();
}

uint32_t ZML_Mask::setColor(uint32_t aColor) {
    _currentColor = aColor;
}

uint32_t ZML_Mask::setColor(uint8_t aR, uint8_t aG, uint8_t aB) {
    _currentColor = _pixels.Color(aR, aG, aB);
}

void ZML_Mask::doBlackOut(void) {
    _currentAction = &ZML_Mask::blackOut;
    blackOut();
    setDelay(-1);
}

void ZML_Mask::blackOut(void) {
    _lastColor = COLOR_BLACK;
    showAllPixels(0);
}

void ZML_Mask::helloPixels(void) {
    showAllPixels(100, 100, 100);
    delay(100);
    blackOut();
    ZML_MASK_SERIAL.print("Leds should have blinked\n");
}

void ZML_Mask::showAllPixels(uint32_t aColor) {
    for (uint8_t i = 0; i < _NB_PIXELS; i++) {
        _pixels.setPixelColor(i, aColor);
    }
    _pixels.show();
}

void ZML_Mask::showAllPixels(uint8_t aR, uint8_t aG, uint8_t aB) {
    showAllPixels(_pixels.Color(aR, aG, aB));
}

void ZML_Mask::doContinuous(void) {
    _currentAction = &ZML_Mask::continuous;
    continuous();
}

void ZML_Mask::continuous(void) {
    if (_currentColor != _lastColor) {
        showAllPixels(_currentColor);
        _lastColor = _currentColor;
    }
    setDelay(ZML_MASK_BIG_TICK);
}

uint8_t ZML_Mask::_setSpeedDivisor(uint8_t aDivisor) const {
    if (aDivisor > ZML_MASK_MAX_SPEED_DIVISOR)
        aDivisor = ZML_MASK_MAX_SPEED_DIVISOR;
    
    return aDivisor;
}

uint16_t ZML_Mask::setBlinkSpeed(uint8_t aDivisor) {
    aDivisor = _setSpeedDivisor(aDivisor);
    if (aDivisor == 0)
        _blinkDelay = -1;
    else
        _blinkDelay = _MAX_VARIABLE_DELAY / aDivisor;
}

uint16_t ZML_Mask::setChaseSpeed(uint8_t aDivisor) {
    aDivisor = _setSpeedDivisor(aDivisor);
    if (aDivisor == 0)
        _chaseDelay = -1;
    else
        _chaseDelay = _MAX_VARIABLE_DELAY / aDivisor;
}

uint16_t ZML_Mask::setDoubleChaseSpeed(uint8_t aDivisor) {
    aDivisor = _setSpeedDivisor(aDivisor);
    if (aDivisor == 0)
        _doubleChaseDelay = -1;
    else
        _doubleChaseDelay = _MAX_VARIABLE_DELAY / aDivisor;
}

void ZML_Mask::doBlink(void) {
    _curStep = 0;
    _currentAction = &ZML_Mask::blink;
    blink();
}

void ZML_Mask::blink(void) {
    switch (_curStep) {
        case 0:
            showAllPixels(_currentColor);
            setDelay(_blinkDelay);
            break;
        case 1:
            blackOut();
            setDelay(_blinkDelay);
            break;
    }
    _curStep++;
    if (_curStep > 1)
        _curStep = 0;
}

void ZML_Mask::doChase(void) {
    blackOut();
    _currentAction = &ZML_Mask::chase;
    for (uint8_t i = 0; i < _NB_LED_GROUPS; i++) {
        _chaseLastILed[i] = -1;
    }
    chase();
}

void ZML_Mask::chase(void) {
    for (uint8_t i = 0; i < _NB_LED_GROUPS; i++) {
        if (_chaseLastILed[i] >= 0)
            _pixels.setPixelColor(_getLedLayoutValue(i, _chaseLastILed[i]), 0);
        
        _chaseLastILed[i]++;
        if (_chaseLastILed[i] >= _last_led_of_group[i])
            _chaseLastILed[i] = 0;
        _pixels.setPixelColor(_getLedLayoutValue(i, _chaseLastILed[i]),
                              _currentColor);
    }
    
    _pixels.show();
    setDelay(_chaseDelay);
}

void ZML_Mask::doDoubleChase(void) {
    blackOut();
    _currentAction = &ZML_Mask::doubleChase;
    for (uint8_t i = 0; i < _NB_LED_GROUPS; i++) {
        _chaseLastILed[i] = -1;
        _doubleChaseDir[i] = 0;
    }
    doubleChase();
}

void ZML_Mask::doubleChase(void) {
    for (uint8_t i = 0; i < _NB_LED_GROUPS; i++) {
        if (_chaseLastILed[i] >= 0)
            _pixels.setPixelColor(_getLedLayoutValue(i, _chaseLastILed[i]), 0);
        
        if (_doubleChaseDir[i] == 0) {
            _chaseLastILed[i]++;
            if (_chaseLastILed[i] >= _last_led_of_group[i]) {
                _doubleChaseDir[i] = -1;
                _chaseLastILed[i]++;
                if (_last_led_of_group[i] > 1)
                    _chaseLastILed[i]++;
            }
        } else {
            _chaseLastILed[i]--;
            if (_chaseLastILed[i] < 0) {
                _doubleChaseDir[i] = 0;
                if (_last_led_of_group[i] > 1)
                    _chaseLastILed[i] = 1;
                else
                    _chaseLastILed[i] = 0;
            }
        }
        _pixels.setPixelColor(_getLedLayoutValue(i, _chaseLastILed[i]),
                              _currentColor);
    }
    _pixels.show();
    setDelay(_doubleChaseDelay);
}

void ZML_Mask::_setStepFactorGB4(uint8_t aFactor, uint8_t &aStepFactor) {
    if (aFactor == aStepFactor)
        return;
    
    if (aFactor < 1)
        aStepFactor = 1;
    else if (aFactor > ZML_MASK_MAX_STEP_FACTOR_GB4)
        aStepFactor = ZML_MASK_MAX_STEP_FACTOR_GB4;
    else
        aStepFactor = aFactor;
}

void ZML_Mask::_gradientsBy4
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
        color = _pixels.Color(r, g, b);
    }
    showAllPixels(color);
}

uint8_t ZML_Mask::setHeartStepFactor(uint8_t aFactor) {
    _setStepFactorGB4(aFactor, _stepFactorHeart);
    
    return _stepFactorHeart;
}

void ZML_Mask::doHeart() {
    _lastStepHeart = 0;
    _currentAction = &ZML_Mask::heart;
    heart();
}

void ZML_Mask::heart() {
    _gradientsBy4(COLOR_BLACK, 1, COLOR_HEART1, 1, COLOR_BLACK, 1, COLOR_HEART2,
                  3, _lastStepHeart, _stepFactorHeart, _lastStepFactorHeart);
   setDelay(ZML_MASK_HEART_TICK);
}

void ZML_Mask::_initTint2rgb(void) {
    if (_TINT2RGB_L < 0.5)
        _TINT2RGB_V2  = _TINT2RGB_L * (1 + _TINT2RGB_S);
    else
        _TINT2RGB_V2  = _TINT2RGB_L + _TINT2RGB_S - (_TINT2RGB_L * _TINT2RGB_S);
    
    _TINT2RGB_V1 = (2 * _TINT2RGB_L - _TINT2RGB_V2);
}

double ZML_Mask::_h2rgb(double aV1, double aV2, uint8_t aH) {
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

uint32_t ZML_Mask::_tint2rgb(uint16_t aTint) {
    if (aTint > 360)
        aTint = 360;
    
    // simplified hsl to rgb conversion, because s and l are fixed and > 0
    
    uint8_t hr = aTint / 60;
    uint8_t r = (uint8_t) (255 * _h2rgb(_TINT2RGB_V1, _TINT2RGB_V2, (hr + 2)));
    uint8_t g = (uint8_t) (255 * _h2rgb(_TINT2RGB_V1, _TINT2RGB_V2, hr));
    uint8_t b = (uint8_t) (255 * _h2rgb(_TINT2RGB_V1, _TINT2RGB_V2, (hr - 2)));
    
    // Serial.printf("r: %d, g: %d, b: %d", r, g, b);
    return _pixels.Color(r, g, b);
}

void ZML_Mask::paintRandomColors(void) {
    uint32_t color;
    for (uint8_t i = 0; i < _NB_PIXELS; i++) {
        color = _tint2rgb(random(0, 360));
        _pixels.setPixelColor(i, color);
    }
    _pixels.show();
    _currentAction = &ZML_Mask::paintRandomColors;
    setDelay(-1);
}

void ZML_Mask::flash(void) {
    showAllPixels(ZML_MASK_FLASH_POWER, ZML_MASK_FLASH_POWER,
                  ZML_MASK_FLASH_POWER);
    setSupraDelay(ZML_MASK_FLASH_DELAY);
    _currentSupraAction = &ZML_Mask::_stopFlash;
}

void ZML_Mask::_stopFlash(void) {
    showAllPixels(0);
    setSupraDelay(-1);
    _currentSupraAction = &ZML_Mask::_nullAction;
    if (_currentAction == &ZML_Mask::continuous) {
        showAllPixels(_currentColor);
    } else if (_currentAction == &ZML_Mask::paintRandomColors) {
        paintRandomColors();
    }
}
