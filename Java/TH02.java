// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// TH02
// This code is designed to work with the TH02_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import java.io.IOException;

public class TH02
{
	public static void main(String args[]) throws Exception
	{
		// Create I2C bus
		I2CBus bus = I2CFactory.getInstance(I2CBus.BUS_1);
		// Get I2C device, TH02 I2C address is 0x40(64)
		I2CDevice device = bus.getDevice(0x40);

		// Select configuration register
		// Normal mode enabled, Temperature 
		device.write(0x03, (byte)0x11);
		Thread.sleep(500);

		// Read 3 bytes of data
		// status, temp msb, temp lsb
		byte[] data = new byte[3];
		device.read(0x00, data, 0, 3);

		// Convert the data to 14-bits
		int temp = (((data[1] & 0xFF) * 256) + (data[2] & 0xFC)) / 4;
		double cTemp = (temp / 32.0) - 50.0;
		double fTemp = cTemp * 1.8 + 32;

		// Select configuration register
		// Normal mode enabled, Relative Humidity 
		device.write(0x03, (byte)0x01);
		Thread.sleep(500);

		// Read 3 bytes of data
		// status, humidity msb, humidity lsb
		device.read(0x00, data, 0, 3);

		// Convert the data to 12-bits
		int hum = ((((data[1] & 0xFF) * 256) + (data[2] & 0xF0)) / 16)/ 16 - 24;
		double humidity = (hum - (((hum * hum) * (-0.00393)) + (hum * 0.4008) -4.7844));
		humidity = humidity + (cTemp - 30) * (humidity * 0.00237 + 0.1937);

		// Output data to screen
		System.out.printf("Relative Humidity : %.2f %% RH %n", humidity);
		System.out.printf("Temperature in Celsius : %.2f C %n", cTemp);
		System.out.printf("Temperature in Fahrenheit : %.2f F %n", fTemp);
	}
}