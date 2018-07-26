#ifndef ZML_MASK_LAYOUT_H_
#define ZML_MASK_LAYOUT_H_

// Be carefull MASK_NUM_PIXEL is uint8_t , so between 0 and 255.
// But we wiil use int8_t for the leds layout, so a max of 127

#define MASK_NUM_PIXELS 48
#define MASK_NB_LED_GROUPS 7
#define MASK_NB_LED_MAX_PER_GROUP 12

const int8_t MASK_LEDS_LAYOUT[MASK_NB_LED_GROUPS][MASK_NB_LED_MAX_PER_GROUP] = {
    {  5,   4,   3,   2,   1,   0,  -1,  -1,  -1,  -1,  -1,  -1},
    { 11,  10,   9,   8,   7,   6,  -1,  -1,  -1,  -1,  -1,  -1},
    { 12,  13,  14,  15,  16,  17,  -1,  -1,  -1,  -1,  -1,  -1},
    { 18,  19,  20,  21,  22,  23,  -1,  -1,  -1,  -1,  -1,  -1},
    { 29,  28,  27,  26,  25,  24,  -1,  -1,  -1,  -1,  -1,  -1},
    { 30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41},
    { 42,  43,  44,  45,  46,  47,  -1,  -1,  -1,  -1,  -1,  -1}
};


#endif  // ZML_MASK_LAYOUT_H_
