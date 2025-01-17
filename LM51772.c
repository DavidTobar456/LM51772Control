//Include header file
#include "LM51772.h"
#include <stdio.h>

/******************************************
* @brief: Makes a write operation on CLEAR_FAULTS register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a write operation in the CLEAR_FAULTS register, with
*        this clearing all faults stated on the STATUS_BYTE register.
*******************************************/
void ClearFaults(uint8_t I2CAddress){
    // Write 0x00 to the CLEAR_FAULTS register
    I2C_WriteRegByte(I2CAddress,CLEAR_FAULTS,0x00);
}

/******************************************
* @brief: Configuration of the ILIM Threshold
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param ILIMVoltage: ILIM threshold voltage given in mA (uint16_t)
* @note: Sets the ILIM_THRESHOLD register value to match the ILIM threshold
*        set by the user, provided in mA. For this, the function calculates
*        the equivalent value to be written to the register, and then writes
*        it to the device.
*        The calculation takes into account the Rsense value used in the
*        application, by default it is defined to be 10 mOhms, as per RSENSE
*        define on the LM51772.h file. Recommendation is to use values from
*        1mOhm onwards with thsi function.
*******************************************/
void setILIM_THRESHOLD(uint8_t I2CAddress, uint16_t ILIMmAmps){
    // Ensure the input value is inside the 500 to 7000 mA range.
    if (ILIMmAmps >= 500 && ILIMmAmps <= 7000){
        // Convert from float to equivalent value
        // ILIM_THRESHOLD = (ILIMmAmps*RSENSE)/500
        uint8_t ilimValue;
        ilimValue = (uint8_t)((ILIMmAmps*R_SENSE)/(500));
        // Write the equivalent value to the ILIM_THRESHOLD register
        float ILIMAmps = ILIMmAmps/1000.0;
        printf("Now writing 0x%X in the ILIM_THRESHOLD register to get a %.2f A current limit\n",ilimValue,ILIMAmps);
        I2C_WriteRegByte(I2CAddress, ILIM_THRESHOLD, ilimValue);
    }
}

/******************************************
* @brief: Sets the VOUT1_TARGET MSB and LSB registers
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param Vout: VOUT to be reached in mV (uint16_t)
* @note: Takes the VOUT and calulates which value has to be in 
*        the VOUT_TARGET1_MSB and VOUT_TARGET1_LSB registers, then
*        writes them to the device accordingly. The function takes
*        the VOUT value as an input, and depending on the FB divider
*        configuration, it will calculate the respective value to be
*        written to the registers.
*        You can select the FB divider configuration by changing the
*        FB_DIVIDER_CONFIG define on the LM51772.h file to one of three
*        possible values:
*           - FB_INTERNAL20
*           - FB_INTERNAL10
*           - FB_EXTERNAL
*******************************************/
void setVOUT1_TARGET(uint8_t I2CAddress, uint16_t Vout){
    #if FB_DIVIDER_CONFIG == FB_INTERNAL20
        // When the internal FB divider is set to 20 then:
        // VoutTarget = Vout/20
        uint16_t VoutTarget = Vout/20;
    #elif FB_DIVIDER_CONFIG == FB_INTERNAL10
        // When the internal FB divider is set to 20 then:
        // VoutTarget = Vout/10
        uint16_t VoutTarget = Vout/10;
    #elif FB_DIVIDER_CONFIG == FB_EXTERNAL
        uint16_t VoutTarget = Vout;
    #else
        #error "FB_DIVIDER_CONFIG not defined"
    #endif
    // Separate VoutTarget on two separate bytes
    uint8_t VoutTargetMSB,VoutTargetLSB;
    VoutTargetLSB = (uint8_t)(VoutTarget & 0xFF);
    VoutTargetMSB = (uint8_t)((VoutTarget >> 8) & 0x0F);
    // Write VOUT_TARGET_LSB register
    I2C_WriteRegByte(I2CAddress,VOUT_TARGET1_LSB,VoutTargetLSB);
    // Write VOUT_TARGET_MSB register
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
    // Read MSB register
    uint8_t VoutTargetMSB;
    VoutTargetMSB = I2C_ReadRegByte(I2CAddress,VOUT_TARGET1_MSB);
    // Concat both registers to ouput the VOUT Target value
    uint16_t VoutTarget;
    VoutTarget = ((VoutTargetMSB&0x0F)<<8)|VoutTargetLSB;
    // printf("Vout target return value 0x%X\n",VoutTarget);
    // Return VoutTarget
    return VoutTarget;
}


/******************************************
* @brief: sets de FORCE_DISCHG of the USB_PD_CONTROL_0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 1 of the
*        USB_PD_CONTROL_0 register, hence opening the discharge
*        path.
*******************************************/
void ForceDischargeEnable(uint8_t I2CAddress){
    // Read USB_PD_CONTROL_0 register current value
    uint8_t Reg = USB_PD_CONTROL_0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 1 of the USB_PD_CONTROL_0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x02);
}

/******************************************
* @brief: clears de FORCE_DISCHG of the USB_PD_CONTROL_0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 1 of the
*        USB_PD_CONTROL_0 register, hence closing the discharge
*        path.
*******************************************/
void ForceDischargeDisable(uint8_t I2CAddress){
    // Read USB_PD_CONTROL_0 register current value
    uint8_t Reg = USB_PD_CONTROL_0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 1 of the USB_PD_CONTROL_0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFD);
}

