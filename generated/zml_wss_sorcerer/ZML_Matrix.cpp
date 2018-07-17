#include "ZML_Matrix.h"

ZML_Matrix::ZML_Matrix
(int aWidth, int aHeight, uint8_t aPin, uint8_t aMatrixType,
 neoPixelType aLedType, uint8_t aBrightness) :
    _WIDTH(aWidth),
    _HEIGHT(aHeight),
    _NB_PIXELS(aWidth * aHeight),
    _pixels(aWidth * aHeight, aPin, NEO_GRB | NEO_KHZ800),
    _matrix(aWidth, aHeight, aPin, aMatrixType, aLedType),
    COLOR_BLACK(_matrix.Color(0, 0, 0)),
    COLOR_WHITE(_matrix.Color(180, 180, 180)),
    COLOR_PURPLE(_matrix.Color(169, 0, 255)),
    COLOR_RED(_matrix.Color(127, 0, 0)),
    COLOR_GREEN(_matrix.Color(0, 127, 0)),
    COLOR_BLUE(_matrix.Color(0, 0, 127)),
    COLOR_ZML_GREEN(_matrix.Color(123, 255, 46)),
    COLOR_ZML_BLUE(_matrix.Color(46, 157, 255)),
    COLOR_ORANGE(_matrix.Color(255, 130, 0)),
    COLOR_DARK_ORANGE(_matrix.Color(46, 24, 0)),
    COLOR_DARK_ORANGE2(_matrix.Color(128, 75, 0)),
    _nextActionTime(-1),
    _nextMetaTime(-1),
    // _scrolledTxt(F("ZYGOS POWA !")),
    _scrolledTxt("ZYGOS POWA !"),
    _scrolledTxtTick(150), // ms
    _scrolledTxtColor(COLOR_PURPLE),
    _scrolledTxtFgColor(COLOR_BLACK),
    _scrolledTxtY(0),
    _randIndexes(new uint16_t[_NB_PIXELS]),
    _curRandI(0),
    _maxCircleR(0),
    _curCircleColorI(0),
    _CIRCLE_NB_COLOR(3),
    _circleColors{COLOR_ORANGE, COLOR_BLUE, COLOR_GREEN},
    _zigzagNbTriangle(-1),
    _fireX0(ZML_MATRIX_FIRE_X0),
    _fireY0(ZML_MATRIX_FIRE_Y0),
    _fire_pcnt(0),
    _curFireType(1)
    {
        _pixels.begin();
        _pixels.setBrightness(50);
        _matrix.begin();
        _matrix.setTextWrap(false);
        _matrix.setBrightness(50);
        blackOut();
    }

ZML_Matrix::~ZML_Matrix() {
    delete(_randIndexes);
}

void ZML_Matrix::loop(void) {
    if (_nextMetaTime != -1 && _nextMetaTime <= millis()) {
        (this->*_currentMetaAction)();
    }
    if (_nextActionTime != -1 && _nextActionTime <= millis()) {
        (this->*_currentAction)();
    }
}

long ZML_Matrix::setMetaDelay(long aDelay) {
    if (aDelay < 0)
        _nextMetaTime = -1;
    else
        _nextMetaTime = millis() + aDelay;
    
    return _nextMetaTime;
}

long ZML_Matrix::setDelay(long aDelay) {
    if (aDelay < 0)
        _nextActionTime = -1;
    else
        _nextActionTime = millis() + aDelay;
    
    return _nextActionTime;
}

long ZML_Matrix::getNextActionTime(void) const {
    return _nextActionTime;
}

void ZML_Matrix::pixelRoute(void) {
    pixelRoute(COLOR_PURPLE);
}

