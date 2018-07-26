#ifndef ZML_MATRIX
#define ZML_MATRIX

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define ZML_MATRIX_SERIAL Serial

#define ZML_MATRIX_FAST_TICK 15

#define ZML_MATRIX_SCROLLTEXT_MAX_DELAY 1000
#define ZML_MATRIX_SCROLLTEXT_MAX_DIV 20
#define ZML_MATRIX_SCROLLTEXT_MAX_LENGTH 128

#define ZML_MATRIX_FIRE_TYPE_NATURAL 1
#define ZML_MATRIX_FIRE_TYPE_PURPLE 2
#define ZML_MATRIX_FIRE_TYPE_PURPLE2 3

#define ZML_MATRIX_RANDPAINT_TICK 60
#define ZML_MATRIX_FILLCIRCLE_TICK 100
#define ZML_MATRIX_ZIGZAG_TICK 500
#define ZML_MATRIX_RVL_TICK 30
#define ZML_MATRIX_RHL_TICK 30
#define ZML_MATRIX_FIRE_TICK 5

#define ZML_MATRIX_ZIGZAG_TRGLEWIDTH 5

#define ZML_MATRIX_FIRE_WIDTH 8
#define ZML_MATRIX_FIRE_HEIGHT 8
#define ZML_MATRIX_FIRE_X0 0
#define ZML_MATRIX_FIRE_Y0 7

#define ZML_MATRIX_FIRE_TYPE_NATURAL 1
#define ZML_MATRIX_FIRE_TYPE_PURPLE 2
#define ZML_MATRIX_FIRE_TYPE_PURPLE2 3

class ZML_Matrix {
  public:
    
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
    // ZML_Matrix(uint8_t aWidth, uint8_t aHeight, uint8_t aPin,
            //    uint8_t aMatrixType, neoPixelType aLedType, uint8_t aEmptyPin);
    ZML_Matrix(int aWidth, int aHeight, uint8_t aPin, uint8_t aMatrixType,
               neoPixelType aLedType, uint8_t aEmptyPin);
    ~ZML_Matrix();
    
    const uint16_t
        COLOR_BLACK,
        COLOR_WHITE,
        COLOR_PURPLE,
        COLOR_RED,
        COLOR_GREEN,
        COLOR_BLUE,
        COLOR_ZML_GREEN,
        COLOR_ZML_BLUE,
        COLOR_ORANGE,
        COLOR_DARK_ORANGE,
        COLOR_DARK_ORANGE2;
    
    long
        getNextActionTime(void) const,
        setDelay(long aDelay), // should be private
        setMetaDelay(long aDelay); // should be private
    
    void
        loop(void),
        // demo(void),
        blackOut(void),
        showAllPixels(uint32_t aColor),
        showAllPixels(uint8_t aR, uint8_t aG, uint8_t aB),
        pixelRoute(void),
        pixelRoute(uint32_t aColor),
        matrixRoute(void),
        matrixRoute(uint16_t aColor),
        // setScrolledText(char aTxt[]),
        setScrolledText(char* aTxt),
        // scrollText(String aTxt, uint16_t aTxtColor, uint16_t aFgColor),
        setScrolledTxtColor(uint16_t aColor),
        setScrolledTxtFgColor(uint16_t aColor),
        setScrolledTxtY(uint8_t aY),
        scrollText(void),
        // scrollText(char aTxt[], uint16_t aTxtColor, uint16_t aFgColor),
        // scrollText(char aTxt[], uint16_t aTxtColor, uint16_t aFgColor,
        scrollText(char* aTxt, uint16_t aTxtColor, uint16_t aFgColor),
        scrollText(char* aTxt, uint16_t aTxtColor, uint16_t aFgColor,
                   uint8_t aY),
        fillRandom(void),
        fillWithCircles(void),
        setZigZagY(uint8_t aY),
        fixedZigzag(void),
        rel_zigzag(int8_t x0),
        zigzag(void),
        longZigzag(void),
        animatedVerticalLine(uint8_t aX, uint16_t aColor),
        drawRainbowVLines(void),
        drawRainbowHLines(void),
        fire(void),
        fire(uint8_t aType),
        setFirePosition(uint8_t aX, uint8_t aY),
        setScrollTextSpeed(uint8_t aDivisor),
        stopMetaAction(void);
        
    uint16_t
        HSVtoRGB(uint8_t ih, uint8_t is, uint8_t iv);
        

  private:
    Adafruit_NeoPixel _pixels;
    Adafruit_NeoMatrix _matrix;
    
    void (ZML_Matrix::*_currentAction)(void);
    void (ZML_Matrix::*_currentMetaAction)(void);
    
    const uint8_t
        _WIDTH,
        _HEIGHT,
        _CIRCLE_NB_COLOR;
    
    const uint16_t
        _NB_PIXELS;
    
    uint8_t
        _curCircleColorI,
        _zigzagX,
        _zigzagY,
        _rvlStep,
        _rhlStep,
        _fireX0,
        _fireY0,
        _fireMatrix[ZML_MATRIX_FIRE_HEIGHT][ZML_MATRIX_FIRE_WIDTH],
        _fireLine[ZML_MATRIX_FIRE_WIDTH],
        _fire_pcnt,
        _curFireType;
    
    uint16_t
        _scrolledTxtTick,
        _scrolledTxtColor,
        _scrolledTxtFgColor,
        *_randIndexes,
        _curRandI,
        // _curCircleColor,
        _curCircleR,
        _curCircleX,
        _curCircleY,
        _maxCircleR,
        _rvlColor,
        _rvlHue,
        _rhlColor,
        _rhlHue;
    
    long
        _nextActionTime,
        _nextMetaTime;
    
    uint16_t
        _circleColors[3];
    
    byte _lastCharBuf;
    
    // char _scrolledTxt[];
    char *_scrolledTxt;
    
    int8_t _zigzagNbTriangle;
    
    int16_t
        _scrolledTxtX,
        _scrolledTxtY,
        _scrolledTxtMaxX,
        _rvlX,
        _rvlY,
        _rhlX,
        _rhlY;
    
    byte _utf8ascii(byte aAscii);
    
    //String _utf8ascii(String aS);
    
    void
        _utf8ascii(char* aS),
        _scrollTextFrame(void),
        _fillRandomStep(void),
        _fillWithCircles(void),
        _fillWithCirclesFrame(void),
        _zigzagStep(void),
        _drawRainbowVLines(void),
        _drawRainbowHLines(void),
        _generateFireLine(void),
        _shiftUpFire(void),
        _drawFireFrame(uint8_t pcnt),
        _drawFireFrame(uint8_t pcnt, uint8_t aType),
        _fire (void);
    
    int8_t
        _getZigZagNbTriangles(void);
    
    int16_t
        _getMaxCircleR(void);
};

#endif