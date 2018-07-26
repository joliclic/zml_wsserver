#ifndef ZML_MASK_LAYOUT_H_
#define ZML_MASK_LAYOUT_H_

// Be carefull MASK_NUM_PIXELS is uint8_t , so between 0 and 255.
// But we wiil use int8_t for the leds layout, so a max of 127

// copy of the skullybeno mask

#define MASK_NUM_PIXELS 41
#define MASK_NB_LED_GROUPS 7
#define MASK_NB_LED_MAX_PER_GROUP 6

const int8_t MASK_LEDS_LAYOUT[MASK_NB_LED_GROUPS][MASK_NB_LED_MAX_PER_GROUP] = {
    { 4,  3,  2,  1,  0, -1},
    {10, 9,  8,  7,  6,  5},
    {11, 12, 13, 14, 15, 16},
    {17, 18, 19, 20, 21, 22},
    {28, 27, 26, 25, 24, 23},
    {29, 30, 31, 32, 33, 34},
    {40, 39, 38, 37, 36, 35}
};

#endif  // ZML_MASK_LAYOUT_H_
