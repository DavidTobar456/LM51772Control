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

    // Test SlopeComp_CorrectionFactor_Select with all possible values
    struct {
        uint8_t value;
        const char* description;
    } correctionFactors[] = {
        {SLOPECOMP_CORRECTION_0_125, "0.125"},
        {SLOPECOMP_CORRECTION_0_25, "0.25"},
        {SLOPECOMP_CORRECTION_0_375, "0.375"},
        {SLOPECOMP_CORRECTION_0_5, "0.5"},
        {SLOPECOMP_CORRECTION_0_625, "0.625"},
        {SLOPECOMP_CORRECTION_0_75, "0.75"},
        {SLOPECOMP_CORRECTION_0_875, "0.875"},
        {SLOPECOMP_CORRECTION_1_0, "1.0"},
        {SLOPECOMP_CORRECTION_1_5, "1.5"},
        {SLOPECOMP_CORRECTION_2_0, "2.0"},
        {SLOPECOMP_CORRECTION_2_5, "2.5"},
        {SLOPECOMP_CORRECTION_3_0, "3.0"},
        {SLOPECOMP_CORRECTION_3_5, "3.5"},
        {SLOPECOMP_CORRECTION_4_0, "4.0"},
        {SLOPECOMP_CORRECTION_4_5, "4.5"},
        {SLOPECOMP_CORRECTION_5_0, "5.0"}
    };

    I2C_WriteRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D7, 0x00);
    uint8_t value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D7);
    printf("Read value after RESET: 0x%02X\n", value);

    for (int i = 0; i < sizeof(correctionFactors) / sizeof(correctionFactors[0]); i++) {
        SlopeComp_CorrectionFactor_Select(SLAVE_ADDRESS, correctionFactors[i].value);
        value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D7);
        printf("Read value after SlopeComp_CorrectionFactor_Select (%s): 0x%02X\n", correctionFactors[i].description, value);
    }

    // Test SlopeComp_InductorDerating_Select with different derating values
    SlopeComp_InductorDerating_Select(SLAVE_ADDRESS, INDUC_DERATE_DISABLE);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D7);
    printf("Read value after SlopeComp_InductorDerating_Select (Disable): 0x%02X\n", value);

    SlopeComp_InductorDerating_Select(SLAVE_ADDRESS, INDUC_DERATE_20);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D7);
    printf("Read value after SlopeComp_InductorDerating_Select (20): 0x%02X\n", value);

    SlopeComp_InductorDerating_Select(SLAVE_ADDRESS, INDUC_DERATE_30);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D7);
    printf("Read value after SlopeComp_InductorDerating_Select (30): 0x%02X\n", value);

    SlopeComp_InductorDerating_Select(SLAVE_ADDRESS, INDUC_DERATE_40);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D7);
    printf("Read value after SlopeComp_InductorDerating_Select (40): 0x%02X\n", value);

    // Do not delete
    gpioTerminate();
    return 0;
}