void ZML_Matrix::pixelRoute(uint32_t aColor) {
// void ZML_Matrix::pixelRoute(uint16_t aColor) {
    for (uint16_t i = 0; i < _NB_PIXELS; i++) {
        // ZML_MATRIX_SERIAL.printf("%d, ", i);
        // _pixels.setPixelColor(i, aColor);
        _pixels.setPixelColor(i, _pixels.Color(127, 0, 0));
        _pixels.show();
        delay(15);
    }
    ZML_MATRIX_SERIAL.println();
}

void ZML_Matrix::matrixRoute(void) {
    matrixRoute(COLOR_BLUE);
}

void ZML_Matrix::matrixRoute(uint16_t aColor) {
    for (uint8_t i = 0; i < _HEIGHT; i++) {
        for (uint16_t j = 0; j < _WIDTH; j++) {
            _matrix.drawPixel(j, i, aColor);
            _matrix.show();
            delay(15);
        }
    }
}

void ZML_Matrix::stopMetaAction(void) {
    _currentMetaAction = &ZML_Matrix::stopMetaAction;
    setMetaDelay(-1);
}

void ZML_Matrix::blackOut(void) {
    _currentAction = &ZML_Matrix::blackOut;
    showAllPixels(0);
    stopMetaAction();
    setDelay(-1);
}

void ZML_Matrix::showAllPixels(uint32_t aColor) {
    for (uint8_t i = 0; i < _NB_PIXELS; i++) {
        _pixels.setPixelColor(i, aColor);
    }
    _pixels.show();
}

void ZML_Matrix::showAllPixels(uint8_t aR, uint8_t aG, uint8_t aB) {
    showAllPixels(_pixels.Color(aR, aG, aB));
}

void ZML_Matrix::setScrollTextSpeed(uint8_t aDivisor) {
    if (aDivisor == 0)
        aDivisor = 1;
    else if (aDivisor > ZML_MATRIX_SCROLLTEXT_MAX_DIV)
        aDivisor = ZML_MATRIX_SCROLLTEXT_MAX_DIV;
    
    _scrolledTxtTick = ZML_MATRIX_SCROLLTEXT_MAX_DELAY / aDivisor;
}

void ZML_Matrix::setScrolledText(char* aTxt) {
    _utf8ascii(aTxt);
    _scrolledTxt = aTxt;
}

void ZML_Matrix::setScrolledTxtColor(uint16_t aColor) {
    _scrolledTxtColor = aColor;
}

void ZML_Matrix::setScrolledTxtFgColor(uint16_t aColor) {
    _scrolledTxtFgColor = aColor;
}

void ZML_Matrix::setScrolledTxtY(uint8_t aY) {
    _scrolledTxtY = aY;
}

void ZML_Matrix::scrollText(void) {
    int l = strlen(_scrolledTxt);
    // default font char width: 6 pixels. + 1 space
    int lpixels = l * 7;
    _matrix.setTextColor(_scrolledTxtColor);
    
    _scrolledTxtX  = _WIDTH;
    _scrolledTxtMaxX = _WIDTH - lpixels;
    _currentAction = &ZML_Matrix::_scrollTextFrame;
    _scrollTextFrame();
}

// void ZML_Matrix::scrollText(String aTxt, uint16_t aTxtColor, uint16_t aFgColor) {
//     scrollText(String aTxt, uint16_t aTxtColor, uint16_t aFgColor, 0);
//}

// void ZML_Matrix::scrollText(char aTxt[], uint16_t aTxtColor, uint16_t aFgColor) {
//     scrollText(aTxt, aTxtColor, aFgColor, 0);
// }
void ZML_Matrix::scrollText(char *aTxt, uint16_t aTxtColor, uint16_t aFgColor) {
    scrollText(aTxt, aTxtColor, aFgColor, 0);
}

void ZML_Matrix::scrollText
(char *aTxt, uint16_t aTxtColor, uint16_t aFgColor, uint8_t aY) {
    setScrolledText(aTxt);
    setScrolledTxtColor(aTxtColor);
    setScrolledTxtFgColor(aFgColor);
    _scrolledTxtY = aY;
    
    scrollText();
}

