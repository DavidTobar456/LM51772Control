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
// Rsense value used in the application
#define R_SENSE                         10 // Value in mOhms

// LM51772 - VOUT Aux definitions
#define VOUT_MSB_RMASK                  0x07
#define VOUT_MSB_WMASK                  0x07
// Different types of feedback divider configurations
#define FB_INTERNAL20                   1
#define FB_INTERNAL10                   2
#define FB_EXTERNAL                     3
//-------CHANGE TO SELECT THE FB DIVIDER CONFIGURATION---------//
#define FB_DIVIDER_CONFIG               FB_INTERNAL20
#if FB_DIVIDER_CONFIG == FB_EXTERNAL
    // External resistors values
#define Rbot                            1000 // Value in Ohms
#define Rtop                            1000 // Value in Ohms
#endif

// LM51772 - STATUS_BYTE auxiliary definitions
// Fault/Interrupt flags
#define FLT_OTHER                       0x01
#define FLT_CML                         0x02
#define FLT_TEMPERATURE                 0x04
#define FLT_IVP                         0x08
#define FLT_OCP                         0x10
#define FLT_OVP                         0x20
#define FLT_OFF                         0x40
#define FLT_BUSY                        0x80

// LM51772 - MFR_SPECIFIC_D1 auxiliary definitions
// Thermal warning thresholds
#define THW_THRESHOLD_140degC           0x00
#define THW_THRESHOLD_125degC           0x20
#define THW_THRESHOLD_110degC           0x40
#define THW_THRESHOLD_95degC            0x60

// LM51772 - MFR_SPECIFIC_D2 auxiliary definitions
// Dishcarge strength currents
#define DISCHG_STRENGTH_25mA            0x00
#define DISCHG_STRENGTH_50mA            0x04
#define DISCHG_STRENGTH_75mA            0x08
// DVS slew rate selection
#define DVS_SLEW_40mV_us                0x00
#define DVS_SLEW_20mV_us                0x10
#define DVS_SLEW_1mV_us                 0x20
#define DVS_SLEW_0_5mV_us               0x30

// LM51772 - MFR_SPECIFIC_D6 auxiliary definitions
// Buck-Boost scaling of minimum on-time and off-time
#define BB_MINTIME_SCALE_0_75x          0x00
#define BB_MINTIME_SCALE_1x             0x01
#define BB_MINTIME_SCALE_1_25x          0x02
#define BB_MINTIME_SCALE_1_5x           0x03
// Gate Driver minimum dead-time at fsw=2MHz
#define GDRV_MINDEADTIME_10ns           0x00
#define GDRV_MINDEADTIME_20ns           0x04
#define GDRV_MINDEADTIME_40ns           0x08
#define GDRV_MINDEADTIME_60ns           0x0C
// Synchronization function to maintain parallel operation
#define OSC_SYNC_INPUT_RISING           0x00
#define OSC_SYNC_INPUT_FALLING          0x40
#define OSC_SYNC_OUTPUT_RISING          0x80
#define OSC_SYNC_OUTPUT_FALLING         0xC0

// LM51772 - MFR_SPECIFIC_D7 auxiliary definitions
// Slope compensation correction factor
#define SLOPECOMP_CORRECTION_0_125      0x00
#define SLOPECOMP_CORRECTION_0_25       0x01
#define SLOPECOMP_CORRECTION_0_375      0x02
#define SLOPECOMP_CORRECTION_0_5        0x03
#define SLOPECOMP_CORRECTION_0_625      0x04
#define SLOPECOMP_CORRECTION_0_75       0x05
#define SLOPECOMP_CORRECTION_0_875      0x06
#define SLOPECOMP_CORRECTION_1_0        0x07
#define SLOPECOMP_CORRECTION_1_5        0x08
#define SLOPECOMP_CORRECTION_2_0        0x09
#define SLOPECOMP_CORRECTION_2_5        0x0A
#define SLOPECOMP_CORRECTION_3_0        0x0B
#define SLOPECOMP_CORRECTION_3_5        0x0C
#define SLOPECOMP_CORRECTION_4_0        0x0D
#define SLOPECOMP_CORRECTION_4_5        0x0E
#define SLOPECOMP_CORRECTION_5_0        0x0F
// Inductor de-rating value for PSM mode to slope
#define INDUC_DERATE_DISABLE            0x00
#define INDUC_DERATE_20                 0x10
#define INDUC_DERATE_30                 0x20
#define INDUC_DERATE_40                 0x30

