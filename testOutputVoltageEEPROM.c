#include "LM51772.h"
#include <pigpio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define I2C_BUS 5
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
void I2C_WriteRegByte(uint8_t SlaveAddress, uint8_t RegAddress, uint8_t ByteData) {
    if (pollForDevice(SlaveAddress) != 0) {
        fprintf(stderr, "Device at address 0x%02X is not ready\n", SlaveAddress);
        return;
    }

    // We open the I2C handle for the writing operation
    int handle = i2cOpen(I2C_BUS, SlaveAddress, 0);
    if (handle < 0) {
        fprintf(stderr, "Failed to open I2C device at address 0x%02X\n", SlaveAddress);
        return;
    }
    //printf("Opened I2C link with device on address %02X, handle:%d\n", SlaveAddress, handle);

    // We prepare the data buffer
    char buff[3];
    buff[0] = 0x00;
    buff[1] = RegAddress;
    buff[2] = ByteData;

    // We write the device on said address the given data
    int status = i2cWriteDevice(handle, buff, 3);
    i2cClose(handle);

    if (status < 0) {
        fprintf(stderr, "Failed to write to I2C device at address 0x%02X\nERROR CODE:%d\n", SlaveAddress, status);
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

    // We prepare the data buffer
    char buff[2];
    buff[0] = 0;
    buff[1] = RegAddress;

    // We write the register we want to access
    int status = i2cWriteDevice(handle,buff,2);

    // We check whether or not the writing operation was successfull or not
    if (status < 0) {
        fprintf(stderr, "Failed to write to I2C device at address 0x%02X\nERROR CODE:%d\n", SlaveAddress,status);
    }

    // We prepare the reception buffer
    uint8_t buffer;
    // Read data byte from device's register
    status = i2cReadDevice(handle,&buffer,1);
    // Close the I2C device
    i2cClose(handle);

    // If the read operation failed, print error message and return 0
    if (status < 0) {
        fprintf(stderr, "Failed to read from register 0x%02X of I2C device at address 0x%02X\nERROR CODE:%d\n", RegAddress, SlaveAddress, status);
        return 0; // Return 0 to indicate failure
    }
    // Return the retrieved value
    return buffer;
}

void SoftwareDelay(uint8_t ms){
    usleep(ms*1000);
}

uint16_t getOutputVoltageTarget(float divValue, float Vout){
    float VRefF = (1/divValue)*Vout;
    printf("Target voltage on FB hast to be %.2f V for a %.2f Output Voltage\n",VRefF,Vout);
    uint16_t Vref = (uint16_t)(VRefF*1000.00);
    printf("The value on the VOUT_TARGET1 registers has to be %x to match this output\n",Vref);
    if(Vref>4096){
        float VoutMax = divValue*4096.00/1000;
        printf("ERROR: Vout %.1f V to high for current configuration, maximum achievable voltage for a configuration with a divider value uf %.1f Ohms is %.1f V\n",Vout,divValue,VoutMax);
        printf("Value for %.1f output is provided instead",VoutMax);
        Vref = 4096;
    }
    printf("VOUT Target value for a %.2f output voltage is: %d\n",Vout,Vref);
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
    printf("Input of a %.2f VOUT\n",Vout);
    float divValue = atof(argv[3]);
    printf("Input of a %.2f voltage divisor\n",divValue);

    // Initialize the pigpio library
    if (gpioInitialise() < 0) {
        fprintf(stderr, "pigpio initialization failed\n");
        return 1;
    }

    // Calculate the value to be loaded
    uint16_t Vref = getOutputVoltageTarget(divValue, Vout);
    printf("\nLoading 0x%X into the VOUT_TARGET1 registers-->...\n",Vref);
    // Set reference voltage
    setVOUT1_TARGET(I2CAddress, Vref);
    // Read loaded value to verify that the function works
    uint16_t VoutReal = getVOUT1_TARGET(I2CAddress);
    printf("\nValue of 0x%X currently loaded to VOUT_TARGET1 registers\n",VoutReal);
    // Do not delete
    gpioTerminate();
    return 0;
}
