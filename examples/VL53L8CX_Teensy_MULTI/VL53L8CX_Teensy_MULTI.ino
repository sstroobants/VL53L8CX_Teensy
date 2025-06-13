/* Includes ------------------------------------------------------------------*/
#include <SD.h>

#include "vl53l8cx.h"

#include "serial_helpers.h"
#include "i2c_helpers.h"
#include "sensor_helpers.h"

#define SerialPort Serial

#define LPN_PIN_FORWARD -1
#define LPN_PIN_BOTTOM 22
#define PWREN_PIN -1

// Components.
VL53L8CX sensor_vl53l8cx_forward(&Wire, LPN_PIN_FORWARD);
VL53L8CX sensor_vl53l8cx_bottom(&Wire, LPN_PIN_BOTTOM);

bool EnableAmbient = false;
bool EnableSignal = false;
uint8_t res = VL53L8CX_RESOLUTION_4X4;
uint8_t status;

// File for logging data
File dataFile;
char dataFileName[20] = "tof_data_01.csv";

/* Setup ---------------------------------------------------------------------*/
void setup()
{
    // Set LPN pin to open drain IMPORTANT TO PROTECT SENSOR FROM DAMAGE!!
    pinMode(LPN_PIN_BOTTOM, OUTPUT_OPENDRAIN);
    digitalWrite(LPN_PIN_BOTTOM, HIGH);

    // Initialize serial for output.
    SerialPort.begin(9600);

    serialPrintln("Initializing I2C bus");
    // Initialize I2C bus.
    Wire.begin();
    Wire.setClock(400000); // Set I2C clock speed to 400kHz

    // Sensor init and status reporting
    serialPrintln("Initializing sensor");


    scan_i2c();
    delay(3);


    // Set bottom sensor LPN pin low so we can set the i2c address of forward sensor
    sensor_vl53l8cx_bottom.off();
    sensor_vl53l8cx_forward.set_i2c_address(0x54); // Set I2C address for forward sensor
    serialPrintln("Set address");
    delay(50);
    sensor_vl53l8cx_bottom.on();
    scan_i2c();
    delay(50);
    sensor_init_and_report(sensor_vl53l8cx_forward, res);
    sensor_init_and_report(sensor_vl53l8cx_bottom, res);
    serialPrintln("Finished setting up sensors");

}

void loop()
{
    VL53L8CX_ResultsData Results = get_sensor_data(sensor_vl53l8cx_bottom);

    serialPrint("Bottom Sensor Data:  ");
    serialPrint("Dist 1: " + String(Results.distance_mm[0]) + " mm, ");
    serialPrint("Status 1: " + String(Results.target_status[0]) + " || ");
    
    Results = get_sensor_data(sensor_vl53l8cx_forward);

    serialPrint("Forward Sensor Data:  ");
    serialPrint("Dist 1: " + String(Results.distance_mm[0]) + " mm, ");
    serialPrintln("Status 1: " + String(Results.target_status[0]));
}