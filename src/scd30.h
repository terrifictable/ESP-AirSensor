#include "SparkFun_SCD30_Arduino_Library.h" // Include the SDC30 library


// set data structure for SDC30
typedef struct {
    int             Co2             = 0; // CO2 variable
    float           Temperature     = 0; // Temperature variable
    int             Humidity        = 0; // Humidity variable

    String          SerialNumber    = "n/a"; // Serial Number
    String          Firmware        = "n/a"; // Firmware Version
    unsigned long   TimeOut         = millis() + 10000; // Timeout

} SDC30Data; // Define data type

SDC30Data ConfigSDC30; // create data structure for SDC30
SCD30 airSensor; // create air sensor object



void SDC30(std::vector<Value> *vector) {

    static boolean SDC30Connected = false;
    static int RingBufferSize = 126;
    static int RingBufferPos = -1;
    __unused static float Data[126][3]; // 0=CO2 1=Temp 3=Hum

    // if SDC30 is not connected but avalible, try to connect
    if (!SDC30Connected) {
        print("SDC30:Init"); // log

        for (int a = 0; a < RingBufferSize; a++) { // set default data
            Data[a][0] = -1.0;
            Data[a][1] = -1.0;
            Data[a][2] = -1.0;
        }
        RingBufferPos = 0; // set first position in ring buffer
        
        if (airSensor.begin(Wire) == false) { // try to connect to SDC30 and set auto calibration to true
            print("SDC30:Sensor not detected. Please check wiring. Now disabled!"); // log
            delay(5000); // wait 5sec 
            ESP.restart(); // restart
        }

        // set sensor settings
        airSensor.setAutoSelfCalibration(true); // to check if auto calibration is actually true
        // airSensor.setMeasurementInterval(2); // set measurement interval to 2sec
        // airSensor.setTemperatureOffset(2); // set temperature offset to 2°C
        // airSensor.setForcedRecalibrationFactor(400);

        SDC30Connected = true; // set SDC30 as connected

    }

    // return if SDC30 is not connected
    if (!SDC30Connected)
        return;

    // Define local variables
    int CurrCo2 = ConfigSDC30.Co2;
    float CurrTemperature = ConfigSDC30.Temperature;
    int CurrHumidity = ConfigSDC30.Humidity;


    // Check if SDC30 is connected / available
    if (airSensor.dataAvailable()) {
        if (airSensor.getCO2() != 0) { // check if CO2 is not 0
            ConfigSDC30.TimeOut = millis() + 10000; // add 10sec timeout
            ConfigSDC30.Co2 = airSensor.getCO2(); // get CO2 value and set ConfigSDC30 value
            ConfigSDC30.Temperature = airSensor.getTemperature(); // get Temperature value and set ConfigSDC30 value
            ConfigSDC30.Humidity = airSensor.getHumidity(); // get Humidity value and set ConfigSDC30 value
        }
    }


    RingBufferPos++;
    if (RingBufferPos == RingBufferSize) {
        RingBufferPos = 0;
    }

    // Set ConfigSDC30 data values
    Data[RingBufferPos][0] = ConfigSDC30.Co2;
    Data[RingBufferPos][1] = ConfigSDC30.Temperature;
    Data[RingBufferPos][2] = ConfigSDC30.Humidity;


    // if SDC30 timeouts or gets disconnected or does something else, reset the sensor
    if (millis() >= ConfigSDC30.TimeOut) {
        print("001 Sensort, SDC30 ERROR; TIMEOUT\n"); // print
        ConfigSDC30.TimeOut = millis() + 10000; // add 10sec timeout

        #ifdef DEBUG
            // Debug
            Msg(F("SDC30"), F("TimeOUT!"));
        #endif
    }



    // check if data has changed
    if (CurrCo2 != ConfigSDC30.Co2 or CurrTemperature != ConfigSDC30.Temperature or CurrHumidity != ConfigSDC30.Humidity) {
        // check if CO2 values are ok or not, color leds and return status String
        String status = checkCO2Values(ConfigSDC30.Co2, ConfigSDC30.Temperature, airSensor.getHumidity()); // check CO2 values and blink red led if values are not ok, otherwise do nothing

        // Log CO2, Temperature, Humidity values
        Serial.printf("CO2: %d\t TEMPERATURE: %.1f\t HUMIDITY: %.2f%\t Status: %s\n", ConfigSDC30.Co2, ConfigSDC30.Temperature, airSensor.getHumidity(), status.c_str());

        // create new Value structure containing Co2, Temperature and humidity values
        Value val = {
            ConfigSDC30.Co2,
            ConfigSDC30.Temperature,
            airSensor.getHumidity(),
        };
        (*vector).push_back(val); // add new value to vector
        draw(vector, ConfigSDC30.Co2, WHITE /* aka `1` */); // draw graph and text

    }
}