/******************************************
* @brief: enables power stage switching in the IC
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 0 of the
*        USB_PD_CONTROL_0 and MFR_SPECIFIC_D0 registers, hence 
*        enabling switching in the power stage.
*******************************************/
void EnablePowerStage(uint8_t I2CAddress){
    // Read USB_PD_CONTROL_0 register current value
    uint8_t Reg = USB_PD_CONTROL_0;
    int8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 0 of the USB_PD_CONTROL_0
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x01);
    // Read MFR_SPECIFIC_D0 register current value
    Reg = MFR_SPECIFIC_D0;
    regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 0 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x01);
}

/******************************************
* @brief: disables power stage switching in the IC
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 0 of the
*        USB_PD_CONTROL_0 and MFR_SPECIFIC_D0 registers, hence 
*        disabling switching in the power stage.
*******************************************/
void DisablePowerStage(uint8_t I2CAddress){
    // Read USB_PD_CONTROL_0 register current value
    uint8_t Reg = USB_PD_CONTROL_0;
    int8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 0 of the USB_PD_CONTROL_0
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFE);
    // Read MFR_SPECIFIC_D0 register current value
    Reg = MFR_SPECIFIC_D0;
    regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 0 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFE);
}

/******************************************
* @brief: returns the contents of the USB_PD_STATUS_0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Reads the USB_PD_STATUS_0 register and returns its value
*        the returned value will contain the Constant Current status
*        on the bit number 6.
*******************************************/
uint8_t get_USBPD_STATUS(uint8_t I2CAddress){
    // Prepare the read opearation
    uint8_t USBPDSTATUS;
    // Read the contents of the USB_PD_STATUS_0 byte
    USBPDSTATUS = I2C_ReadRegByte(I2CAddress,USB_PD_STATUS_0);
    // Return the contents of the USB_PD_STATUS_0 byte
    return USBPDSTATUS;
}

/******************************************
* @brief: returns the contents of the STATUS_BYTE register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Reads the STATUS_BYTE register and returns its value.
*        The returned value will contain all of the fault flags
*        current status. This includes the following flags:
*           - BUSY (bit 7): says if the unit is busy
*           - OFF (bit 6): says if the device is providing VOUT
*           - VOUT (bit 5): signals an OVP fault
*           - IOUT (bit 4): signals an OCP fault
*           - INPUT (bit 3): signals an IVP fault
*           - TEMPERATURE (bit 2): temperature fault
*           - CML (bit 1): signals a communication or logic fault
*           - OTHER (bit 0): any other fault not mentioned above
*******************************************/
uint8_t get_STATUS_BYTE(uint8_t I2CAddress){
    // Prepare the read opearation
    uint8_t STATUS;
    // Read the contents of the USB_PD_STATUS_0 byte
    STATUS = (I2CAddress,STATUS_BYTE);
    // Return the contents of the USB_PD_STATUS_0 byte
    return STATUS;
}

/******************************************
* @brief: makes a masked write operation to clear a fault flag
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param FaultFlag: Fault flag/flags to be cleared (uint8_t)
* @note: Writes STATUS_BYTE register to clear a fault flag, the
*        fault flag to be cleared is passed as a parameter and is
*        the logical OR of the desired flags to be cleared from the
*        following list
*           - FLT_OTHER      
*           - FLT_CML        
*           - FLT_TEMPERATURE
*           - FLT_IVP        
*           - FLT_OCP        
*           - FLT_OVP        
*           - FLT_OFF        
*           - FLT_BUSY       
*******************************************/
void ClearFaultFlag(uint8_t I2CAddress,uint8_t FaultFlag){
    // Read STATUS_BYTE register current value
    uint8_t Reg = STATUS_BYTE;
    // Clear the desired fault flag/flags
    I2C_WriteRegByte(I2CAddress,Reg,FaultFlag);
}

/******************************************
* @brief: sets the USLEEP_EN of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 1 of the
*        MFR_SPECIFIC_D0 register, hence activating the micro
*        sleep mode.
*******************************************/
void uSleep_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 1 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x02);
}

/******************************************
* @brief: clears the USLEEP_EN of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 1 of the
*        MFR_SPECIFIC_D0 register, hence disabling the micro
*        sleep mode.
*******************************************/
void uSleep_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 1 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFD);
}

/******************************************
* @brief: sets the DRSS_EN of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 2 of the
*        MFR_SPECIFIC_D0 register, hence activating dual random
*        spread spectrum switching feature.
*******************************************/
void DRSS_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 2 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x04);
}

/******************************************
* @brief: clears the DRSS_EN of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 2 of the
*        MFR_SPECIFIC_D0 register, hence disabling dual random
*        spread spectrum switching feature.
*******************************************/
void DRSS_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 2 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFB);
}

/******************************************
* @brief: sets the HICCUP_EN of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 3 of the
*        MFR_SPECIFIC_D0 register, hence setting the OCP to work
*        in hiccup short circuit mode.
*******************************************/
void HiccupProtection_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 3 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x08);
}

/******************************************
* @brief: clears the HICCUP_EN of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 3 of the
*        MFR_SPECIFIC_D0 register, hence setting the OCP to work
*        in cycle-by-cycle current limiting mode.
*******************************************/
void HiccupProtection_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 3 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xF7);
}

/******************************************
* @brief: sets the IMON_LIMITER_EN of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 4 of the
*        MFR_SPECIFIC_D0 register, hence setting the current sense
*        circuit to work as a current limiter.
*******************************************/
void CurrentLimiter_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 4 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x10);
}

/******************************************
* @brief: clears the IMON_LIMITER_EN of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 4 of the
*        MFR_SPECIFIC_D0 register, hence setting the current sense
*        circuit to work as a current monitor.
*******************************************/
void CurrentLimiter_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 4 of the
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xEF);
}

