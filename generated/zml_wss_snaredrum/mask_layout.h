#ifndef ZML_MASK_LAYOUT_H_
#define ZML_MASK_LAYOUT_H_

// Be carefull MASK_NUM_PIXELS is uint8_t , so between 0 and 255.
// But we wiil use int8_t for the leds layout, so a max of 127

#define MASK_NUM_PIXELS 72
#define MASK_NB_LED_GROUPS 6
#define MASK_NB_LED_MAX_PER_GROUP 12

#define MASK_BRIGHTNESS 50

const int8_t MASK_LEDS_LAYOUT[MASK_NB_LED_GROUPS][MASK_NB_LED_MAX_PER_GROUP] = {
    {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11},
    { 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
    { 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
    { 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36},
    { 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48},
    { 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60}
};

#endif  // ZML_MASK_LAYOUT_H_
