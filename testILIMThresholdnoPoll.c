#include "LM51772.h"
#include <pigpio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define I2C_BUS 3
#define SLAVE_ADDRESS 0x50
#define POLL_DELAY 100 // Microseconds
#define POLL_RETRIES 100

// Function to poll for device readiness
int pollForDevice(uint8_t SlaveAddress) {
    int handle;
    for (int i = 0; i < POLL_RETRIES; ++i) {
        handle = i2cOpen(I2C_BUS, SlaveAddress, 0);
        if (handle >= 0) {
            int status = i2cWriteQuick(handle, 0); // Send a quick write operation
            i2cClose(handle);
            if (status == 0) {
                //printf("Number of iterations needed for device to be ready: %d\n",i);
                return 0; // Device is ready
            }
        }
        usleep(POLL_DELAY);
    }
    return -1; // Device is not ready after maximum retries
}

// We define the writing function
void I2C_WriteRegByte(uint8_t SlaveAddress, uint8_t RegAddress, uint8_t ByteData){
    /*if (pollForDevice(SlaveAddress) != 0) {
        fprintf(stderr, "Device at address 0x%02X is not ready\n", SlaveAddress);
        //return;
    }*/

    // We open the I2C handle for the reading operation
    int handle = i2cOpen(I2C_BUS,SlaveAddress,0);
    // If we fail to open handle we raise error
    if (handle < 0) {
        fprintf(stderr, "Failed to open I2C device at address 0x%02X\n", SlaveAddress);
        return;
    }

    // We prepare the data buffer
    char buff[2];
    buff[0] = RegAddress;
    buff[1] = ByteData;

    // We write the device on said address the given data
    int status = i2cWriteDevice(handle,buff,2);
    // Close the I2C device
    i2cClose(handle);

    // We check whether or not the writing operation was successfull or not
    if (status < 0) {
        fprintf(stderr, "Failed to write to I2C device at address 0x%02X\n", SlaveAddress);
    }
}

// We define the reading function
uint8_t I2C_ReadRegByte(uint8_t SlaveAddress, uint8_t RegAddress){
    /*if (pollForDevice(SlaveAddress) != 0) {
        fprintf(stderr, "Device at address 0x%02X is not ready\n", SlaveAddress);
        return 0;
    }*/
    // We open the I2C handle for the reading operation
    int handle = i2cOpen(I2C_BUS,SlaveAddress,0);
    // If we fail to open handle we raise error
    if (handle < 0) {
        fprintf(stderr, "Failed to open I2C device at address 0x%02X\n", SlaveAddress);
        return 0;
    }
    // Read data byte from device's register
    int status = i2cReadByteData(handle,RegAddress);
    // Close the I2C device
    i2cClose(handle);

    // If the read operation failed, print error message and return 0
    if (status < 0) {
        fprintf(stderr, "Failed to read from register 0x%02X of I2C device at address 0x%02X\n", RegAddress, SlaveAddress);
        return 0; // Return 0 to indicate failure
    }
    // Return the retrieved value
    return (uint8_t)status;
}

void SoftwareDelay(uint8_t ms){
    usleep(ms*1000);
}

uint16_t getILIMThresholdValue(float ILIMThreshold, float Rshunt){
    float ILIMThresholdmAmpsF = ILIMThreshold*1000.0;
    printf("ILIM threshold in mA: %.2f as a float\n",ILIMThresholdmAmpsF);
    uint16_t ILIMThresholdmAmps = (uint16_t)(ILIMThresholdmAmpsF);
    return ILIMThresholdmAmps;
}

int main(int argc, char *argv[]){
    // Check if the correct number of arguments is provided
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <I2CAddress> <ILIM Threshold in Amps> <RShunt in Ohms>\n", argv[0]);
        return 1;
    }

    // Parse input arguments
    uint8_t I2CAddress = (uint8_t)strtol(argv[1], NULL, 0);
    float ILIMThreshold = atof(argv[2]);
    printf("Input of a %.2f A ILIM threshold\n",ILIMThreshold);
    float Rshunt = atof(argv[3]);
    printf("Input of a %.2f Ohms shunt resistor\n",Rshunt);

    // Initialize the pigpio library
    if (gpioInitialise() < 0) {
        fprintf(stderr, "pigpio initialization failed\n");
        return 1;
    }

    // Calculate ILIMThreshold in mV
    uint16_t ILIMThresholdmAmps = getILIMThresholdValue(ILIMThreshold,Rshunt);
    printf("ILIM Threshold in mA is: %d\n",ILIMThresholdmAmps);
    // Write the ILIM on the ILIM_THRESHOLD register
    setILIM_THRESHOLD_Voltage(I2CAddress,ILIMThresholdmAmps);
    // Verify the value written
    // uint8_t ILIM_realValue = I2C_ReadRegByte(I2CAddress,ILIM_THRESHOLD);
    // printf("\nRead 0x%X from the ILIM_THRESHOLD register\n",ILIM_realValue);
    // Do not delete
    gpioTerminate();
    return 0;
}
