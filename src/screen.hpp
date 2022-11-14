#include <Adafruit_SSD1306.h> // Include the SSD1306 library needed for OLED



// Set DATA and CLOCK pins for the OLED
#define SDA             D1
#define SCL             D2

// Set RST pin for the OLED
#define OLED_RESET      0
// Define OLED screen width and height
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define SIZE_LIMIT      SCREEN_WIDTH


// create OLED object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// initialize OLED
void oled_init() {

    // Initialize the OLED display.
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay(); // clear the display buffer
    display.display(); // display the current buffer

}


// draw image and text
void DisplayText(String text, int text_size = 2, int posX = 0, int posY = 0) {

    // Clear the display.
    display.clearDisplay();
    display.display();

    display.setTextColor(WHITE); // set text color
    display.setTextSize(text_size); // set text size
    display.setCursor(posX, posY); // set cursor to 0,0
    display.println(text); // print text to display starting at position 0,0
    display.display(); // display the current buffer

}

// draw graph of co2 data and display average value
void draw(std::vector<Value> *in, int avg, int16_t color) {
    display.clearDisplay(); // clear current display buffer
    
    // remove data that is older than `x`
    while ((*in).size() > SIZE_LIMIT) {
        (*in).erase((*in).begin()); // remove first aka oldest entry in array/vector 
    }

    int16_t MAX           = 1; // Highest co2 value in current array
    static int16_t HEIGHT = 64; // the screen height
    for (unsigned int i=0; i < (*in).size(); i++ ) // get highest co2 value
        if ((*in)[i].co2 >= MAX) MAX = (*in)[i].co2; // check if value is higher than current highest, if yes update else, do nothing
    
    int16_t lastI = 0; // variables for last values
    int16_t lastY = HEIGHT - ((*in)[0].co2 * HEIGHT) / MAX; // variables for last values
    for (unsigned int i=0; i < (*in).size(); i++ ) { // iterate over Value array/vector
        Value val = (*in)[i]; // local `val` has the value of current index in array/vector
        int16_t y = HEIGHT - (val.co2 * HEIGHT) / MAX; // calculate on what hight to draw a pixel
        display.drawLine(lastI, lastY, i, y, color); // draw line from last point to new point
        lastI = i; // update values
        lastY = y; // update values
    }
    
    display.setTextColor(WHITE); // set text color
    display.setTextSize(2); // set text size
    float x = (SCREEN_WIDTH/2) - (String("Co2: " + String(avg)).length() * 6); // get width of `Co2: <current verage co2 value>`
    display.setCursor(x, SCREEN_HEIGHT/2); // set cursor to 0,0
    display.println("Co2: " + String(avg)); // print text to display starting at position 0,0
    
    display.display(); // update display
}


