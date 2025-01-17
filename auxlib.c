#include "aux.h"
#include <stdint.h>

// Auxiliary functions helping development of firmware I2C libraries
void setBit0(uint8_t I2CAddress, uint8_t Reg){
    // Read the register contents
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 0 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x01);
}

void clearBit0(uint8_t I2CAddress, uint8_t Reg){
    // Read the register contents
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 0 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFE);
}

// Function to set bit 1 of a register at a given I2C address
void setBit1(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 1 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x02);
}

// Function to clear bit 1 of a register at a given I2C address
void clearBit1(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 1 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFD);
}

// Function to set bit 2 of a register at a given I2C address
void setBit2(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 2 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x04);
}

// Function to clear bit 2 of a register at a given I2C address
void clearBit2(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 2 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFB);
}

// Function to set bit 3 of a register at a given I2C address
void setBit3(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 3 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x08);
}

// Function to clear bit 3 of a register at a given I2C address
void clearBit3(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 3 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xF7);
}

// Function to set bit 4 of a register at a given I2C address
void setBit4(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 4 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x10);
}

// Function to clear bit 4 of a register at a given I2C address
void clearBit4(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 4 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xEF);
}

// Function to set bit 5 of a register at a given I2C address
void setBit5(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 5 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x20);
}

// Function to clear bit 5 of a register at a given I2C address
void clearBit5(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 5 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xDF);
}

// Function to set bit 6 of a register at a given I2C address
void setBit6(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 6 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x40);
}

// Function to clear bit 6 of a register at a given I2C address
void clearBit6(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 6 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xBF);
}

// Function to set bit 7 of a register at a given I2C address
void setBit7(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 7 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x80);
}

// Function to clear bit 7 of a register at a given I2C address
void clearBit7(uint8_t I2CAddress, uint8_t Reg){
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 7 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0x7F);
}