void ZML_Matrix::_scrollTextFrame(void) {
    _matrix.fillScreen(_scrolledTxtFgColor);
    _matrix.setCursor(--_scrolledTxtX, _scrolledTxtY);
    _matrix.print(_scrolledTxt);
    _matrix.show();
    if (_scrolledTxtX > _scrolledTxtMaxX)
        setDelay(_scrolledTxtTick);
    else
        setDelay(-1);
}

// https://playground.arduino.cc/Main/Utf8ascii

// Convert a single Character from UTF8 to Extended ASCII
// Return "0" if a byte has to be ignored
byte ZML_Matrix::_utf8ascii(byte aAscii) {
    if (aAscii < 128)   // Standard ASCII-set 0..0x7F handling
    {
        _lastCharBuf = 0;
        return aAscii;
    }

    // get previous input
    byte last = _lastCharBuf;   // get last char
    _lastCharBuf = aAscii;      // remember actual character

    switch (last)     // conversion depending on first UTF8-character
    {   case 0xC2: return  aAscii;  break;
        case 0xC3: return  (aAscii | 0xC0);  break;
        case 0x82: if (aAscii == 0xAC) return 0x80; // special case Euro-symbol
    }

    return 0; // otherwise: return zero, if character has to be ignored
}

// convert String object from UTF8 String to Extended ASCII
// String utf8ascii(String s)
// {
//         String r="";
//         char c;
//         for (int i=0; i<s.length(); i++)
//         {
//                 c = utf8ascii(s.charAt(i));
//                 if (c!=0) r+=c;
//         }
//         return r;
// }

// In Place conversion UTF8-string to Extended ASCII (ASCII is shorter!)
void ZML_Matrix::_utf8ascii(char* aS) {
    int k = 0;
    char c;
    int l = strlen(aS);
    for (int i = 0; i < l; i++) {
        c = _utf8ascii(aS[i]);
        if (c != 0)
            aS[k++] = c;
    }
    aS[k]=0;
}

// static void swap (uint16_t *a, uint16_t *b) {
static void swap (uint16_t &a, uint16_t &b) {
    uint16_t temp = a;
    a = b;
    b = temp;
}

void ZML_Matrix::fillRandom(void) {
    uint16_t i = 0;
    for (i = 0; i < _NB_PIXELS; i++) {
        _randIndexes[i] = i;
    }
    for (i = _NB_PIXELS - 1; i > 0; i--) {
        uint16_t j = random(0, _NB_PIXELS);
        swap(_randIndexes[i], _randIndexes[j]);
    }
    _curRandI = 0;
    _currentAction = &ZML_Matrix::_fillRandomStep;
    _fillRandomStep();
}

void ZML_Matrix::_fillRandomStep(void) {
    if (_curRandI >= _NB_PIXELS) {
        fillRandom();
    } else {
        _pixels.setPixelColor(_randIndexes[_curRandI],
                              _pixels.Color(random(0, 128),
                                            random(0, 128),
                                            random(0, 128)));
        _pixels.show();
        _curRandI++;
        setDelay(ZML_MATRIX_RANDPAINT_TICK);
    }
}

int16_t ZML_Matrix::_getMaxCircleR(void) {
    if (_maxCircleR == 0) {
        _maxCircleR = max(floor(_WIDTH * 1.43), floor(_HEIGHT * 1.43));
    }
    
    return _maxCircleR;
}

void ZML_Matrix::fillWithCircles(void) {
    _curCircleColorI = 0;
    _fillWithCircles();
}

void ZML_Matrix::_fillWithCircles(void) {
    _curCircleR = 1;
    _curCircleX = random(0, _WIDTH + 1);
    _curCircleY = random(0, _HEIGHT + 1);
    ZML_MATRIX_SERIAL.printf("_curCircleX: %d, _curCircleY: %d\n", _curCircleX, _curCircleY);
    _currentAction = &ZML_Matrix::_fillWithCirclesFrame;
    _fillWithCirclesFrame();
}