// LM51772 - MFR_SPECIFIC_D8 auxiliary definitions
// Driver configuration for the DRV1 pin
#define DRV1_SUP_OPENDRAIN              0x00
#define DRV1_SUP_VOUT                   0X01
#define DRV1_SUP_VBIAS                  0x02
#define DRV1_SUP_VCC2                   0x03
// Sequencing of the DRV1 pin
#define DRV1_SEQ_PULL_LOW_CONV_OFF      0x00
#define DRV1_SEQ_PULL_LOW_CONV_ON       0x04
#define DRV1_SEQ_FORCE_ACTIVE           0x08
#define DRV1_SEQ_FORCE_OFF              0x0C
// Gain for Cable Drop Compensation (CDC)
#define CDC_GAIN_0_250V                 0x00
#define CDC_GAIN_0_500V                 0x10
#define CDC_GAIN_1_000V                 0x20
#define CDC_GAIN_2_000V                 0x30

// Function definitions
// Functions for the CLEAR_FAULTS register
void ClearFaults(uint8_t I2CAddress);

// Functions for ILIM_THRESHOLD modifications
void setILIM_THRESHOLD(uint8_t I2CAddress, uint16_t ILIMmAmps);
// Functions for VOUT_TARGET1 registers
// Setting of the VOUT target
void setVOUT1_TARGET(uint8_t I2CAddress, uint16_t Vout);
uint16_t getVOUT1_TARGET(uint8_t I2CAddress);

// Functions for the USB_PD_CONTROL_0 register
// Functions for enabling/disabling discharge functionality
void ForceDischargeEnable(uint8_t I2CAddress);
void ForceDischargeDisable(uint8_t I2CAddress);
// Functions for enabling/disabling the IC power stage
void EnablePowerStage(uint8_t I2CAddress);
void DisablePowerStage(uint8_t I2CAddress);

// Functions for reading the USB_PD_STATUS_0 register
uint8_t get_USBPD_STATUS(uint8_t I2CAddress);

// Functions for reading the STATUS_BYTE register
uint8_t get_STATUS_BYTE(uint8_t I2CAddress);
void ClearFaultFlag(uint8_t I2CAddress,uint8_t FaultFlag);

// Functions for the MFR_SPECIFIC_D0 register
// Enabling/disabling micro sleep mode
void uSleep_Enable(uint8_t I2CAddress);
void uSleep_Disable(uint8_t I2CAddress);
// Enabling/Disabling Dual Spread Spectrum switching
void DRSS_Enable(uint8_t I2CAddress);
void DRSS_Disable(uint8_t I2CAddress);
// Enabling/Disabling Hiccup Protection Mode
void HiccupProtection_Enable(uint8_t I2CAddress);
void HiccupProtection_Disable(uint8_t I2CAddress);
// Enabling/Disabling current limiting feature
void CurrentLimiter_Enable(uint8_t I2CAddress);
void CurrentLimiter_Disable(uint8_t I2CAddress);
// Enabling/Disabling VCC1 auxiliary LDO
void Vcc1LDO_Enable(uint8_t I2CAddress);
void Vcc1LDO_Disable(uint8_t I2CAddress);
// Enabling/Disabling negative current limiting
void NegativeCurrentLimiting_Enable(uint8_t I2CAddress);
void NegativeCurrentLimiting_Disable(uint8_t I2CAddress);

// Functions for the MFR_SPECIFIC_D1 register
// Enabling/Disabling 2phase Buck-Boost switching in PSM mode
void PSM_2PhaseBB_Enable(uint8_t I2CAddress);
void PSM_2PhaseBB_Disable(uint8_t I2CAddress);
// Enabling/Disabling 2phase Buck-Boost switching in PSM mode
void FPWM_2PhaseBB_Enable(uint8_t I2CAddress);
void FPWM_2PhaseBB_Disable(uint8_t I2CAddress);
// Enabling/Disabling forced bias selection for VCC2 supply
void ForceBias_Enable(uint8_t I2CAddress);
void ForceBias_Disable(uint8_t I2CAddress);
// Enabling/Disabling direct startup on DTRK mode
void DTRK_DirectStartup_Enable(uint8_t I2CAddress);
void DTRK_DirectStartup_Disable(uint8_t I2CAddress);
// Setting nFLT/nINT pin's functionality
void nFLT_as_INT_Enable(uint8_t I2CAddress);
void nFLT_as_INT_Disable(uint8_t I2CAddress);
// Configuring thermal warning threshold
void ThermalWarning_ThresholdConfigure(uint8_t I2CAddress,uint8_t Threshold);
// Enabling/Disabling thermal threshold warnings
void ThermalWarning_Enable(uint8_t I2CAddress);
void ThermalWarning_Disable(uint8_t I2CAddress);

