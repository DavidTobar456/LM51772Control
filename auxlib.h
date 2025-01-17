#include <stdint.h>

extern void I2C_WriteRegByte(uint8_t SlaveAddress, uint8_t RegAddress, uint8_t ByteData);   //Write a byte to the device register via I2C
extern uint8_t I2C_ReadRegByte(uint8_t SlaveAddress, uint8_t RegAddress);                   //Read a byte from the device register via I2C

#ifndef I2C_AUXILIARY_FUNCTIONS_H
#define I2C_AUXILIARY_FUNCTIONS_H

// Function prototypes for setting and clearing bit 0
void setBit0(uint8_t I2CAddress, uint8_t Reg);
void clearBit0(uint8_t I2CAddress, uint8_t Reg);

// Function prototypes for setting and clearing bit 1
void setBit1(uint8_t I2CAddress, uint8_t Reg);
void clearBit1(uint8_t I2CAddress, uint8_t Reg);

// Function prototypes for setting and clearing bit 2
void setBit2(uint8_t I2CAddress, uint8_t Reg);
void clearBit2(uint8_t I2CAddress, uint8_t Reg);

// Function prototypes for setting and clearing bit 3
void setBit3(uint8_t I2CAddress, uint8_t Reg);
void clearBit3(uint8_t I2CAddress, uint8_t Reg);

// Function prototypes for setting and clearing bit 4
void setBit4(uint8_t I2CAddress, uint8_t Reg);
void clearBit4(uint8_t I2CAddress, uint8_t Reg);

// Function prototypes for setting and clearing bit 5
void setBit5(uint8_t I2CAddress, uint8_t Reg);
void clearBit5(uint8_t I2CAddress, uint8_t Reg);

// Function prototypes for setting and clearing bit 6
void setBit6(uint8_t I2CAddress, uint8_t Reg);
void clearBit6(uint8_t I2CAddress, uint8_t Reg);

// Function prototypes for setting and clearing bit 7
void setBit7(uint8_t I2CAddress, uint8_t Reg);
void clearBit7(uint8_t I2CAddress, uint8_t Reg);

#endif // I2C_AUXILIARY_FUNCTIONS_H