void ZML_Matrix::_fillWithCirclesFrame(void) {
    if (_curCircleR > _getMaxCircleR()) {
        _curCircleColorI++;
        if (_curCircleColorI >= _CIRCLE_NB_COLOR)
            _curCircleColorI = 0;
        _fillWithCircles();
    } else {
        ZML_MATRIX_SERIAL.printf("_curCircleR: %d\n", _curCircleR);
        _matrix.fillCircle(_curCircleX, _curCircleY, _curCircleR,
                           _circleColors[_curCircleColorI]);
        _matrix.show();
        _curCircleR++;
        setDelay(ZML_MATRIX_FILLCIRCLE_TICK);
    }
}

void ZML_Matrix::setZigZagY(uint8_t aY) {
    _zigzagY = aY;
}

int8_t ZML_Matrix::_getZigZagNbTriangles(void) {
    if (_zigzagNbTriangle == -1) {
        _zigzagNbTriangle = ceil((float)_WIDTH /
                                 (float) ZML_MATRIX_ZIGZAG_TRGLEWIDTH);
    }
    
    return _zigzagNbTriangle;
}

void ZML_Matrix::fixedZigzag(void) {
    _matrix.fillScreen(0);
    _matrix.show();
    
    uint8_t x = 0;
    for (int8_t i = 0; i <= _getZigZagNbTriangles(); i++) {
        _matrix.drawLine(x, _zigzagY + 3, x + 2, _zigzagY + 1, COLOR_WHITE);
        _matrix.drawLine(x + 2, _zigzagY + 1, x + 4, _zigzagY + 3, COLOR_WHITE);
        _matrix.fillTriangle(x, _zigzagY + 5, x + 2, _zigzagY + 3, x + 4,
                             _zigzagY + 5, COLOR_PURPLE);
        _matrix.drawLine(x, _zigzagY + 7, x + 1, _zigzagY + 7, COLOR_WHITE);
        _matrix.drawLine(x + 3, _zigzagY + 7, x + 4, _zigzagY + 7, COLOR_WHITE);
        x = (i * ZML_MATRIX_ZIGZAG_TRGLEWIDTH) - 1;
    }
    _matrix.show();
}

void ZML_Matrix::rel_zigzag(int8_t x0) {
    _matrix.fillScreen(0);
    _matrix.show();
    
    int8_t x = x0;
    int8_t l = _getZigZagNbTriangles() + 1;
    // int8_t l = _getZigZagNbTriangles();
    for (int8_t i = 0; i <= l; i++) {
        _matrix.drawLine(x, _zigzagY + 3, x + 2, _zigzagY + 1, COLOR_WHITE);
        _matrix.drawLine(x + 2, _zigzagY + 1, x + 4, _zigzagY + 3, COLOR_WHITE);
        _matrix.fillTriangle(x, _zigzagY + 5, x + 2, _zigzagY + 3, x + 4,
                             _zigzagY + 5, COLOR_PURPLE);
        _matrix.drawLine(x, _zigzagY + 7, x + 1, _zigzagY + 7, COLOR_WHITE);
        _matrix.drawLine(x + 3, _zigzagY + 7, x + 4, _zigzagY + 7, COLOR_WHITE);
        x = x0 + ((i + 1) * ZML_MATRIX_ZIGZAG_TRGLEWIDTH) - i - 1;
    }
    _matrix.show();
}

void ZML_Matrix::zigzag(void) {
    _zigzagX = 0;
    _currentAction = &ZML_Matrix::_zigzagStep;
    _zigzagStep();
}

void ZML_Matrix::_zigzagStep(void) {
    if (_zigzagX >= ZML_MATRIX_ZIGZAG_TRGLEWIDTH - 1)
        _zigzagX = 0;
    rel_zigzag(-_zigzagX);
    _zigzagX++;
    setDelay(ZML_MATRIX_ZIGZAG_TICK);
}