/******************************************
* @brief: sets the EN_VCC1 of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 5 of the
*        MFR_SPECIFIC_D0 register, hence activating the auxiliary
*        LDO for VCC1 supply.
*******************************************/
void Vcc1LDO_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 5 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x20);
}

/******************************************
* @brief: clears the EN_VCC1 of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 5 of the
*        MFR_SPECIFIC_D0 register, hence disabling the auxiliary
*        LDO for VCC1 supply.
*******************************************/
void Vcc1LDO_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 5 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xDF);
}

/******************************************
* @brief: sets the EN_NEG_CL_LIMIT of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 6 of the
*        MFR_SPECIFIC_D0 register, hence enabling the negative current
*        limiting functionality.
*******************************************/
void NegativeCurrentLimiting_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 6 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x40);
}

/******************************************
* @brief: clears the EN_NEG_CL_LIMIT of the MFR_SPECIFIC_D0 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 6 of the
*        MFR_SPECIFIC_D0 register, hence disabling the negative current
*        limiting functionality. Here ILIM clamps positive.
**************D*****************************/
void NegativeCurrentLimiting_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D0 current value
    uint8_t Reg = MFR_SPECIFIC_D0;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 6 of the MFR_SPECIFIC_D0 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xBF);
}

/******************************************
* @brief: sets the EN_BB_2P_PSM of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 0 of the
*        MFR_SPECIFIC_D1 register, hence enabling 2 phase buckboost
*        switching in PSM mode.
*******************************************/
void PSM_2PhaseBB_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 0 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x01);
}

/******************************************
* @brief: clears the EN_BB_2P_PSM of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 0 of the
*        MFR_SPECIFIC_D1 register, hence disabling 2 phase buckboost
*        switching in PSM mode.
*******************************************/
void PSM_2PhaseBB_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 0 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFE);
}

/******************************************
* @brief: sets the EN_BB_2P_FPWM of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 1 of the
*        MFR_SPECIFIC_D1 register, hence enabling 2 phase buckboost
*        switching in fPWM mode.
*******************************************/
void FPWM_2PhaseBB_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 1 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x02);
}

/******************************************
* @brief: clears the EN_BB_2P_FPWM of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 1 of the
*        MFR_SPECIFIC_D1 register, hence disabling 2 phase buckboost
*        switching in fPWM mode.
*******************************************/
void FPWM_2PhaseBB_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 1 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFD);
}

/******************************************
* @brief: sets the FORCE_BIASPIN of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 2 of the
*        MFR_SPECIFIC_D1 register, hence enabling forced selection
*        of the BIAS pin for the internal voltage regulators and
*        overriding VSMART selection.
*******************************************/
void ForceBias_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 2 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x04);
}

/******************************************
* @brief: clears the FORCE_BIASPIN of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 2 of the
*        MFR_SPECIFIC_D1 register, hence disabling forced selection
*        of the BIAS pin for the internal voltage regulators and
*        taking VSMART selection instead.
*******************************************/
void ForceBias_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 2 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFB);
}

/******************************************
* @brief: sets the EN_DTRK_STARTOVER of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 3 of the
*        MFR_SPECIFIC_D1 register, hence enabling direct startup
*        on DTRK mode without waiting for DTRK PWM signal.
*******************************************/
void DTRK_DirectStartup_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 3 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x08);
}

/******************************************
* @brief: clears the EN_DTRK_STARTOVER of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 3 of the
*        MFR_SPECIFIC_D1 register, hence disabling direct startup
*        on DTRK and waiting for the DTRK PWM signal on startup.
*******************************************/
void DTRK_DirectStartup_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 3 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xF7);
}

/******************************************
* @brief: sets the EN_NINT of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 4 of the
*        MFR_SPECIFIC_D1 register, hence setting the nFLT pin to
*        work as an interrupt pin.
*******************************************/
void nFLT_as_INT_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 4 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x10);
}

/******************************************
* @brief: clears the EN_NINT of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 4 of the
*        MFR_SPECIFIC_D1 register, hence setting the nFLT pin to
*        act as an indicator of faults.
*******************************************/
void nFLT_as_INT_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 4 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xEF);
}

/******************************************
* @brief: sets up the temperature threshold for thermal warnings
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param Threshold: threshold for thermal warnings
* @note: Makes a masked write operation to the bits 6:5 of the
*        MFR_SPECIFIC_D1 register to set the thermal threshold
*        to one of four different values:
*           - THW_THRESHOLD_140degC
*           - THW_THRESHOLD_125degC
*           - THW_THRESHOLD_140degC
*           - THW_THRESHOLD_140degC
*******************************************/
void ThermalWarning_ThresholdConfigure(uint8_t I2CAddress,uint8_t Threshold){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = (I2C_ReadRegByte(I2CAddress,Reg)&0x9F);
    // Masked write of the threshold value selected to MFR_SPECIFIC_D1
    I2C_WriteRegByte(I2CAddress,Reg,regContent|Threshold);
}

/******************************************
* @brief: sets the EN_THER_WARN of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 7 of the
*        MFR_SPECIFIC_D1 register, hence enabling Thermal Warning.
*******************************************/
void ThermalWarning_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 7 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x80);
}

/******************************************
* @brief: clears the EN_THER_WARN of the MFR_SPECIFIC_D1 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 7 of the
*        MFR_SPECIFIC_D1 register, hence disabling Thermal Warning.
*******************************************/
void ThermalWarning_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D1 current value
    uint8_t Reg = MFR_SPECIFIC_D1;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 7 of the MFR_SPECIFIC_D1 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0x7F);
}

