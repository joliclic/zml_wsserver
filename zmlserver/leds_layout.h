#ifndef LEDS_LAYOUT_H_
#define LEDS_LAYOUT_H_

// Be carefull num_pixel is uint8_t , so between 0 and 255.
// But we wiil use int8_t for the leds layout, so a max of 127
//#define NUM_PIXELS 144
#define NUM_PIXELS 40

const int8_t LEDS_LAYOUT[4][10] = {
    { 0,  1 ,  2,  3,  4,  5,  6,  7,  8,  9},
    //{10, 11 , 12, 13, 14, 15, 16, 17, 18, 19},
    //{20, 21 , 22, 23, 24, 25, 26, 27, 28, 29},
    {10, 11 , 12, 13, 14, 15, 16, -1, -1, -1},
    {20, 21 , 22, 23, 24, 25, -1, -1, -1, -1},
    {30, 31 , 32, 33, 34, 35, 36, 37, 38, 39},
};

#endif  // LEDS_LAYOUT_H_
