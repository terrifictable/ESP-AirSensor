// include LED strip library
#include "FastLED.h"
#define LED_Count   8 // set number of LEDs
#define LED_Used    5 // set number of LEDs used
#define LED_PIN     3 // set pin for LEDs
#define LED_TYPE    WS2812B // set type of LEDs

CRGB leds[LED_Count]; // create array of LEDs



// set color for all LEDs
void SetAllLED(CRGB  c) {
    // set color for every LED in `leds` array
    for (int i = 0; i < LED_Used; i++) 
        leds[i] = c;
    
    FastLED.setBrightness(100); // set brightness to 100%
    FastLED.show(); // update LEDs
}
// set color for specific LED
void SetLED(int LED_index, CRGB  c) {
    leds[LED_index] = c; // set color for specific LED
    FastLED.setBrightness(100); // set brightness to 100%
    FastLED.show(); // update LEDs
}

// initialization function for led strip
void led_init() {
    FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, LED_Count); // add LEDs to FastLED library  
    SetAllLED(CRGB::Black); // set color for all LEDs
    FastLED.setBrightness(100); // set brightness to 100%
    FastLED.clear();  // clear all pixel data
    FastLED.show(); // update LEDs

    FastLED.setBrightness(30); // set brightness to 30%
}

// initialize LEDs
void LED() {
    static bool LEDConnected = false; // set LEDConnected to false

    if (!LEDConnected) { // if LEDConnected is false
        Serial.println("LED init.."); // log
        led_init(); // initialize LEDs

        LEDConnected = true; // set LEDConnected to true
    }
}


/* check if CO2 values are ok or not
 * if not change led color
 * 
 * values based on this graphic: https://media.discordapp.net/attachments/974217954461904906/974222244542832701/unknown.png
 */
#define MIN_GREEN_VAL   0
#define MAX_GREEN_VAL   700
#define MIN_YELLOW_VAL  700
#define MAX_YELLOW_VAL  1000
#define MIN_RED_VAL     1000
#define MAX_RED_VAL     2000
#define MIN_BLACK_VAL   2000

String checkCO2Values(int co2, float temperature, float humidity) { // may going to use `temperature` and `humidity` values to be more precise
    static unsigned long timeout    = millis() + 2500;  // set timeout to 2500ms
    static String status            = "Unknown"; // set static to empty string
    /*
     * GOOD [GREEN] ->            <700 qqm
     * OK [YELLOW] ->              700  - 1000 qqm
     * BAD [RED] ->                1000 - 2000 qqm
     * REALLY BAD [RED Blink] ->  >2000 qqm
     */

    setBuzzer(co2);

    if (co2 <= MAX_GREEN_VAL) {
        SetAllLED(CRGB::Green);
        status = "Good <700";
    } else if (co2 <= MAX_YELLOW_VAL && co2 > MIN_YELLOW_VAL) {
        SetAllLED(CRGB::Yellow);
        status = "OK >700";
    } else if (co2 <= MAX_RED_VAL && co2 > MIN_RED_VAL) {
        SetAllLED(CRGB::Red);
        status = "BAD >1000";
    } else if (co2 >= MIN_BLACK_VAL) {
        if (millis() > timeout) {
            for (int i = 0; i < LED_Count; i++) {
                if (i % 2 == 0) {
                    leds[i] = CRGB::Red;
                } else {
                    leds[i] = CRGB::Black;
                }
            }
            FastLED.show();

            timeout = millis() + 2500;
        }
        status = "BAD >2000";    
    } else {
        print("Unknown CO2 value: %d", co2);
        status = "? ?";
    }

    return status;
}
