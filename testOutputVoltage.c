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
    if (pollForDevice(SlaveAddress) != 0) {
        fprintf(stderr, "Device at address 0x%02X is not ready\n", SlaveAddress);
        return;
    }

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
    if (pollForDevice(SlaveAddress) != 0) {
        fprintf(stderr, "Device at address 0x%02X is not ready\n", SlaveAddress);
        return 0;
    }
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

uint16_t getOutputVoltageTarget(float divValue, float Vout){
    float VRefF = (1/divValue)*Vout;
    uint16_t Vref = (uint16_t)VRefF*1000;
    if(Vref>4096){
        float VoutMax = ((R1+R2)/R2)*4096.00;
        printf("ERROR: Vout %.1f V to high for current configuration, maximum achievable voltage for a configuration with R1 = %.1f Ohms and R2 = %.1f Ohms is %.1f V\n",Vout,R1,R2,VoutMax);
        printf("Value for %.1f output is provided instead",VoutMax);
        Vref = 4096;
    }
    return Vref;
}

int main(int argc, char *argv[]){
    // Check if the correct number of arguments is provided
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <I2CAddress> <Vout> <divValue>\n", argv[0]);
        return 1;
    }

    // Parse input arguments
    uint8_t I2CAddress = (uint8_t)strtol(argv[1], NULL, 0);
    float Vout = atof(argv[2]);
    float divValue = atof(argv[3]);

    // Initialize the pigpio library
    if (gpioInitialise() < 0) {
        fprintf(stderr, "pigpio initialization failed\n");
        return 1;
    }

    // Calculate the value to be loaded
    uint16_t Vref = getReferenceVoltage(divValue, Vout);

    // Set reference voltage
    setVOUT1_TARGET(I2CAddress, Vref);

    // Do not delete
    gpioTerminate();
    return 0;
}
