// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TH02
// This code is designed to work with the TH02_I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

#include <application.h>
#include <spark_wiring_i2c.h>

// TH02 I2C address is 0x40(64)
#define Addr 0x40

float cTemp = 0.0, humidity = 0.0;
void setup()
{
  // Set variables
  Particle.variable("i2cdevice", "TH02");
  Particle.variable("cTemp", cTemp);
  Particle.variable("humidity", humidity);
  
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Initialise serial communication, set baud rate = 9600
  Serial.begin(9600);

  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(300);
}

void loop()
{
  unsigned int data[3];
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select configuration register
  Wire.write(0x03);
  // Normal mode enabled, Temperature
  Wire.write(0x11);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(500);
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x00);
  // Stop I2C transmission
  Wire.endTransmission();
  
  // Request 3 bytes of data
  Wire.requestFrom(Addr, 3);

  // Read 3 bytes of data
  // Status, temp msb, temp lsb
  if(Wire.available() == 3)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
  }

  // Convert the data to 14-bits
  float temp = ((data[1] * 256.0) + (data[2] & 0xFC)) / 4;
  float cTemp = (temp / 32.0) - 50.0;
  float fTemp = cTemp * 1.8 + 32 ;
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select configuration register
  Wire.write(0x03);
  // Normal mode enabled, Relative Humidity
  Wire.write(0x01);
  // Stop I2C transmission
  Wire.endTransmission();
  delay(500);
  
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x00);
  // Stop I2C transmission
  Wire.endTransmission();

  // Request 3 bytes of data
  Wire.requestFrom(Addr, 3);

  // Read 3 bytes of data
  // Status, humidity msb, humidity lsb
  if(Wire.available() == 3)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
  }
  
  // Convert the data to 12-bits
  float hum = ((((data[1] * 256.0) + (data[2] & 0xF0)) / 16) / 16 ) - 24;
  float humidity = (hum * 1.0 - (((hum * 1.0 * hum * 1.0) * (-0.00393)) + (hum * 0.4008) - 4.7844));
  humidity = humidity + ((cTemp - 30) * ((humidity * 0.00237) + 0.1937));

  // Output data to dashboard
  Particle.publish("Relative Humidity         : ", String(humidity));
  Particle.publish("Temperature in Celsius    : ", String(cTemp));
  Particle.publish("Temperature in Fahrenheit : ", String(fTemp));
}

