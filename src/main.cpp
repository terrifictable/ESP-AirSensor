#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include <SPI.h>

#include <iostream>
#include <chrono>
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



uint BUTTON = D3;

bool displaytime = false;


// include headers
#include "leds.hpp"
#include "screen.hpp"
#include "scd30.h"


// `setup` is called once when ESP starts
void setup() {
    Serial.begin(115200); // begin serial communication at 115200 baud
    delay(100); // wait 100ms
    
    print("Starting..."); // log
    
    pinMode(BUTTON, INPUT);

    oled_init(); // initialize OLED
    DisplayText("Starting.."); // display text
    print("OLED initialized"); // log
    // led_init();
}

// `loop` is called in an endless loop
void loop() {
    static bool once = false; // set once to false
    static std::vector<Value> vector;

    static unsigned long button_timeout = millis();
    static unsigned long last = millis();


    LED();
    SDC30(&vector); // call SDC30 function

    if (digitalRead(BUTTON) == HIGH && millis() > button_timeout) { // toggle displaytime variable when button (connected to D3 and 3v3) is pressed
        button_timeout = millis() + 500; // updated timeout, to not toggle every few milliseconds 
        displaytime = !displaytime; // toggle variable
    }

    if (millis() > last + 500 && vector.size() > 0) { // draw screen every half a second AND when data vector is not empty
        draw(&vector, ConfigSDC30.Co2, WHITE /* aka `1` */); // draw graph and text
        last = millis(); // update "timeout" variable
    }

    if (!once) { // if once is false
        print("SDC30 started"); // log
        once = true; // set once to true
    }

}
