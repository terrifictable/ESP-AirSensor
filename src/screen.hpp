#include <Adafruit_SSD1306.h> // Include the SSD1306 library needed for OLED
#include <algorithm>



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
    std::vector<Value> vec = *in;

    // remove data that is older than `x`
    while (vec.size() > SIZE_LIMIT)
        vec.erase((vec).begin()); // remove first (oldest) entry in array/vector 

    int16_t TOP           = 0;
    if (displaytime) {
        TOP = 16;
    }

    std::vector<int> co2vec{};
    for (Value val : vec) { co2vec.push_back(val.co2); }
    int16_t MAX           = *std::max_element(co2vec.begin(), co2vec.end()); // Highest co2 value in current array
    static int16_t HEIGHT = 64 - TOP; // the screen height

    int16_t lastY = (HEIGHT - (vec[0].co2 * HEIGHT) / MAX) + TOP; // variables for last values
    /* DONT use linear interpolation to calculate y positions */
    // int16_t lastY = rangeLerpC(vec[0].co2, MIN, MAX, 0, 64);
    for (unsigned int i=0; i < vec.size(); i++ ) { // iterate over Value array/vector
        int16_t y = ((HEIGHT - vec[i].co2 * HEIGHT) / MAX); // + TOP; // calculate on what hight to draw a pixel
        display.drawLine(i-1, lastY + 64, i, y + 64, color); // draw line from last point to new point
        lastY = y; // update values
    }

    display.setTextColor(WHITE); // set text color
    display.setTextSize(2); // set text size
    float x = (SCREEN_WIDTH/2) - (String("Co2: " + String(avg)).length() * 6); // get width of `Co2: <current verage co2 value>`
    display.setCursor(x, SCREEN_HEIGHT/2 + TOP/2); // set cursor to 0,0
    display.println("Co2: " + String(avg)); // print text to display starting at position 0,0


    if (displaytime) {
        display.setTextColor(WHITE);
        display.setTextSize(1);
        time_t now = time(nullptr);
        struct tm *ti = localtime(&now);
        String currtime = (ti->tm_hour < 10 ? "0" + String(ti->tm_hour) : String(ti->tm_hour)) + ":" 
                        + (ti->tm_min  < 10 ? "0" + String(ti->tm_min)  : String(ti->tm_min))  + ":" 
                        + (ti->tm_sec  < 10 ? "0" + String(ti->tm_sec)  : String(ti->tm_sec));
        display.setCursor(128/2 - (currtime.length() * 3), 4);
        display.println(currtime);

        display.drawLine(0, TOP-2, 128, TOP-2, WHITE);
        display.drawLine(0, TOP-3, 128, TOP-3, WHITE);
    }


    display.display(); // update display
}


