#ifndef LEDS_LAYOUT_H_
#define LEDS_LAYOUT_H_

// Be carefull num_pixel is uint8_t , so between 0 and 255.
// But we wiil use int8_t for the leds layout, so a max of 127

#define NUM_PIXELS 70
#define NB_LED_GROUPS 6
#define NB_LED_MAX_PER_GROUP 18

const int8_t LEDS_LAYOUT[NB_LED_GROUPS][NB_LED_MAX_PER_GROUP] = {
    {15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, -1, -1},
    {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33},
    {34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, -1, -1},
    {55, 54, 53, 52, 51, 50, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {63, 62, 61, 60, 59, 58, 57, 56, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {64, 65, 66, 67, 68, 69, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

#endif  // LEDS_LAYOUT_H_
