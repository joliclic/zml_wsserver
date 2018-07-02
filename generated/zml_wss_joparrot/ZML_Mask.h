#ifndef ZML_MASK
#define ZML_MASK

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define ZML_MASK_SERIAL Serial

#define ZML_MASK_BIG_TICK 42

#define ZML_MASK_HEART_TICK 42

#define ZML_MASK_MAX_SPEED_DIVISOR 50

#define ZML_MASK_MAX_STEP_FACTOR_GB4 100

class ZML_Mask {
  public:
    
    ZML_Mask(uint8_t aLayoutRows, uint8_t aLayoutWidth,
             const int8_t *aLedLayout, uint16_t aNbPixel, uint8_t aPin,
             neoPixelType aPixelType = NEO_GRB | NEO_KHZ800);
    ~ZML_Mask();
    
    const uint32_t
        COLOR_BLACK,
        COLOR_PURPLE,
        COLOR_ORANGE,
        COLOR_DARK_ORANGE,
        COLOR_HEART1,
        COLOR_HEART2;
    
    long
        getNextActionTime(void) const,
        setDelay(long aDelay);
    
    static uint32_t Color(uint8_t aR, uint8_t aG, uint8_t aB);
    
    void yo(void) const;
    
    void
        loop(void),
        doBlackOut(void),
        blackOut(void),
        helloPixels(void),
        showAllPixels(uint32_t aColor),
        showAllPixels(uint8_t aR, uint8_t aG, uint8_t aB),
        doContinuous(void),
        continuous(void),
        doBlink(void),
        blink(void),
        doChase(void),
        chase(void),
        doDoubleChase(void),
        doubleChase(void),
        doHeart(),
        heart(),
        paintRandomColors();

    uint8_t
        setHeartStepFactor(uint8_t aFactor);
    
    uint16_t
        setBlinkSpeed(uint8_t aDivisor),
        setChaseSpeed(uint8_t aDivisor),
        setDoubleChaseSpeed(uint8_t aDivisor);
    
    uint32_t
        setColor(uint32_t aColor),
        setColor(uint8_t aR, uint8_t aG, uint8_t aB);
    
  private:
        const uint16_t _MAX_VARIABLE_DELAY;
        
        const double
            _TINT2RGB_S,
            _TINT2RGB_L;
        
        Adafruit_NeoPixel _pixels;
        
        void (ZML_Mask::*_currentAction)(void);
    
        const int8_t *_ledLayout;
        
        uint8_t
            *_last_led_of_group, // simili constant > uppercase
            *_doubleChaseDir; // 0: up, 1: down
        
        int8_t *_chaseLastILed;
        
        const uint8_t
            _NB_PIXELS,
            _NB_LED_GROUPS,
            _NB_LED_MAX_PER_GROUP;
        
        uint8_t
            _stepFactorHeart,
            _lastStepFactorHeart;
        
        uint16_t
            _blinkDelay,
            _chaseDelay,
            _doubleChaseDelay,
            _lastStepHeart;
        
        uint32_t
            _curStep,
            _currentColor,
            _lastColor;
        
        long _nextActionTime;
        
        double
            _TINT2RGB_V1,
            _TINT2RGB_V2;
        
        void
        //     initLedLayoutData(),
            _initTint2rgb(void),
            _printLedLayoutData() const,
            _setStepFactorGB4(uint8_t aFactor, uint8_t &aStepFactor),
            _gradientsBy4
                (uint32_t aColor1, uint8_t aNbSteps1, uint32_t aColor2,
                uint8_t aNbSteps2, uint32_t aColor3, uint8_t aNbSteps3,
                uint32_t aColor4, uint8_t aNbSteps4, uint16_t &aLastStep,
                uint8_t &aStepFactor, uint8_t &aLastStepFactor);

        
        uint8_t
            _setSpeedDivisor(uint8_t aDivisor) const;
        
        int8_t
            _getLedLayoutValue(uint8_t aX, uint8_t aY) const;
        
        static double
            _h2rgb(double aV1, double aV2, uint8_t aH);
        
        uint32_t
            _tint2rgb(uint16_t aTint);
};

#endif