/******************************************
* @brief: sets the DISCHARGE_CONFIG1 of the MFR_SPECIFIC_D2 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 0 of the
*        MFR_SPECIFIC_D2 register, hence enabling discharge until VTH
*        discharge is reached.
*******************************************/
void Discharge_VTH_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D2 current value
    uint8_t Reg = MFR_SPECIFIC_D2;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 0 of the MFR_SPECIFIC_D2 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x01);
}

/******************************************
* @brief: clears the DISCHARGE_CONFIG1 of the MFR_SPECIFIC_D2 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 0 of the
*        MFR_SPECIFIC_D2 register, hence enabling discharge until VTH
*        discharge is reached.
*******************************************/
void Discharge_VTH_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D2 current value
    uint8_t Reg = MFR_SPECIFIC_D2;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 0 of the MFR_SPECIFIC_D2 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFE);
}

/******************************************
* @brief: sets the DISCHARGE_CONFIG1 of the MFR_SPECIFIC_D2 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 1 of the
*        MFR_SPECIFIC_D2 register, hence enabling discharge along with
*        CONV_EN.
*******************************************/
void Discharge_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D2 current value
    uint8_t Reg = MFR_SPECIFIC_D2;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 1 of the MFR_SPECIFIC_D2 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x02);
}

/******************************************
* @brief: sets the DISCHARGE_CONFIG1 of the MFR_SPECIFIC_D2 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 1 of the
*        MFR_SPECIFIC_D2 register, hence enabling discharge along with
*        CONV_EN.
*******************************************/
void Discharge_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D2 current value
    uint8_t Reg = MFR_SPECIFIC_D2;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 1 of the MFR_SPECIFIC_D2 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xFD);
}

/******************************************
* @brief: sets up DISCHG_STRENGTH of the MFR_SPECIFIC_D2 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param Strength: current strength on the discharging circuit (uint8_t)
* @note: Makes a masked write operation to the bits 3:2 of the
*        MFR_SPECIFIC_D2 register to set the discharging strength
*        to one of three different values:
*           - DISCHG_STRENGTH_25mA
*           - DISCHG_STRENGTH_50mA
*           - DISCHG_STRENGTH_75mA
*******************************************/
void Dishcarge_StrengthConfigure(uint8_t I2CAddress,uint8_t Strength){
    // Read MFR_SPECIFIC_D2 current value
    uint8_t Reg = MFR_SPECIFIC_D2;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xF3;
    // Masked write of the strength value selected to MFR_SPECIFIC_D2
    I2C_WriteRegByte(I2CAddress,Reg,regContent|Strength);
}

/******************************************
* @brief: sets up DVS_SLEW_RAMP of the MFR_SPECIFIC_D2 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param Slewrate: slew rate of DVS ramp (uint8_t)
* @note: Makes a masked write operation to the bits 5:4 of the
*        MFR_SPECIFIC_D2 register to set the discharging strength
*        to one of three different values:
*           - DVS_SLEW_40mV_us 
*           - DVS_SLEW_20mV_us 
*           - DVS_SLEW_1mV_us  
*           - DVS_SLEW_0_5mV_us
*******************************************/
void DVS_SlewrateConfigure(uint8_t I2CAddress, uint8_t Slewrate){
    // Read MFR_SPECIFIC_D2 current value
    uint8_t Reg = MFR_SPECIFIC_D2;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xCF;
    // Masked write of the slew rate value selected to MFR_SPECIFIC_D2
    I2C_WriteRegByte(I2CAddress,Reg,regContent|Slewrate);
}

/******************************************
* @brief: sets the EN_ACTIVE_DVS of the MFR_SPECIFIC_D2 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 6 of the
*        MFR_SPECIFIC_D2 register, hence enabling active down ramp
*        on DVS using the discharge.
*******************************************/
void DVS_ActiveDownRamp_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D2 current value
    uint8_t Reg = MFR_SPECIFIC_D2;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 6 of the MFR_SPECIFIC_D2 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x40);
}

/******************************************
* @brief: clears the EN_ACTIVE_DVS of the MFR_SPECIFIC_D2 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the clear 6 of the
*        MFR_SPECIFIC_D2 register, hence disabling active down ramp
*        on DVS.
*******************************************/
void DVS_ActiveDownRamp_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D2 current value
    uint8_t Reg = MFR_SPECIFIC_D2;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 6 of the MFR_SPECIFIC_D2 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xBF);
}

/******************************************
* @brief: sets the falling threshold for VDET functionality
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param Threshold: threshold voltage for VDET given in mV (uint16_t)
* @note: Makes a masked write operation to modify the 4:0 bits of the
*        MFR_SPECIFIC_D3 register, with this setting the falling threshold
*   	 for VDET functionality. The value provided on the Threshold
*        parameter must be between 2700 and 8900 mV otherwise the register
*        won't be modified.
*******************************************/
void VDET_FallingThresholdConfigure(uint8_t I2CAddress,uint16_t Threshold){
    // Verify if the threshold is between 2700 and 8900
    if((Threshold>=2700)&&(Threshold<=8900)){
        // Read MFR_SPECIFIC_D3 current value
        uint8_t Reg = MFR_SPECIFIC_D3;
        uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xE0;
        // Calculate the value to be written on the 4:0 bits
        // VDET_FALL[4:0]=(Threshold-2700mV)/200mV
        uint8_t VDET = (uint8_t)((Threshold-2700)/200);
        // Masked write on the MFR_SPECIFIC_D3 4:0 bits
        I2C_WriteRegByte(I2CAddress,Reg,regContent|VDET);
    }
    // If threshold is not between 2700 and 8900, do nothing
}