// Functions for the MFR_SPECIFIC_D2 register
// Enabling/Disabling discharge until VTH is reached
void Discharge_VTH_Enable(uint8_t I2CAddress);
void Discharge_VTH_Disable(uint8_t I2CAddress);
// Enabling/Disabling discharge together with CONV_EN
void Discharge_Enable(uint8_t I2CAddress);
void Discharge_Disable(uint8_t I2CAddress);
// Configuring Discharge Strength
void Dishcarge_StrengthConfigure(uint8_t I2CAddress,uint8_t Strength);
// Configuring the slewrate for DVS
void DVS_SlewrateConfigure(uint8_t I2CAddress, uint8_t Slewrate);
// Enabling/Disabling Active Down Ramp on DVS
void DVS_ActiveDownRamp_Enable(uint8_t I2CAddress);
void DVS_ActiveDownRamp_Disable(uint8_t I2CAddress);

// Functions for the MFR_SPECIFIC_D3 register
// Configure VDET falling threshold
void VDET_FallingThresholdConfigure(uint8_t I2CAddress,uint16_t Threshold);
// Enabling/Disabling VDET functionality
void VDET_Enable(uint8_t I2CAddress);
void VDET_Disable(uint8_t I2CAddress);
// Enabling/Disabling Input Voltage Regulation (IVR)
void IVP_InputVoltageRegulation_Enable(uint8_t I2CAddress);
void IVP_InputVoltageRegulation_Disable(uint8_t I2CAddress);
// Enabling/Disabling Input Voltage Protection (IVP)
void IVP_Enable(uint8_t I2CAddress);
void IVP_Disable(uint8_t I2CAddress);

// Functions for the MFR_SPECIFIC_D4 register
// Configure VDET rising threshold
void VDET_RisingThresholdConfigure(uint8_t I2CAddress,uint16_t Threshold);

// Functions for the MFR_SPECIFIC_D5 register
// Configure secondary OVP2 voltage threshold
void OVP_SecondaryThreshold_Configure(uint8_t I2CAddress,uint16_t Threshold);

// Functions for the MFR_SPECIFIC_D6 register
// Selecting Buck-Boost scaling of minimum on-time and off-time
void BB_MinTimeScale_Select(uint8_t I2CAddress,uint8_t Scale);
// Select the minimum dead-time at fsw=2MHz
void GDRV_MinDeadTime_Select(uint8_t I2CAddress,uint8_t DeadTime);
// Enabling/Disabling frequency dependent scaling of dead-time
void GDRV_DeadTimeScaling_Enable(uint8_t I2CAddress);
void GDRV_DeadTimeScaling_Disable(uint8_t I2CAddress);
// Enabling/Disabling forced constant deadtime on the gate driver
void GDRV_ForceConstantDeadTime_Enable(uint8_t I2CAddress);
void GDRV_ForceConstantDeadTime_Disable(uint8_t I2CAddress);
// Configuring frequency synchronization of the oscillator frequency
void OSC_FreqSyncConfigure(uint8_t I2CAddress,uint8_t SyncFunction);

// Functions for the MFR_SPECIFIC_D7 register
// Selecting the correnction factor of slope compensation
void SlopeComp_CorrectionFactor_Select(uint8_t I2CAddress,uint8_t CorrectionFactor);
// Selecting the Inductor de-rating value for PSM mode to slope
void SlopeComp_InductorDerating_Select(uint8_t I2CAddress,uint8_t InductorDerating);

// Functions for the MFR_SPECIFIC_D8 register
// Selecting the driver configuration for the DRV1 pin
void DRV1_Supply_Configure(uint8_t I2CAddress,uint8_t DRV1Config);
// Selecting the sequencing of the DRV1 pin
void DRV1_Sequence_Configure(uint8_t I2CAddress,uint8_t DRV1Sequence);
// Selecting the gain for Cable Drop Compensation (CDC)
void CDC_GainVoltage_Select(uint8_t I2CAddress,uint8_t GainVoltage);
// Enabling/Disabling Cable Drop Compensation (CDC)
void CDC_Enable(uint8_t I2CAddress);
void CDC_Disable(uint8_t I2CAddress);
// Selecting either 10 or 20 voltage divider on internal FB
void LM51772_FB_Divider_Sel20(uint8_t I2CAddress);
void LM51772_FB_Divider_Sel10(uint8_t I2CAddress);

// Functions for the MFR_SPECIFIC_D9 register
// Configuring the lower voltage window for PCM operation
void PCM_LowerVoltageWindow_Configure(uint8_t I2CAddress,uint16_t LowerWindow); // No float
void PCM_LowerVoltageWindow_ConfigureF(uint8_t I2CAddress,float LowerWindow); // Float
// Enabling/Disabling forcing ISET pin reference over ILIM DAC
void OCP_ISET_OverILIM_Enable(uint8_t I2CAddress);
void OCP_ISET_OverILIM_Disable(uint8_t I2CAddress);

// Functions for the IVP_VOLTAGE register
// Setting the IVP protection and regulation threshold
void IVP_VoltageThreshold_Configure(uint8_t I2CAddress,uint16_t Threshold);
