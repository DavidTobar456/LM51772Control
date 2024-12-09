//Include header file
#include "LM51772.h"
#include "stdio.h"
/******************************************
* @brief: Configuration of the ILIM Threshold
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param ILIMVoltage: ILIM threshold voltage given in mV (float)
* @note: Sets the ILIM_THRESHOLD register value to match the ILIMVoltage
*        provided by the user.
*******************************************/
void setILIM_THRESHOLD_Voltage(uint8_t I2CAddress, float ILIMVoltage){
    // Ensure the input value is inside the 5 to 70 mV range.
    if (ILIMVoltage < 5.0 || ILIMVoltage > 70.0){
        fprintf(stderr, "Error: ILIMVoltage out of range. Must be between 5 and 70. %.2f was given.\n", ILIMVoltage);
        return;
    }
    // Convert from float to equivalent value
    uint8_t ilimValue; 
    ilimValue = (uint8_t)(2*ILIMVoltage);
    // Write the equivalent value to the ILIM_THRESHOLD register
    I2C_WriteRegByte(I2CAddress, ILIM_THRESHOLD, ilimValue);
}

/******************************************
* @brief: Returns de current ILIM_THRESHOLD value in mV
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes the conversion from the register current value to the
*        threshold voltage in mV and returns it.
*******************************************/
float getILIM_THRESHOLD_Voltage(uint8_t I2CAddress){
    // Read the current value on the ILIM_THRESHOLD register
    uint8_t thresholdValue;
    thresholdValue = I2C_ReadRegByte(I2CAddress, ILIM_THRESHOLD);

    float thresholdVoltage;
    
    // Apply limits to thresholdValue
    if (thresholdValue < 10){
        thresholdVoltage = 5.0; // Corresponds to 5.0 volts
    } 
    else if(thresholdValue > 140){
        thresholdVoltage = 70.0; // Corresponds to 70.0 volts
    } 
    else{
        // Convert register value to threshold voltage in mV
        thresholdVoltage = (float)(0.5 * thresholdValue);
    }
    // Return the current threshold in mV
    return thresholdVoltage;
}

/******************************************
* @brief: Sets the VOUT1_TARGET MSB and LSB registers
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param VoutTarget: Target value of VOUT to be set on registers (uint16_t)
* @note: Takes the VoutTarget, separates it in two bytes and writes it on
*        the VOUT_TARGET1_MSB and VOUT_TARGET1_LSB registers
*******************************************/
void setVOUT1_TARGET(uint8_t I2CAddress, uint16_t VoutTarget){
    // Separate VoutTarget on two separate bytes
    uint8_t VoutTargetMSB,VoutTargetLSB;
    VoutTargetLSB = (uint8_t)(VoutTarget & 0xFF);
    VoutTargetMSB = (uint8_t)((VoutTarget >> 8) & 0x0F);
    // Write VOUT_TARGET_LSB register
    printf("Loading 0x%X to the VOUT_TARGET1_LSB register\n",VoutTargetLSB);
    I2C_WriteRegByte(I2CAddress,VOUT_TARGET1_LSB,VoutTargetLSB);
    // Write VOUT_TARGET_MSB register
    printf("Loading 0x%X to the VOUT_TARGET1_MSB register\n\n",VoutTargetMSB);
    I2C_WriteRegByte(I2CAddress,VOUT_TARGET1_MSB,VoutTargetMSB);
}

/******************************************
* @brief: Gets the VOUT1_TARGET from MSB and LSB registers
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Reads the VOUT1_TARGET registers and concats them to
*        return the target value which multiplied by the voltage
*        divider configuration gives the output voltage target.
*******************************************/
uint16_t getVOUT1_TARGET(uint8_t I2CAddress){
    // Read LSB register
    uint8_t VoutTargetLSB;
    VoutTargetLSB = I2C_ReadRegByte(I2CAddress,VOUT_TARGET1_LSB);
    printf("\nRead 0x%X from the VOUT_TARGET1_LSB register\n",VoutTargetLSB);
    // Read MSB register
    uint8_t VoutTargetMSB;
    VoutTargetMSB = I2C_ReadRegByte(I2CAddress,VOUT_TARGET1_MSB);
    printf("Read 0x%X from the VOUT_TARGET1_MSB register\n",VoutTargetMSB);
    // Concat both registers to ouput the VOUT Target value
    uint16_t VoutTarget;
    VoutTarget = ((VoutTargetMSB&0x0F)<<8)|VoutTargetLSB;
    // printf("Vout target return value 0x%X\n",VoutTarget);
    // Return VoutTarget
    return VoutTarget;
}


