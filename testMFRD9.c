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

int main() {
    // Initialize the pigpio library
    if (gpioInitialise() < 0) {
        fprintf(stderr, "pigpio initialization failed\n");
        return 1;
    }

    // Test PCM_LowerVoltageWindow_Configure with different dimensionless values
    I2C_WriteRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9, 0x00);
    uint8_t value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after RESET: 0x%02X\n", value);
    PCM_LowerVoltageWindow_Configure(SLAVE_ADDRESS, 0);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after PCM_LowerVoltageWindow_Configure (0): 0x%02X\n", value);

    PCM_LowerVoltageWindow_Configure(SLAVE_ADDRESS, 387);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after PCM_LowerVoltageWindow_Configure (387): 0x%02X\n", value);

    PCM_LowerVoltageWindow_Configure(SLAVE_ADDRESS, 775);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after PCM_LowerVoltageWindow_Configure (775): 0x%02X\n", value);

    // Test PCM_LowerVoltageWindow_ConfigureF with different percentage values
    I2C_WriteRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9, 0x00);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after RESET: 0x%02X\n", value);
    PCM_LowerVoltageWindow_ConfigureF(SLAVE_ADDRESS, 0.0);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after PCM_LowerVoltageWindow_ConfigureF (0.0%%): 0x%02X\n", value);

    PCM_LowerVoltageWindow_ConfigureF(SLAVE_ADDRESS, 38.75);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after PCM_LowerVoltageWindow_ConfigureF (38.75%%): 0x%02X\n", value);

    PCM_LowerVoltageWindow_ConfigureF(SLAVE_ADDRESS, 77.5);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after PCM_LowerVoltageWindow_ConfigureF (77.5%%): 0x%02X\n", value);

    // Test OCP_ISET_OverILIM_Enable
    I2C_WriteRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9, 0x00);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after RESET: 0x%02X\n", value);
    OCP_ISET_OverILIM_Enable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after OCP_ISET_OverILIM_Enable: 0x%02X\n", value);

    // Test OCP_ISET_OverILIM_Disable
    OCP_ISET_OverILIM_Disable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D9);
    printf("Read value after OCP_ISET_OverILIM_Disable: 0x%02X\n", value);

    // Do not delete
    gpioTerminate();
    return 0;
}