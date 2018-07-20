#ifndef LEDS_LAYOUT_H_
#define LEDS_LAYOUT_H_

// Be carefull MASK_NUM_PIXELS is uint8_t , so between 0 and 255.
// But we wiil use int8_t for the leds layout, so a max of 127

#define MASK_NUM_PIXELS 68
#define MASK_NB_LED_GROUPS 11
#define MASK_NB_LED_MAX_PER_GROUP 14

const int8_t MASK_LEDS_LAYOUT[MASK_NB_LED_GROUPS][MASK_NB_LED_MAX_PER_GROUP] = {
    {  0,   1,   2,   3,   4,   5,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
    { 11,  10,   9,   8,   7,   6,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
    { 12,  13,  14,  15,  16,  17,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
    { 23,  22,  21,  20,  19,  18,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
    { 24,  25,  26,  27,  28,  29,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
    { 30,  31,  32,  33,  34,  35,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
    { 36,  37,  38,  39,  40,  41,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
    { 47,  46,  45,  44,  43,  42,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
    { 50,  49,  48,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1},
    { 51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64},
    { 67,  66,  65,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}
};

#endif  // LEDS_LAYOUT_H_
