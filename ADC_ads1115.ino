// This sketch is used to practice reading differential values from the ADS1115

#include <Wire.h>

#define GAIN 5
#define SHUNT_VOLTAGE_LIMIT 0.05 // 500 amps gives 50 mv
#define SHUNT_AMP_LIMIT 200
#define PROG_GAIN_AMP_CONFIG 0.256
#define ADS1115_1 0x48 // address of the module it is 0x48 because the address pin is set to ground. There are 3 other options

#define BIAS 0 // for grounded shunt
int16_t measured_value = 0;

const float MVPS = PROG_GAIN_AMP_CONFIG*1000/ (32768.0*GAIN); // mili volts per step. Use this conversion in place of Amps per step.
const float APS = PROG_GAIN_AMP_CONFIG*SHUNT_AMP_LIMIT / (32768.0*SHUNT_VOLTAGE_LIMIT*GAIN); // volts per step then multiplied by converion factor to get out amps per step (7.8125uV/step)*(200amps/(gain*0.05V))

// pin layout for arduino uno
// SCL to A5
// SDA to A4

// Input to ADS1115
// using inputs A0, and A1.


void setup() {

  Serial.begin(115200); // start the serial communication.

  Wire.begin(); // begin the I2C

  // To change what is written in the configure register refer to the register map in 9.6 of the ADS1115 datasheet.
  Wire.beginTransmission(ADS1115_1); // this tells the device with the given address to get ready for the subsequent bytes.
  Wire.write(0x01); // this selects which register to access. 0x01 selects the configure register.
  Wire.write(0x8a); // now enter in the msb (bits 15-8) we entered in 0x8a
  Wire.write(0x83); // now enter the lsb (bits 7-0) we entered 0x83
  Wire.endTransmission(ADS1115_1);

  delay(100); // add a brief delay so the ADS1115 has time to set up properly.

  // This is this to point to the conversion register and put an inital value into the measured value.
  Wire.beginTransmission(ADS1115_1); 
  Wire.write(0x00); // this selects the conversion register where the data is being stored.
  Wire.endTransmission();
  
}

void loop() {

  // now that we are pointing to the conversion register we can read the data from it.
  // the msb will be given first so shift it over 8 bits then use the or operator to add the lsb to measured value.
  Wire.beginTransmission(ADS1115_1); // When using request from it is important to use the begin and end transmission functions.
  Wire.requestFrom(ADS1115_1, 2); // this will get 2 bytes of data from the given address.
  measured_value = Wire.read() << 8; // This shifts the bits over 8 places to the left.
  measured_value |= Wire.read(); // This masks bits 8 to 15 and adds the rest of the data to bits 0 through 7.
  Wire.endTransmission();

  // account for bias if any
  measured_value += BIAS;

  Serial.println(measured_value * MVPS,6); // map to mV and print. Switch MVPS with APS to get Amps per step.

  delay(150); // add a delay

}
