#ifndef ZML_MASK_LAYOUT_H_
#define ZML_MASK_LAYOUT_H_

// Be carefull MASK_NUM_PIXELS is uint8_t , so between 0 and 255.
// But we wiil use int8_t for the leds layout, so a max of 127

#define MASK_NUM_PIXELS 0
#define MASK_NB_LED_GROUPS 0
#define MASK_NB_LED_MAX_PER_GROUP 0

const int8_t MASK_LEDS_LAYOUT[MASK_NB_LED_GROUPS][MASK_NB_LED_MAX_PER_GROUP] = {
};

#endif  // ZML_MASK_LAYOUT_H_