//Converts an HSV color to RGB color
uint16_t ZML_Matrix::HSVtoRGB(uint8_t ih, uint8_t is, uint8_t iv) {
    float r, g, b, h, s, v; //this function works with floats between 0 and 125⋅255/100
    float f, p, q, t;
    int i;
    
    h = (float)(ih / 256.0);
    s = (float)(is / 256.0);
    v = (float)(iv / 256.0);
    
    //if saturation is 0, the color is a shade of grey
    if (s == 0.0) {
        b = v;
        g = b;
        r = g;
    }
    //if saturation > 0, more complex calculations are needed
    else
    {
        h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
        i = (int)(floor(h)); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
        f = h - i;//the fractional part of h
        
        p = (float)(v * (1.0 - s));
        q = (float)(v * (1.0 - (s * f)));
        t = (float)(v * (1.0 - (s * (1.0 - f))));
        
        switch (i) {
            case 0: r=v; g=t; b=p; break;
            case 1: r=q; g=v; b=p; break;
            case 2: r=p; g=v; b=t; break;
            case 3: r=p; g=q; b=v; break;
            case 4: r=t; g=p; b=v; break;
            case 5: r=v; g=p; b=q; break;
            default: r = g = b = 0; break;
        }
    }
    
    return _matrix.Color(r * 255.0, g * 255.0, b * 255.0);
}

void ZML_Matrix::drawRainbowVLines(void) {
    _rvlX = 0;
    _rvlY = _HEIGHT - 1;
    _rvlStep = 255 / _WIDTH;
    _rvlHue = 0;
    _rvlColor = HSVtoRGB(_rvlHue, 255, 200);
    _matrix.fillScreen(0);
    _matrix.show();
    _currentAction = &ZML_Matrix::_drawRainbowVLines;
    _drawRainbowVLines();
}

void ZML_Matrix::_drawRainbowVLines(void) {
    if (_rvlX >= _WIDTH) {
        setDelay(-1);
        
        return;
    }
    
    if (_rvlY < 0) {
        _rvlY = _HEIGHT - 1;
        _rvlX++;
        _rvlHue += _rvlStep;
        _rvlColor = HSVtoRGB(_rvlHue, 255, 200);
    }
    
    _matrix.drawPixel(_rvlX, _rvlY, _rvlColor);
    _matrix.show();
    _rvlY--;
    setDelay(ZML_MATRIX_RVL_TICK);
}

void ZML_Matrix::drawRainbowHLines(void) {
    _rhlX = _WIDTH - 1;
    _rhlY = 0;
    _rhlStep = 255 / _HEIGHT;
    _rhlHue = 0;
    _rhlColor = HSVtoRGB(_rhlHue, 255, 200);
    _matrix.fillScreen(0);
    _matrix.show();
    _currentAction = &ZML_Matrix::_drawRainbowHLines;
    _drawRainbowHLines();
}

void ZML_Matrix::_drawRainbowHLines(void) {
    if (_rhlY >= _HEIGHT) {
        setDelay(-1);
        
        return;
    }
    
    if (_rhlX < 0) {
        _rhlX = _WIDTH - 1;
        _rhlY++;
        _rhlHue += _rhlStep;
        _rhlColor = HSVtoRGB(_rhlHue, 255, 200);
    }
    
    _matrix.drawPixel(_rhlX, _rhlY, _rhlColor);
    _matrix.show();
    _rhlX--;
    setDelay(ZML_MATRIX_RHL_TICK);
}

//-----------------------------------------------------------------------------
// Fire Effect
//-----------------------------------------------------------------------------
// https://github.com/giladaya/arduino-led-matrix/tree/master/fire
// https://github.com/mic159/NeoFire