/******************************************
* @brief: sets the VDET_EN of the MFR_SPECIFIC_D3 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 5 of the
*        MFR_SPECIFIC_D3 register, hence enabling the VDET internal
*   	 UVLO comparator.
*******************************************/
void VDET_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D3 current value
    uint8_t Reg = MFR_SPECIFIC_D3;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 5 of the MFR_SPECIFIC_D3 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x20);
}

/******************************************
* @brief: clears the VDET_EN of the MFR_SPECIFIC_D3 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 5 of the
*        MFR_SPECIFIC_D3 register, hence disabling the VDET internal
*   	 UVLO comparator.
*******************************************/
void VDET_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D3 current value
    uint8_t Reg = MFR_SPECIFIC_D3;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 5 of the MFR_SPECIFIC_D3 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xDF);
}

/******************************************
* @brief: sets the SEL_IVR of the MFR_SPECIFIC_D3 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 6 of the
*        MFR_SPECIFIC_D3 register, hence enabling the Input Voltage
*        Regulation when IVP is enabled. It is important to note
*        that for this feature to be effectively enabled one must
*        first activate IVP through the IVP_Enable() function.
*******************************************/
void IVP_InputVoltageRegulation_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D3 current value
    uint8_t Reg = MFR_SPECIFIC_D3;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 6 of the MFR_SPECIFIC_D3 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x40);
}

/******************************************
* @brief: sets the SEL_IVR of the MFR_SPECIFIC_D3 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 6 of the
*        MFR_SPECIFIC_D3 register, hence disabling the Input Voltage
*        Regulation when IVP is enabled.
*******************************************/
void IVP_InputVoltageRegulation_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D3 current value
    uint8_t Reg = MFR_SPECIFIC_D3;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 6 of the MFR_SPECIFIC_D3 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xBF);
}

/******************************************
* @brief: sets the EN_IVP of the MFR_SPECIFIC_D3 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 7 of the
*        MFR_SPECIFIC_D3 register, hence enabling Input Voltage
*        Protection.
*******************************************/
void IVP_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D3 current value
    uint8_t Reg = MFR_SPECIFIC_D3;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 7 of the MFR_SPECIFIC_D3 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x80);
}

/******************************************
* @brief: clears the EN_IVP of the MFR_SPECIFIC_D3 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 7 of the
*        MFR_SPECIFIC_D3 register, hence disabling Input Voltage
*        Protection.
*******************************************/
void IVP_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D3 current value
    uint8_t Reg = MFR_SPECIFIC_D3;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 7 of the MFR_SPECIFIC_D3 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0x7F);
}

/******************************************
* @brief: sets the rising threshold for VDET functionality
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param Threshold: threshold voltage for VDET given in mV (uint16_t)
* @note: Makes a masked write operation to modify the 4:0 bits of the
*        MFR_SPECIFIC_D4 register, with this setting the rising threshold
*   	 for VDET functionality. The value provided on the Threshold
*        parameter must be between 2800 and 9000 mV otherwise the register
*        won't be modified.
*******************************************/
void VDET_RisingThresholdConfigure(uint8_t I2CAddress,uint16_t Threshold){
    // Verify if the threshold is between 2800 and 9000
    if((Threshold>=2800)&&(Threshold<=9000)){
        uint8_t Reg = MFR_SPECIFIC_D4;
        // Calculate the value to be written on the 4:0 bits
        // VDET_RISE[4:0]=(Threshold-2800mV)/200mV
        uint8_t VDET = (uint8_t)((Threshold-2800)/200);
        // Masked write on the MFR_SPECIFIC_D3 4:0 bits
        I2C_WriteRegByte(I2CAddress,Reg,VDET);
    }
    // If threshold is not between 2800 and 9000, do nothing
}

/******************************************
* @brief: sets the threshold for OVP2 protection
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param Threshold: threshold voltage for OVP2 given in mV (uint16_t)
* @note: Makes a masked write operation to modify the 5:0 bits of the
*        MFR_SPECIFIC_D5 register, with this setting the threshold
*   	 for OVP2 functionality. The value provided on the Threshold
*        parameter must be between 4000 and 55000 mV otherwise the register
*        won't be modified.
*******************************************/
void OVP_SecondaryThreshold_Configure(uint8_t I2CAddress,uint16_t Threshold){
    // Verify if the threshold is between 4000 and 55000
    if((Threshold>=4000)&&(Threshold<=55000)){
        uint8_t Reg = MFR_SPECIFIC_D5;
        uint8_t VOVP2;
        if(Threshold<16000){
            // Calculate the value to be written on the 5:0 bits
            // When Threshold < 16000mV
            // V_OVP2[5:0]=(Threshold-4000mV)/500mV
            VOVP2 = (uint8_t)((Threshold-4000)/500);
            // Masked write on the MFR_SPECIFIC_D3 4:0 bits
            I2C_WriteRegByte(I2CAddress,Reg,VOVP2);
        }
        else{
            // Calculate the value to be written on the 5:0 bits
            // When Threshold > 16000mV
            // V_OVP2[5:0]=24+(Threshold-16000mV)/1000mV
            VOVP2 = (uint8_t)(24+(Threshold-16000)/1000);
            // Masked write on the MFR_SPECIFIC_D3 4:0 bits
            I2C_WriteRegByte(I2CAddress,Reg,VOVP2);
        }
    }
    // If threshold is not between 4000 and 55000, do nothing
}

