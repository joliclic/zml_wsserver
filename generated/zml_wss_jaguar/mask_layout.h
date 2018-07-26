#ifndef ZML_MASK_LAYOUT_H_
#define ZML_MASK_LAYOUT_H_

// Be carefull MASK_NUM_PIXEL is uint8_t , so between 0 and 255.
// But we wiil use int8_t for the leds layout, so a max of 127

#define MASK_NUM_PIXELS 60
#define MASK_NB_LED_GROUPS 7
#define MASK_NB_LED_MAX_PER_GROUP 16

const int8_t MASK_LEDS_LAYOUT[MASK_NB_LED_GROUPS][MASK_NB_LED_MAX_PER_GROUP] = {
    { 9,  8,  7,  6,  5,  4,  3,  2,  1,  0, -1, -1, -1, -1, -1, -1},
    {15, 14, 13, 12, 11, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {16, 17, 18, 19, 20, 21, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1, -1},
    {37, 36, 35, 34, 33, 32, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53},
    {54, 55, 56, 57, 58, 59, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

#endif  // ZML_MASK_LAYOUT_H_
