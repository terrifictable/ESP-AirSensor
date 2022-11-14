#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <iostream>
#include <vector>



// Simple Print function
class print {
public:
    print(String s...) { // Constructor, takes string as argument
        Serial.printf((s + "\n").c_str(), s.c_str()); // Print string and newline
    }
};

// structure for sensor values
typedef struct Value {
    int co2;
    float temperature;
    float humidity;
} Value;



// include headers
#include "leds.hpp"
#include "screen.hpp"
#include "scd30.h"


// `setup` is called once when ESP starts
void setup() {
    Serial.begin(115200); // begin serial communication at 115200 baud
    delay(100); // wait 100ms
    
    print("Starting..."); // log
    
    oled_init(); // initialize OLED
    DisplayText("Starting.."); // display text
    print("OLED initialized"); // log
    // led_init();
}

// `loop` is called in an endless loop
void loop() {
    static bool once = false; // set once to false
    static std::vector<Value> vector;

    LED();
    SDC30(&vector); // call SDC30 function


    if (!once) { // if once is false
        print("SDC30 started"); // log
        once = true; // set once to true
    }

}