/******************************************
* @brief: sets up the minimum time scale of BB on gate refreshes
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param Scale: scale factor of the minimum on and off time (uint8_t)
* @note: Makes a masked write operation to the bits 1:0 of the
*        MFR_SPECIFIC_D6 register to set the scale factor of the on
*        and off time of the BuckBoost gate refreshes. It can be one
*        of four values:
*           - BB_MINTIME_SCALE_0_75x
*           - BB_MINTIME_SCALE_1x   
*           - BB_MINTIME_SCALE_1_25x
*           - BB_MINTIME_SCALE_1_5x 
*******************************************/
void BB_MinTimeScale_Select(uint8_t I2CAddress,uint8_t Scale){
    uint8_t Reg = MFR_SPECIFIC_D6;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xFC;
    // Masked write of the scale value selected to MFR_SPECIFIC_D6
    I2C_WriteRegByte(I2CAddress,Reg,regContent|Scale);
}

/******************************************
* @brief: sets up the minimum dead-time for the gate driver at fsw=2MHz
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param DeadTime: dead time of GDRV at fsw=2MHz (uint8_t)
* @note: Makes a masked write operation to the bits 3:2 of the
*        MFR_SPECIFIC_D6 register to set the minimum dead-time for the
*        gate driver at fsw=2MHz. It can be one of four values:
*           - GDRV_MINDEADTIME_10ns
*           - GDRV_MINDEADTIME_20ns
*           - GDRV_MINDEADTIME_40ns
*           - GDRV_MINDEADTIME_60ns
*******************************************/
void GDRV_MinDeadTime_Select(uint8_t I2CAddress,uint8_t DeadTime){
    uint8_t Reg = MFR_SPECIFIC_D6;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xF3;
    // Masked write of the scale value selected to MFR_SPECIFIC_D6
    I2C_WriteRegByte(I2CAddress,Reg,regContent|DeadTime);
}

/******************************************
* @brief: sets the SEL_SCALE_DT of the MFR_SPECIFIC_D6 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 4 of the
*        MFR_SPECIFIC_D6 register, hence enabling frequency dependent
*        dead-time scaling on the Gate Driver.
*******************************************/
void GDRV_DeadTimeScaling_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D6 current value
    uint8_t Reg = MFR_SPECIFIC_D6;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 4 of the MFR_SPECIFIC_D6 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x10);
}

/******************************************
* @brief: clears the SEL_SCALE_DT of the MFR_SPECIFIC_D6 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 4 of the
*        MFR_SPECIFIC_D6 register, hence disabling frequency dependent
*        dead-time scaling on the Gate Driver.
*******************************************/
void GDRV_DeadTimeScaling_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D6 current value
    uint8_t Reg = MFR_SPECIFIC_D6;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 4 of the MFR_SPECIFIC_D6 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xEF);
}

/******************************************
* @brief: sets the EN_CONTS_TDEAD of the MFR_SPECIFIC_D6 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 5 of the
*        MFR_SPECIFIC_D6 register, hence forcing a constant dead
*        on the Gate Driver and disabling frequency dependency of
*        the dead-time.
*******************************************/
void GDRV_ForceConstantDeadTime_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D6 current value
    uint8_t Reg = MFR_SPECIFIC_D6;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 5 of the MFR_SPECIFIC_D6 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x20);
}

/******************************************
* @brief: clears the EN_CONTS_TDEAD of the MFR_SPECIFIC_D6 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 5 of the
*        MFR_SPECIFIC_D6 register, with this, a constant dead-time
*        is not forced into the Gate Driver and frequency dependency
*        of the dead-time is enabled.
*******************************************/
void GDRV_ForceConstantDeadTime_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D6 current value
    uint8_t Reg = MFR_SPECIFIC_D6;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 5 of the MFR_SPECIFIC_D6 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xDF);
}

/******************************************
* @brief: configures the synchronization function of the oscillator
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param DeadTime: Synchronization function for parallel operation (uint8_t)
* @note: Makes a masked write operation to the bits 7:6 of the
*        MFR_SPECIFIC_D6 register to configure the synchronization
*        function of the oscillator. It can be one of four values:
*           - OSC_SYNC_INPUT_RISING  
*           - OSC_SYNC_INPUT_FALLING 
*           - OSC_SYNC_OUTPUT_RISING 
*           - OSC_SYNC_OUTPUT_FALLING
*******************************************/
void OSC_FreqSyncConfigure(uint8_t I2CAddress,uint8_t SyncFunction){
    uint8_t Reg = MFR_SPECIFIC_D6;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0x3F;
    // Masked write of the scale value selected to MFR_SPECIFIC_D6
    I2C_WriteRegByte(I2CAddress,Reg,regContent|SyncFunction);
}

/******************************************
* @brief: selects the correction factor for slope compensation
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param CorrectionFactor: Slope Compensation correction factor (uint8_t)
* @note: Makes a masked write operation to the bits 3:0 of the
*        MFR_SPECIFIC_D7 register to configure the slope compensation
*        correction factor. The correction factor can be one of 16
*        values, each and every one is given as a ratio of RT current:
*           - SLOPECOMP_CORRECTION_0_125
*           - SLOPECOMP_CORRECTION_0_25 
*           - SLOPECOMP_CORRECTION_0_375
*           - SLOPECOMP_CORRECTION_0_5  
*           - SLOPECOMP_CORRECTION_0_625
*           - SLOPECOMP_CORRECTION_0_75 
*           - SLOPECOMP_CORRECTION_0_875
*           - SLOPECOMP_CORRECTION_1_0  
*           - SLOPECOMP_CORRECTION_1_5  
*           - SLOPECOMP_CORRECTION_2_0  
*           - SLOPECOMP_CORRECTION_2_5  
*           - SLOPECOMP_CORRECTION_3_0  
*           - SLOPECOMP_CORRECTION_3_5  
*           - SLOPECOMP_CORRECTION_4_0  
*   	    - SLOPECOMP_CORRECTION_4_5  
*           - SLOPECOMP_CORRECTION_5_0  
*******************************************/
void SlopeComp_CorrectionFactor_Select(uint8_t I2CAddress,uint8_t CorrectionFactor){
    uint8_t Reg = MFR_SPECIFIC_D7;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xF0;
    // Masked write of the scale value selected to MFR_SPECIFIC_D7
    I2C_WriteRegByte(I2CAddress,Reg,regContent|CorrectionFactor);
}

