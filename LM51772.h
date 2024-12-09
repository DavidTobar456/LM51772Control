#include <stdint.h>

//To use this library, you need to provide the following external functions, which are the functions that the SC8815 library needs to use
extern void I2C_WriteRegByte(uint8_t SlaveAddress, uint8_t RegAddress, uint8_t ByteData);   //Write a byte to the device register via I2C
extern uint8_t I2C_ReadRegByte(uint8_t SlaveAddress, uint8_t RegAddress);                   //Read a byte from the device register via I2C
extern void SoftwareDelay(uint8_t ms);                                                      //Software delay in milliseconds

// I2C addressing definitions
#define LM51772_I2CADDR1                0x6A
#define LM51772_I2CADDR2                0x6B

// LM51772 register definitions     
#define CLEAR_FAULTS                    0x3
#define ILIM_THRESHOLD                  0xA
#define VOUT_TARGET1_LSB                0xC
#define VOUT_TARGET1_MSB                0xD
#define USB_PD_STATUS_0                 0x21
#define STATUS_BYTE                     0x78
#define USB_PD_CONTROL_0                0x81
#define MFR_SPECIFIC_D0                 0xD0
#define MFR_SPECIFIC_D1                 0xD1
#define MFR_SPECIFIC_D2                 0xD2
#define MFR_SPECIFIC_D3                 0xD3
#define MFR_SPECIFIC_D4                 0xD4
#define MFR_SPECIFIC_D5                 0xD5
#define MFR_SPECIFIC_D6                 0xD6
#define MFR_SPECIFIC_D7                 0xD7
#define MFR_SPECIFIC_D8                 0xD8
#define MFR_SPECIFIC_D9                 0xD9
#define IVP_VOLTAGE                     0xDA

// LM51772 - ILIM Threshold definitions
// Value lower and upper bouds for the threshold, the values in between
// are where the variable ILIM range is
#define ILIM_THRESHOLD_LBOUND           0x0A // Values under 0x0A all set a 5mV threshold
#define ILIM_THRESHOLD_HBOUND           0x8C // Values from 0x8C onwards set a 70mV threshold


// LM51772 - VOUT Aux definitions
#define VOUT_MSB_RMASK                  0x07
#define VOUT_MSB_WMASK                  0x07

// LM51772 - USB_PD_CONTROL auxiliary definitions
// Discharge functions
#define USBPDCNTRL_DISCHG_RMASK         0x02
#define USBPDCNTRL_DISCHG_WMASK         0xFD
#define USBPDCNTRL_DISCHG_DISABLE       0x00
#define USBPDCNTRL_DISCHG_ENABLE        0x02
// Power stage ON/OFF
#define USBPDCNTRL_ENCONV_RMASK         0x01
#define USBPDCNTRL_ENCONV_WMASK         0xFE
#define USBPDCNTRL_ENCONV_DISABLE       0x00
#define USBPDCNTRL_ENCONV_ENABLE        0x01

// Function definitions
// Functions for ILIM_THRESHOLD modifications
void setILIM_THRESHOLD_Voltage(uint8_t I2CAddress, float ILIMVoltage);
float getILIM_THRESHOLD_Voltage(uint8_t I2CAddress);
// Functions for VOUT_TARGET1 registers
// Setting of the VOUT target
void setVOUT1_TARGET(uint8_t I2CAddress, uint16_t VoutTarget);
uint16_t getVOUT1_TARGET(uint8_t I2CAddress);
// Function for 20mV increments
void VOUT1_TARGET_Inc_20mV(uint8_t I2CAddress, uint16_t nInc);
// Function for 20mV decrements
void VOUT1_TARGET_Dec_20mV(uint8_t I2CAddress, uint16_t nDec);
// Function for 100mV increments
void VOUT1_TARGET_Inc_100mV(uint8_t I2CAddress, uint16_t nInc);
// Function for 100mV decrements
void VOUT1_TARGET_Dec_100mV(uint8_t I2CAddress, uint16_t nDec);


