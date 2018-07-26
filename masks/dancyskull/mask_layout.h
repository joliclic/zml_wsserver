#ifndef ZML_MASK_LAYOUT_H_
#define ZML_MASK_LAYOUT_H_

// Be carefull MASK_NUM_PIXELS is uint8_t , so between 0 and 255.
// But we wiil use int8_t for the leds layout, so a max of 127

#define MASK_NUM_PIXELS 42
#define MASK_NB_LED_GROUPS 7
#define MASK_NB_LED_MAX_PER_GROUP 6

const int8_t MASK_LEDS_LAYOUT[MASK_NB_LED_GROUPS][MASK_NB_LED_MAX_PER_GROUP] = {
    { 5,  4,  3,  2,  1,  0},
    {11, 10,  9,  8,  7,  6},
    {12, 13, 14, 15, 16, 17},
    {18, 19, 20, 21, 22, 23},
    {29, 28, 27, 26, 25, 24},
    {30, 31, 32, 33, 34, 35},
    {41, 40, 39, 38, 37, 36}
};

#endif  // ZML_MASK_LAYOUT_H_
