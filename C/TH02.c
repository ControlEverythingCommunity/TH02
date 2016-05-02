// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TH02
// This code is designed to work with the TH02_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void main() 
{
	// Create I2C bus
	int file;
	char *bus = "/dev/i2c-1";
	if((file = open(bus, O_RDWR)) < 0) 
	{
		printf("Failed to open the bus. \n");
		exit(1);
	}
	// Get I2C device, TH02 I2C address is 0x40(64)
	ioctl(file, I2C_SLAVE, 0x40);

	// Select configuration register(0x03)
	// Normal mode enabled, Temperature(0x11)
	char config[2] = {0};
	config[0] = 0x03;
	config[1] = 0x11;
	write(file, config, 2);
	sleep(1);

	// Read 3 byte of data from register(0x00)
	// status, temp msb, temp lsb
	char reg[1] = {0x00};
	write(file, reg, 1);
	char data[3] = {0};
	if(read(file, data, 3) != 3)
	{
		printf("Error : Input/output Error \n");
		exit(1);
	}
	// Convert the data to 14-bits
	int temp = ((data[1] * 256) + (data[2] & 0xFC)) / 4;
	float cTemp = (temp / 32.0) - 50.0;
	float fTemp = cTemp * 1.8 + 32;

	// Output data to screen
	printf("Temperature in Celsius : %.2f C \n", cTemp);
	printf("Temperature in Fahrenheit : %.2f F \n", fTemp);

	// Select configuration register(0x03)
	// Normal mode enabled, Relative Humidity(0x01)
	config[0] = 0x03;
	config[1] = 0x01;
	write(file, config, 2);
	sleep(1);

	// Read 3 bytes of data
	// status, humidity msb, humidity lsb
	reg[0] = 0x00;
	write(file, reg, 1);
	read(file, data, 3);

	// Convert the data to 12-bits
	int hum = (((data[1] * 256) + (data[2] & 0xF0)) / 16)/ 16 - 24;
	float humidity = (hum - (((hum * hum) * (-0.00393)) + (hum * 0.4008) -4.7844));
	humidity = humidity + (cTemp - 30) * (humidity * 0.00237 + 0.1937);
	
	// Output data to screen
	printf("Relative Humidity : %.2f RH \n", humidity);
}