// these values are substraced from the generated values to give a shape to the animation
// const unsigned char
const uint8_t fireValueMask[ZML_MATRIX_FIRE_HEIGHT][ZML_MATRIX_FIRE_WIDTH] = {
    {32 , 0  , 0  , 0  , 0  , 0  , 0  , 32 },
    {64 , 0  , 0  , 0  , 0  , 0  , 0  , 64 },
    {96 , 32 , 0  , 0  , 0  , 0  , 32 , 96 },
    {128, 64 , 32 , 0  , 0  , 32 , 64 , 128},
    {160, 96 , 64 , 32 , 32 , 64 , 96 , 160},
    {192, 128, 96 , 64 , 64 , 96 , 128, 192},
    {255, 160, 128, 96 , 96 , 128, 160, 255},
    {255, 192, 160, 128, 128, 160, 192, 255}
};

// these are the hues for the fire,
// should be between 0 (red) to about 25 (yellow)
// const unsigned char
const uint8_t fireHueMask[ZML_MATRIX_FIRE_HEIGHT][ZML_MATRIX_FIRE_WIDTH] = {
    {1 , 11, 19, 25, 25, 22, 11, 1},
    {1 , 8 , 13, 19, 25, 19, 8 , 1},
    {1 , 8 , 13, 16, 19, 16, 8 , 1},
    {1 , 5 , 11, 13, 13, 13, 5 , 1},
    {1 , 5 , 11, 11, 11, 11, 5 , 1},
    {0 , 1 , 5 , 8 , 8 , 5 , 1 , 0},
    {0 , 0 , 1 , 5 , 5 , 1 , 0 , 0},
    {0 , 0 , 0 , 1 , 1 , 0 , 0 , 0}
};
// const unsigned char
const uint8_t
firePurpleHueMask[ZML_MATRIX_FIRE_HEIGHT][ZML_MATRIX_FIRE_WIDTH] = {
    {201 , 211, 219, 225, 225, 222, 211, 201},
    {201 , 208 , 213, 219, 225, 219, 208 , 201},
    {201 , 208 , 213, 216, 219, 216, 208 , 201},
    {201 , 205 , 211, 213, 213, 213, 205 , 201},
    {201 , 205 , 211, 211, 211, 211, 205 , 201},
    {200 , 201 , 205 , 208 , 208 , 205 , 201 , 200},
    {200 , 200 , 201 , 205 , 205 , 201 , 200 , 200},
    {200 , 200 , 200 , 201 , 201 , 200 , 200 , 200}
};
// const unsigned char
const uint8_t
firePurple2HueMask[ZML_MATRIX_FIRE_HEIGHT][ZML_MATRIX_FIRE_WIDTH] = {
    {225, 219, 211, 201, 201, 211, 222, 225},
    {219, 213, 208, 201, 201, 208, 219, 225},
    {216, 213, 208, 201, 201, 208, 216, 219},
    {213, 211, 205, 201, 201, 205, 213, 213},
    {211, 211, 205, 201, 201, 205, 211, 211},
    {208, 205, 201, 200, 200, 201, 205, 208},
    {205, 201, 200, 200, 200, 200, 201, 205},
    {201, 200, 200, 200, 200, 200, 200, 201}
};

/**
 * Randomly generate the next line (matrix row)
 */
void ZML_Matrix::_generateFireLine(void) {
    for (uint8_t x = 0; x < ZML_MATRIX_FIRE_WIDTH; x++) {
        _fireLine[x] = random(64, 255);
    }
}

/**
 * shift all values in the matrix up one row
 */
void ZML_Matrix::_shiftUpFire(void) {
    for (uint8_t y = ZML_MATRIX_FIRE_HEIGHT - 1; y > 0; y--) {
        for (uint8_t x = 0; x < ZML_MATRIX_FIRE_WIDTH; x++) {
            _fireMatrix[y][x] = _fireMatrix[y-1][x];
        }
    }
    for (uint8_t x = 0; x < ZML_MATRIX_FIRE_WIDTH; x++) {
        _fireMatrix[0][x] = _fireLine[x];
    }
}

