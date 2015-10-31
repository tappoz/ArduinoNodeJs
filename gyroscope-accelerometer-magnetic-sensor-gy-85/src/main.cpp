
// Cfr.:
// http://www.himix.lt/arduino/arduino-and-gy-85-9dof-accelerometer-adxl345-gyroscope-itg3200-and-magnetometer-hmc5883-angle-information-comparison/
// http://eeenthusiast.com/arduino-i2c-adxl-345-robot/
// https://github.com/sqrtmo/GY-85-arduino/blob/master/GY_85.cpp

#include <Wire.h>
#include <SPI.h>
#include "I2CHelper.h"
#include "ADXL345.h"

// register cells
#define D0 1  // 2^0
#define D1 2  // 2^1
#define D2 4  // 2^2
#define D3 8  // 2^3
#define D4 16 // 2^4
#define D5 32 // 2^5
#define D6 64 // 2^6

// ADXL345 accelerometer address as specified in data sheet
// http://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf
// #define ADXL345 (0x53)

// ITG3200 gyroscope
// http://www.invensense.com/products/motion-tracking/3-axis/itg-3200/
// http://43zrtwysvxb2gf29r5o0athu.wpengine.netdna-cdn.com/wp-content/uploads/2015/02/ITG-3200-Datasheet.pdf
// http://43zrtwysvxb2gf29r5o0athu.wpengine.netdna-cdn.com/wp-content/uploads/2015/02/ITG-3200-Register-Map.pdf
// AD0 1101000 I2C address
#define ITG3200 (0x68)


#define DATAX0 0x32 // X-Axis Data 0
// #define DATAX1 0x33 // X-Axis Data 1
// #define DATAY0 0x34 // Y-Axis Data 0
// #define DATAY1 0x35 // Y-Axis Data 1
// #define DATAZ0 0x36 // Z-Axis Data 0
// #define DATAZ1 0x37 // Z-Axis Data 1

// ADXL345
#define POWER_CTL (0x2D)
#define DATA_FORMAT (0x31)


// ITG3200 (register map, page 6)
// Power Management
#define PWR_MGM (0x3E)
// Sample Rate Divider
#define SMPLRT_DIV (0x15)
// Digital Low Pass Filter/ Full Scale range
#define DLPF_FS (0x16)
// Interrupt: Configuration
#define INT_CFG (0x17)


// milliseconds between reads
const int DEVICE_SAMPLING_MS = 250;

ADXL345 accelerometer;


// Gets the g (gravity) range setting and return it into the input rangeSetting
// it can be 2, 4, 8 or 16
void getGRange(byte* rangeSetting) {
  byte _b;
  accelerometer.readFromRegister(DATA_FORMAT, 1, &_b);
  *rangeSetting = _b & B00000011;
}

void setup(){
  Serial.begin(9600);

  // initialise the accelerometer
  accelerometer.init();
  byte currentRange;
  getGRange(&currentRange);

  // TODO: fix the following one...
  Serial.println("Current range: " + currentRange);
}

void readAccelerometer(int *outXYZ) {
  byte accelerometerValues[6];
  // the output data is twos complement, 
  // with DATAx0 as the least significant byte and DATAx1 as the most significant byte,
  // where x represent X, Y,  or Z
  accelerometer.readFromRegister(DATAX0, 6, accelerometerValues);

  outXYZ[0] = (((int)accelerometerValues[1]) << 8) | accelerometerValues[0]; 
  outXYZ[1] = (((int)accelerometerValues[3]) << 8) | accelerometerValues[2]; 
  outXYZ[2] = (((int)accelerometerValues[5]) << 8) | accelerometerValues[4];
}

void loop(){

  char accOutput[512];
  int currentXYZ[3];
  readAccelerometer(currentXYZ);
  sprintf(accOutput, "X=%d Y=%d Z=%d", currentXYZ[0], currentXYZ[1], currentXYZ[2]); 
  Serial.print(accOutput); 
  Serial.write(10); 
  delay(DEVICE_SAMPLING_MS); 
}
