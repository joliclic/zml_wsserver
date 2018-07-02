#ifndef LEDS_LAYOUT_H_
#define LEDS_LAYOUT_H_

// Be carefull MASK_NUM_PIXELS is uint8_t , so between 0 and 255.
// But we wiil use int8_t for the leds layout, so a max of 127

#define MASK_NUM_PIXELS 62
#define MASK_NB_LED_GROUPS 7
#define MASK_NB_LED_MAX_PER_GROUP 15

const int8_t MASK_LEDS_LAYOUT[NB_LED_GROUPS][NB_LED_MAX_PER_GROUP] = {
    {14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0},
    {20, 19, 18, 17, 16, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {21, 22, 23, 24, 25, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41},
    {47, 46, 45, 44, 43, 42, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {48, 49, 50, 51, 52, 53, 54, 55, -1, -1, -1, -1, -1, -1, -1},
    {56, 57, 58, 59, 60, 61}
};

#endif  // LEDS_LAYOUT_H_