/******************************************
* @brief: selects the inductor derating value or disables it
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param InductorDerating: inductor de rating value (uint8_t)
* @note: Makes a masked write operation to the bits 5:4 of the
*        MFR_SPECIFIC_D7 register to configure the inductor de
*        rating value. The InductorDerating value can be one of
*        four values:
*           - INDUC_DERATE_DISABLE
*           - INDUC_DERATE_20     
*           - INDUC_DERATE_30     
*           - INDUC_DERATE_40     
*******************************************/
void SlopeComp_InductorDerating_Select(uint8_t I2CAddress,uint8_t InductorDerating){
    uint8_t Reg = MFR_SPECIFIC_D7;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xCF;
    // Masked write of the scale value selected to MFR_SPECIFIC_D7
    I2C_WriteRegByte(I2CAddress,Reg,regContent|InductorDerating);
}

/******************************************
* @brief: selects the supply of the DRV1 pin for driving
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param DRV1Config: DRV1 driver supply configuration (uint8_t)
* @note: Makes a masked write operation to the bits 1:0 of the
*        MFR_SPECIFIC_D8 register to configure the driving supply
*        of the DRV1 pin. The DRV1Config value can be one of:
*           - DRV1_SUP_OPENDRAIN
*           - DRV1_SUP_VOUT     
*           - DRV1_SUP_VBIAS    
*           - DRV1_SUP_VCC2     
*******************************************/
void DRV1_Supply_Configure(uint8_t I2CAddress,uint8_t DRV1Config){
    uint8_t Reg = MFR_SPECIFIC_D8;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xFC;
    // Masked write of the scale value selected to MFR_SPECIFIC_D8
    I2C_WriteRegByte(I2CAddress,Reg,regContent|DRV1Config);
}

/******************************************
* @brief: selects the sequencing of the DRV1 pin for driving
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param DRV1Sequence: DRV1 driver sequencing configuration (uint8_t)
* @note: Makes a masked write operation to the bits 3:2 of the
*        MFR_SPECIFIC_D8 register to configure the sequencing
*        of the DRV1 pin. The DRV1Sequence value can be one of:
*           - DRV1_SEQ_PULL_LOW_CONV_ON 
*           - DRV1_SEQ_PULL_LOW_CONV_OFF
*           - DRV1_SEQ_FORCE_ACTIVE     
*           - DRV1_SEQ_FORCE_OFF        
*******************************************/
void DRV1_Sequence_Configure(uint8_t I2CAddress,uint8_t DRV1Sequence){
    uint8_t Reg = MFR_SPECIFIC_D8;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xF3;
    // Masked write of the scale value selected to MFR_SPECIFIC_D8
    I2C_WriteRegByte(I2CAddress,Reg,regContent|DRV1Sequence);
}

/******************************************
* @brief: selects the CDC voltage gain with respect to Vout
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param GainVoltage: voltage gain of CDC compensation (uint8_t)
* @note: Makes a masked write operation to the bits 5:4 of the
*        MFR_SPECIFIC_D8 register to configure the voltage gain
*        of the CDC compensation. The GainVoltage value can be
*        one of four values:
*           - CDC_GAIN_0_250V
*           - CDC_GAIN_0_500V
*           - CDC_GAIN_1_000V
*           - CDC_GAIN_2_000V
*******************************************/
void CDC_GainVoltage_Select(uint8_t I2CAddress,uint8_t GainVoltage){
    uint8_t Reg = MFR_SPECIFIC_D8;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xCF;
    // Masked write of the scale value selected to MFR_SPECIFIC_D8
    I2C_WriteRegByte(I2CAddress,Reg,regContent|GainVoltage);
}

/******************************************
* @brief: sets the EN_CDC of the MFR_SPECIFIC_D8 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 6 of the
*        MFR_SPECIFIC_D8 register, hence enabling the cable drop
*        compensation (CDC) feature.
*******************************************/
void CDC_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D8 current value
    uint8_t Reg = MFR_SPECIFIC_D8;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 6 of the MFR_SPECIFIC_D8 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x40);
}

/******************************************
* @brief: clears the EN_CDC of the MFR_SPECIFIC_D8 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 6 of the
*        MFR_SPECIFIC_D8 register, hence disabling the cable drop
*        compensation (CDC) feature.
*******************************************/
void CDC_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D8 current value
    uint8_t Reg = MFR_SPECIFIC_D8;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 6 of the MFR_SPECIFIC_D8 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xBF);
}

/******************************************
* @brief: sets the SEL_FB_DIV20 of the MFR_SPECIFIC_D8 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 7 of the
*        MFR_SPECIFIC_D8 register, hence selecting the internal
*        FB divider of ratio 20.
*******************************************/
void LM51772_FB_Divider_Sel20(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D8 current value
    uint8_t Reg = MFR_SPECIFIC_D8;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 7 of the MFR_SPECIFIC_D8 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x80);
}

