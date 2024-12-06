#include "MPQ4210.h"
#include <pigpio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

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

int main(){
    // Initialize the pigpio library
    if (gpioInitialise() < 0) {
        fprintf(stderr, "pigpio initialization failed\n");
        return 1;
    }
    // Loading of the tested register on the I2C EEPROM
    printf("Now loading the default configuration of the VREF and ILIM registers\n");
    I2C_WriteRegByte(SLAVE_ADDRESS,MPQREG_REF_LSB,0b00000100);
    I2C_WriteRegByte(SLAVE_ADDRESS,MPQREG_REF_MSB,0b00111110);
    I2C_WriteRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1,0b01000000);
    I2C_WriteRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2,0b10000101);
    I2C_WriteRegByte(SLAVE_ADDRESS,MPQREG_ILIM,0b00001001);
    I2C_WriteRegByte(SLAVE_ADDRESS,MPQREG_INT_STATUS,0b00000000);
    I2C_WriteRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK,0b00000001);

    // Test of the library functions which change VREF
    // Setting voltage to 1.1 V
    // Values to be set should go as following
    // REF_LSB = 100
    // REF_MSB = 10001001
    uint8_t rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_REF_LSB);
    printf("REF_LSB reg: 0x%X",rcvd);
    rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_REF_MSB);
    printf("\tREF_MSB reg: 0x%X\n",rcvd);
    printf("Setting voltage reference to 1.1V, VOUT should be 11V\n");
    MPQ_SetVoltageReference(SLAVE_ADDRESS,1100);
    printf("REF_LSB reg: 0x%X",rcvd);
    rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_REF_MSB);
    printf("\tREF_MSB reg: 0x%X\n\n",rcvd);
    I2C_WriteRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1,0b01000000);

    // // Test of MPQ_DisablePowerSwitching() and MPQ_EnablePowerSwitching()
    // // functions
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Enabling Power switching...\n");
    // MPQ_EnablePowerSwitching(SLAVE_ADDRESS);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Disabling Power switching...\n");
    // MPQ_DisablePowerSwitching(SLAVE_ADDRESS);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n\n",rcvd);

    // // Test of MPQ_SET_GOBIT() function
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Setting Go bit...\n");
    // MPQ_SET_GOBIT(SLAVE_ADDRESS);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n\n",rcvd);
    // I2C_WriteRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1,0b01000000);

    // // Test of MPQ_PNG_Latch_Enable() and MPQ_PNG_Latch_Disable()
    // // functions
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Enabling PNG latch functionality...\n");
    // MPQ_PNG_Latch_Enable(SLAVE_ADDRESS);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Disabling PNG latch functionality...\n");
    // MPQ_PNG_Latch_Disable(SLAVE_ADDRESS);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n\n",rcvd);

    // // Test of MPQ_FreqSpreadSpectrum_Enable() and MPQ_FreqSpreadSpectrum_Disable()
    // // functions
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Enabling frequency spread spectrum functionality...\n");
    // MPQ_FreqSpreadSpectrum_Enable(SLAVE_ADDRESS);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Disabling frequency spread spectrum functionality...\n");
    // MPQ_FreqSpreadSpectrum_Disable(SLAVE_ADDRESS);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n\n",rcvd);

    // // Test of MPQ_OutputDischargePath_Enable() and MPQ_OutputDischargePath_Disable()
    // // functions
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Enabling output discharge path to ground...\n");
    // MPQ_OutputDischargePath_Enable(SLAVE_ADDRESS);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Disabling output discharge path to ground...\n");
    // MPQ_OutputDischargePath_Disable(SLAVE_ADDRESS);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n\n",rcvd);

    // // Test of MPQ_SetVREF_SlewRate() function
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Setting SR to 38mV/ms MPQ4210...\n");
    // MPQ_SetVREF_SlewRate(SLAVE_ADDRESS,MPQ4210_CONTROL1_SR_38mV_ms);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Setting SR to 50mV/ms MPQ4210...\n");
    // MPQ_SetVREF_SlewRate(SLAVE_ADDRESS,MPQ4210_CONTROL1_SR_50mV_ms);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Setting SR to 75mV/ms MPQ4210...\n");
    // MPQ_SetVREF_SlewRate(SLAVE_ADDRESS,MPQ4210_CONTROL1_SR_75mV_ms);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Setting SR to 150mV/ms MPQ4210...\n");
    // MPQ_SetVREF_SlewRate(SLAVE_ADDRESS,MPQ4210_CONTROL1_SR_150mV_ms);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Setting SR to 38mV/ms MPQ4214...\n");
    // MPQ_SetVREF_SlewRate(SLAVE_ADDRESS,MPQ4214_CONTROL1_SR_38mV_ms);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Setting SR to 50mV/ms MPQ4214...\n");
    // MPQ_SetVREF_SlewRate(SLAVE_ADDRESS,MPQ4214_CONTROL1_SR_50mV_ms);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Setting SR to 72mV/ms MPQ4214...\n");
    // MPQ_SetVREF_SlewRate(SLAVE_ADDRESS,MPQ4214_CONTROL1_SR_72mV_ms);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n",rcvd);
    // printf("Setting SR to 150mV/ms MPQ4214...\n");
    // MPQ_SetVREF_SlewRate(SLAVE_ADDRESS,MPQ4214_CONTROL1_SR_150mV_ms);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL1);
    // printf("CONTROL1 reg: 0x%X\n\n",rcvd);

    // // Test of MPQ_SetSwitchingFrequency() function
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting switching frequency to 200kHz...\n");
    // MPQ_SetSwitchingFrequency(SLAVE_ADDRESS,MPQ_CONTROL2_FSW_200khz);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting switching frequency to 300kHz...\n");
    // MPQ_SetSwitchingFrequency(SLAVE_ADDRESS,MPQ_CONTROL2_FSW_300khz);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting switching frequency to 400kHz...\n");
    // MPQ_SetSwitchingFrequency(SLAVE_ADDRESS,MPQ_CONTROL2_FSW_400khz);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting switching frequency to 600kHz...\n");
    // MPQ_SetSwitchingFrequency(SLAVE_ADDRESS,MPQ_CONTROL2_FSW_600khz);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n\n",rcvd);

    // // Test of MPQ_Set_BB_FSW() function
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting BB_FSW to higher switching frequency MPQ4210...\n");
    // MPQ_Set_BB_FSW(SLAVE_ADDRESS,MPQ4210_CONTROL2_BBFSW_HIGH);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting BB_FSW to lower switching frequency MPQ4210...\n");
    // MPQ_Set_BB_FSW(SLAVE_ADDRESS,MPQ4210_CONTROL2_BBFSW_LOW);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting BB_FSW to higher switching frequency MPQ4214...\n");
    // MPQ_Set_BB_FSW(SLAVE_ADDRESS,MPQ4214_CONTROL2_BBFSW_HIGH);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting BB_FSW to lower switching frequency MPQ4214...\n");
    // MPQ_Set_BB_FSW(SLAVE_ADDRESS,MPQ4214_CONTROL2_BBFSW_LOW);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n\n",rcvd);

    // // Test of MPQ_setOCPMode() function
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting OCP mode to no protection...\n");
    // MPQ_setOCPMode(SLAVE_ADDRESS,MPQ_CONTROL2_OCP_MODE_NONE);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting OCP mode to Hiccup protection...\n");
    // MPQ_setOCPMode(SLAVE_ADDRESS,MPQ_CONTROL2_OCP_MODE_HICCUP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting OCP mode to Latch off protection...\n");
    // MPQ_setOCPMode(SLAVE_ADDRESS,MPQ_CONTROL2_OCP_MODE_LATCH);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n\n",rcvd);

    // // Test of MPQ_setOVPMode() function
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting OVP mode to no protection...\n");
    // MPQ_setOVPMode(SLAVE_ADDRESS,MPQ_CONTROL2_OVP_MODE_NONE);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting OVP mode to Hiccup protection...\n");
    // MPQ_setOVPMode(SLAVE_ADDRESS,MPQ_CONTROL2_OVP_MODE_HICCUP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);
    // printf("Setting OVP mode to Latch Off protection...\n");
    // MPQ_setOVPMode(SLAVE_ADDRESS,MPQ_CONTROL2_OVP_MODE_LATCH);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_CONTROL2);
    // printf("CONTROL2 reg: 0x%X\n",rcvd);

    // // Test of MPQ_setILIM() function
    // uint8_t rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 27.9 mV current limit threshold in MPQ4210...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4210_ILIM_27_9mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 33.3 mV current limit threshold in MPQ4210...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4210_ILIM_33_3mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 39.3 mV current limit threshold in MPQ4210...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4210_ILIM_39_3mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 45.1 mV current limit threshold in MPQ4210...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4210_ILIM_45_1mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 51.2 mV current limit threshold in MPQ4210...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4210_ILIM_51_2mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 56.8 mV current limit threshold in MPQ4210...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4210_ILIM_56_8mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 62.8 mV current limit threshold in MPQ4210...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4210_ILIM_62_8mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 68.7 mV current limit threshold in MPQ4210...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4210_ILIM_68_7mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 26 mV current limit threshold in MPQ4214...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4214_ILIM_26mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 32 mV current limit threshold in MPQ4214...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4214_ILIM_32mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 38 mV current limit threshold in MPQ4214...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4214_ILIM_38mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 45 mV current limit threshold in MPQ4214...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4214_ILIM_45mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 50 mV current limit threshold in MPQ4214...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4214_ILIM_50mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 56 mV current limit threshold in MPQ4214...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4214_ILIM_56mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 62 mV current limit threshold in MPQ4214...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4214_ILIM_62mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n",rcvd);
    // printf("Setting 68 mV current limit threshold in MPQ4214...\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4214_ILIM_68mV);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_ILIM);
    // printf("ILIM reg: 0x%X\n\n",rcvd);

    // // Test of MPQ_IntClear() function
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_STATUS);
    // printf("Interrupt Status reg: 0x%X\n",rcvd);
    // printf("Clearing interrupt status...\n");
    // MPQ_IntClear(SLAVE_ADDRESS);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_STATUS);
    // printf("Interrupt Status reg: 0x%X\n\n",rcvd);

    // // Test of MPQ_IntEnable() and MPQ_IntDisable() functions
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Disabling PNG interrupt in MPQ4210...\n");
    // MPQ_IntDisable(SLAVE_ADDRESS,MPQ4210_INT_PNG);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Enabling PNG interrupt in MPQ4210...\n");
    // MPQ_IntEnable(SLAVE_ADDRESS,MPQ4210_INT_PNG);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Enabling OCP interrupt in MPQ4210...\n");
    // MPQ_IntEnable(SLAVE_ADDRESS,MPQ4210_INT_OCP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Disabling OCP interrupt in MPQ4210...\n");
    // MPQ_IntDisable(SLAVE_ADDRESS,MPQ4210_INT_OCP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Enabling OVP interrupt in MPQ4210...\n");
    // MPQ_IntEnable(SLAVE_ADDRESS,MPQ4210_INT_OVP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Disabling OVP interrupt in MPQ4210...\n");
    // MPQ_IntDisable(SLAVE_ADDRESS,MPQ4210_INT_OVP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Enabling OTP interrupt in MPQ4210...\n");
    // MPQ_IntEnable(SLAVE_ADDRESS,MPQ4210_INT_OTP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Disabling OTP interrupt in MPQ4210...\n");
    // MPQ_IntDisable(SLAVE_ADDRESS,MPQ4210_INT_OTP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Disabling PNG interrupt in MPQ4214...\n");
    // MPQ_IntDisable(SLAVE_ADDRESS,MPQ4214_INT_PNG);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Enabling PNG interrupt in MPQ4214...\n");
    // MPQ_IntEnable(SLAVE_ADDRESS,MPQ4214_INT_PNG);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Enabling OCP interrupt in MPQ4214...\n");
    // MPQ_IntEnable(SLAVE_ADDRESS,MPQ4214_INT_OCP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Disabling OCP interrupt in MPQ4214...\n");
    // MPQ_IntDisable(SLAVE_ADDRESS,MPQ4214_INT_OCP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Enabling OVP interrupt in MPQ4214...\n");
    // MPQ_IntEnable(SLAVE_ADDRESS,MPQ4214_INT_OVP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Disabling OVP interrupt in MPQ4214...\n");
    // MPQ_IntDisable(SLAVE_ADDRESS,MPQ4214_INT_OVP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Enabling OTP interrupt in MPQ4214...\n");
    // MPQ_IntEnable(SLAVE_ADDRESS,MPQ4214_INT_OTP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Disabling OTP interrupt in MPQ4214...\n");
    // MPQ_IntDisable(SLAVE_ADDRESS,MPQ4214_INT_OTP);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Enabling CC interrupt in MPQ4214...\n");
    // MPQ_IntEnable(SLAVE_ADDRESS,MPQ4214_INT_CC);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);
    // printf("Disabling CC interrupt in MPQ4214...\n");
    // MPQ_IntDisable(SLAVE_ADDRESS,MPQ4214_INT_CC);
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,MPQREG_INT_MASK);
    // printf("Interrupt Mask reg: 0x%X\n",rcvd);

    // Setting ILIM threshold to 45.1mV
    // Value of the ILIM register should be set as follows
    // ILIM = 011
    // printf("Setting ILIM threshold to 45.1mV\n\n");
    // MPQ_setILIM(SLAVE_ADDRESS,MPQ4210_ILIM_45_1mV);

    // Verification of what has been written
    // Verification of what has been written on the eeprom
    // printf("Now reading the loaded configuration\n\n");
    // RegAddress = MPQREG_REF_LSB;
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,RegAddress);

    // printf("Read 0x%X from register 0x%X\n", rcvd, RegAddress);
    // RegAddress = MPQREG_REF_MSB;
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,RegAddress);
    // printf("Read 0x%X from register 0x%X\n", rcvd, RegAddress);
    // RegAddress = MPQREG_CONTROL1;
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,RegAddress);
    // printf("Read 0x%X from register 0x%X\n", rcvd, RegAddress);
    // RegAddress = MPQREG_ILIM;
    // rcvd = I2C_ReadRegByte(SLAVE_ADDRESS,RegAddress);
    // printf("Read 0x%X from register 0x%X\n\n", rcvd, RegAddress);

    // Continue with the rest of your main function

    gpioTerminate();
    return 0;
}
