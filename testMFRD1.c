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

    // Test PSM_2PhaseBB_Enable
    I2C_WriteRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1, 0x00);
    uint8_t value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after RESET: 0x%02X\n", value);
    PSM_2PhaseBB_Enable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after PSM_2PhaseBB_Enable: 0x%02X\n", value);

    // Test PSM_2PhaseBB_Disable
    PSM_2PhaseBB_Disable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after PSM_2PhaseBB_Disable: 0x%02X\n", value);

    // Test FPWM_2PhaseBB_Enable
    FPWM_2PhaseBB_Enable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after FPWM_2PhaseBB_Enable: 0x%02X\n", value);

    // Test FPWM_2PhaseBB_Disable
    FPWM_2PhaseBB_Disable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after FPWM_2PhaseBB_Disable: 0x%02X\n", value);

    // Test ForceBias_Enable
    ForceBias_Enable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after ForceBias_Enable: 0x%02X\n", value);

    // Test ForceBias_Disable
    ForceBias_Disable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after ForceBias_Disable: 0x%02X\n", value);

    // Test DTRK_DirectStartup_Enable
    DTRK_DirectStartup_Enable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after DTRK_DirectStartup_Enable: 0x%02X\n", value);

    // Test DTRK_DirectStartup_Disable
    DTRK_DirectStartup_Disable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after DTRK_DirectStartup_Disable: 0x%02X\n", value);

    // Test nFLT_as_INT_Enable
    nFLT_as_INT_Enable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after nFLT_as_INT_Enable: 0x%02X\n", value);

    // Test nFLT_as_INT_Disable
    nFLT_as_INT_Disable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after nFLT_as_INT_Disable: 0x%02X\n", value);

    // Test ThermalWarning_ThresholdConfigure with different thresholds
    ThermalWarning_ThresholdConfigure(SLAVE_ADDRESS, THW_THRESHOLD_140degC);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after ThermalWarning_ThresholdConfigure (140degC): 0x%02X\n", value);

    ThermalWarning_ThresholdConfigure(SLAVE_ADDRESS, THW_THRESHOLD_125degC);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after ThermalWarning_ThresholdConfigure (125degC): 0x%02X\n", value);

    ThermalWarning_ThresholdConfigure(SLAVE_ADDRESS, THW_THRESHOLD_110degC);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after ThermalWarning_ThresholdConfigure (110degC): 0x%02X\n", value);

    ThermalWarning_ThresholdConfigure(SLAVE_ADDRESS, THW_THRESHOLD_95degC);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after ThermalWarning_ThresholdConfigure (95degC): 0x%02X\n", value);

    ThermalWarning_ThresholdConfigure(SLAVE_ADDRESS, THW_THRESHOLD_140degC);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after ThermalWarning_ThresholdConfigure (140degC): 0x%02X\n", value);

    // Test ThermalWarning_Enable
    ThermalWarning_Enable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after ThermalWarning_Enable: 0x%02X\n", value);

    // Test ThermalWarning_Disable
    ThermalWarning_Disable(SLAVE_ADDRESS);
    value = I2C_ReadRegByte(SLAVE_ADDRESS, MFR_SPECIFIC_D1);
    printf("Read value after ThermalWarning_Disable: 0x%02X\n", value);

    // Do not delete
    gpioTerminate();
    return 0;
}