/******************************************
* @brief: clears the SEL_FB_DIV20 of the MFR_SPECIFIC_D8 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to clear the bit 7 of the
*        MFR_SPECIFIC_D8 register, hence selecting the internal
*        FB divider of ratio 10.
*******************************************/
void LM51772_FB_Divider_Sel10(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D8 current value
    uint8_t Reg = MFR_SPECIFIC_D8;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 7 of the MFR_SPECIFIC_D8 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0x7F);
}

/******************************************
* @brief: configures the lower window of PCM operation
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param LowerWindow: lower window of PCM operation (uint16_t)
* @note: Makes a masked write operation to the bits 4:0 of the
*        MFR_SPECIFIC_D9 register to configure the lower window
*        of the PCM operation. The function receives values ranging
*        from 0 to 775, these correspond to ten times the percentage
*        of Vout one wants to place the lower window of PCM. This
*        is done to avoid floating point operations.
*******************************************/
void PCM_LowerVoltageWindow_Configure(uint8_t I2CAddress,uint16_t LowerWindow){
    // Verify if the LowerWindow is between 0 and 775
    if((LowerWindow>=0)&&(LowerWindow<=775)){
        uint8_t Reg = MFR_SPECIFIC_D9;
        uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xE0;
        // Calculate the value to be written on the 4:0 bits
        uint8_t PCMWindowLow = (uint8_t)(LowerWindow/25);
        // Masked write on the MFR_SPECIFIC_D9 4:0 bits
        I2C_WriteRegByte(I2CAddress,Reg,regContent|PCMWindowLow);
    }
    // If LowerWindow is not between 0 and 775, do nothing
}

/******************************************
* @brief: configures the lower window of PCM operation
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param LowerWindow: lower window of PCM operation (float)
* @note: Makes a masked write operation to the bits 4:0 of the
*        MFR_SPECIFIC_D9 register to configure the lower window
*        of the PCM operation. The function receives values ranging
*        from 0 to 77.5, these correspond to the percentage of 
*        Vout one wants to place the lower window of PCM on.
*******************************************/
void PCM_LowerVoltageWindow_ConfigureF(uint8_t I2CAddress,float LowerWindow){
    // Verify if the LowerWindow is between 0 and 77.5
    if((LowerWindow>=0)&&(LowerWindow<=77.5)){
        uint8_t Reg = MFR_SPECIFIC_D9;
        uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg)&0xE0;
        // Calculate the value to be written on the 4:0 bits
        uint8_t PCMWindowLow = (uint8_t)(LowerWindow*10/25);
        // Masked write on the MFR_SPECIFIC_D9 4:0 bits
        I2C_WriteRegByte(I2CAddress,Reg,regContent|PCMWindowLow);
    }
    // If LowerWindow is not between 0 and 77.5, do nothing
}

/******************************************
* @brief: sets the SEL_ISET_PIN of the MFR_SPECIFIC_D9 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 5 of the
*        MFR_SPECIFIC_D9 register, hence forcing the ISET pin
*        to be used as the current limit input over the ILIM DAC.
*******************************************/
void OCP_ISET_OverILIM_Enable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D9 current value
    uint8_t Reg = MFR_SPECIFIC_D9;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Set bit 5 of the MFR_SPECIFIC_D9 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent|0x20);
}

/******************************************
* @brief: sets the SEL_ISET_PIN of the MFR_SPECIFIC_D9 register
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @note: Makes a masked write operation to set the bit 5 of the
*        MFR_SPECIFIC_D9 register, hence forcing the ISET pin
*        to be used as the current limit input over the ILIM DAC.
*******************************************/
void OCP_ISET_OverILIM_Disable(uint8_t I2CAddress){
    // Read MFR_SPECIFIC_D9 current value
    uint8_t Reg = MFR_SPECIFIC_D9;
    uint8_t regContent = I2C_ReadRegByte(I2CAddress,Reg);
    // Clear bit 5 of the MFR_SPECIFIC_D9 register
    I2C_WriteRegByte(I2CAddress,Reg,regContent&0xDF);
}

/******************************************
* @brief: sets the threshold for IVP protection and regulation
* @param I2CAddress: I2CAddress of the LM51772 device (uint8_t)
* @param Threshold: threshold voltage for IVP given in mV (uint16_t)
* @note: writes the IVP_VOLTAGE register, setting the threshold
*   	 for IVP functionality. The value provided on the Threshold
*        parameter must be between 4750 and 55000 mV otherwise the register
*        won't be modified.
*******************************************/
void IVP_VoltageThreshold_Configure(uint8_t I2CAddress,uint16_t Threshold){
    // Verify if the threshold is between 4750 and 55000
    if((Threshold>=4750)&&(Threshold<=55000)){
        uint8_t Reg = IVP_VOLTAGE;
        uint8_t IVP;
        if(Threshold<24000){
            // Calculate the value to be written on the 5:0 bits
            // When Threshold < 16000mV
            // IVP_VOLTAGE=(Threshold-4750mV)/250mV
            IVP = (uint8_t)((Threshold-4750)/125);
            // Write the IVP_VOLTAGE register
            I2C_WriteRegByte(I2CAddress,Reg,IVP);
        }
        else{
            // Calculate the value to be written on the 5:0 bits
            // When Threshold > 24000mV
            // IVP_VOLTAGE=151+(Threshold-24000mV)/250mV
            IVP = (uint8_t)(151+(Threshold-24000)/250);
            // Write the IVP_VOLTAGE register
            I2C_WriteRegByte(I2CAddress,Reg,IVP);
        }
    }
    // If threshold is not between 4750 and 55000, do nothing
}