/**
 * draw a frame, interpolating between 2 "key frames"
 * @param pcnt percentage of interpolation
 */
void ZML_Matrix::_drawFireFrame(uint8_t pcnt) {
    _drawFireFrame(pcnt, ZML_MATRIX_FIRE_TYPE_NATURAL);
}
void ZML_Matrix::_drawFireFrame(uint8_t pcnt, uint8_t aType) {
    int nextv;
    uint16_t color;
  
    // each row interpolates with the one before it
    for (uint8_t y = ZML_MATRIX_FIRE_HEIGHT - 1; y > 0; y--) {
        for (uint8_t x = 0; x < ZML_MATRIX_FIRE_WIDTH; x++) {
            nextv =
            (((100.0 - pcnt) * _fireMatrix[y][x] + pcnt * _fireMatrix[y-1][x])
            / 100.0)
            - fireValueMask[y][x];
            
            switch (aType) {
                case ZML_MATRIX_FIRE_TYPE_PURPLE:
                    color = HSVtoRGB(
                        firePurpleHueMask[y][x], // H
                        255,                     // S
                        (uint8_t) max(0, nextv)  // V
                    );
                    break;
                case ZML_MATRIX_FIRE_TYPE_PURPLE2:
                    color = HSVtoRGB(
                        firePurple2HueMask[y][x], // H
                        255,                      // S
                        (uint8_t) max(0, nextv)   // V
                    );
                    break;
                case ZML_MATRIX_FIRE_TYPE_NATURAL:
                default:
                    color = HSVtoRGB(
                        fireHueMask[y][x],      // H
                        255,                    // S
                        (uint8_t) max(0, nextv) // V
                    );
            }
            
            _matrix.drawPixel(_fireX0 + x, _fireY0 - y, color);
        }
    }
    
    // first row interpolates with the "next" line
    for (uint8_t x = 0; x < ZML_MATRIX_FIRE_WIDTH; x++) {
        nextv = ((100.0-pcnt) * _fireMatrix[ZML_MATRIX_FIRE_HEIGHT][x]
                    + pcnt * _fireLine[x])
                / 100.0;
        switch (aType) {
            case ZML_MATRIX_FIRE_TYPE_PURPLE:
                color = HSVtoRGB(
                    firePurpleHueMask[0][x], // H
                    255,                     // S
                    (uint8_t) nextv          // V
                );
                break;
            case ZML_MATRIX_FIRE_TYPE_PURPLE2:
                color = HSVtoRGB(
                    firePurple2HueMask[0][x], // H
                    255,                      // S
                    (uint8_t) nextv           // V
                );
                break;
            case ZML_MATRIX_FIRE_TYPE_NATURAL:
            default:
                color = HSVtoRGB(
                    fireHueMask[0][x], // H
                    255,               // S
                    (uint8_t) nextv    // V
                );
        }
        _matrix.drawPixel(_fireX0 + x, _fireY0, color);
    }
}

void ZML_Matrix::setFirePosition(uint8_t aX, uint8_t aY) {
    _fireX0 = aX;
    _fireY0 = aY;
}

void ZML_Matrix::_fire (void) {
    if (_fire_pcnt >= 100) {
        _shiftUpFire();
        _generateFireLine();
        _fire_pcnt = 0;
    }
    _drawFireFrame(_fire_pcnt, _curFireType);
    _matrix.show();
    _fire_pcnt += 30;
}

void ZML_Matrix::fire(void) {
    fire(ZML_MATRIX_FIRE_TYPE_NATURAL);
}

void ZML_Matrix::fire(uint8_t aType) {
    _matrix.fillScreen(0);
    _matrix.show();
    _curFireType = aType;
    _currentAction = &ZML_Matrix::_fire;
    _fire();
    setDelay(ZML_MATRIX_FIRE_TICK);
}
