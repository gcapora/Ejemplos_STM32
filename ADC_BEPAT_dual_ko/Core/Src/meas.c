/* ******************************************************************************
 * File:		rtb_meas.c
 * Project: 	Dual ADC DMA regular simultaneous mode Example 01
 * Description	Analog Input Measurement
 * Created on: 	Nov 26, 2020
 * Author: 		cb
 *******************************************************************************/

// ********************************
// *********** INCLUDES ***********
// ********************************

#include "meas.h"

// ********* SYSTEM LIBS **********
#include "adc.h"
#include <math.h>

uint8_t MEAS_ACD1_update;

// ********************************
// *********** DEFINES ************
// ********************************

#define ADC1_OVERSAMLING 2	// ADC value shifting (oversamling)
// 0x Oversamling: ADC-REsolution = 12 bit (2^12 = 4096)
// 1x Oversampling: 12 bit --> 13 bit (2^13 = 8192) --> Buffer Size: 4^1 = 4
// 2x Oversampling: 12 bit --> 14 bit (2^14 = 16384) --> Buffer Size: 4^2 = 16
// 3x Oversampling: 12 bit --> 15 bit (2^15 = 32768) --> Buffer Size: 4^3 = 64
// 4x Oversampling: 12 bit --> 16 bit (2^16 = 65535) --> Buffer Size: 4^4 = 256
// Buffer size must match oversamling!!
#define ADC1_BUFFER_SZ 16			// ADC Buffer size per channel

static const float ADC_MaxVal = 16384.0F;	// ADC max. value (12 bit + Oversamling)
static const float ADV_RevVal = 3.3F;		// ADC reference voltage

// Parameters for calculating the CPU Temperature
static const float CPUTemp_V25 = 1.43F;		// 1.43 Volt
static const float CPUTemp_aSlope = 4.3;	// 4.3 mV/°C (from Datasheet STM23F103x8)

// LED1 blink rate (ADC acticity LED)
static const uint16_t ADC_ledctr_max = 500;

// ********************************
// ********** VARIABLES ***********
// ********************************

uint16_t MEAS_samplectr;

// In ADC1+ADC2 Multimode configuration, ADC2 uses the upper two bytes in ADC1 DMA Buffer
// --> channel count needs to be divided by 2
uint32_t              ADC1_Buffer [ADC1_BUFFER_SZ * (MEAS_ADC1_CHANNELS / 2) * 2];		// * 2 --> Double Buffer for DMA
static const uint16_t ADC1_BufferSize = ADC1_BUFFER_SZ * (MEAS_ADC1_CHANNELS / 2);		// ADC1 "half" Buffer size

// these two buffers are only needed for the tutorial
uint16_t ADC1_Brf[ADC1_BUFFER_SZ * (MEAS_ADC1_CHANNELS / 2)];
uint16_t ADC2_Brf[ADC1_BUFFER_SZ * (MEAS_ADC1_CHANNELS / 2)];

uint32_t adc[MEAS_ADC1_CHANNELS];	// ADC1 value accumulator
float adcf[MEAS_ADC1_CHANNELS];		// ADC1 voltage
float CPU_Temp;					// CPU Temperature

// ********************************
// ********** PROTOTYPES **********
// ********************************

void CPU_GetTemperature(float* Voltage, float* CPUTemp);

// ********************************
// *** FUNCTION IMPLEMENTATION ****
// ********************************

uint8_t MEAS_ADC_start(void) {
	// @brief	start ADC
	// @param	none
	// @return	U8 status, 0=OK (no error)

	uint8_t retval = 0;
	uint8_t s = 0;
	HAL_StatusTypeDef status = HAL_OK;

	/*
	status = HAL_ADCEx_Calibration_Start(&hadc1);		// calibrate ADC1
	if(status) retval |= (1 << s);
	s++;
	status = HAL_ADCEx_Calibration_Start(&hadc2);		// calibrate ADC2
	if(status) retval |= (1 << s);
	s++;
	*/
	HAL_Delay(10);

	// Start ADC1+ADC2 in Multimode configuration
	status = HAL_ADC_Start(&hadc2);																	// start ADC2 (slave) first!
	status = HAL_ADCEx_MultiModeStart_DMA(&hadc1, ADC1_Buffer, (uint32_t)(ADC1_BufferSize * 2)); 	// start ADC1 /w Double Buffer

	if(status) retval |= (1 << s);
	s++;

	return(retval);
}

void MEAS_ADC_stop(void) {
	// @brief	stop ADC
	// @param	none
	// @return	none, void

	HAL_ADC_Stop(&hadc2);
	HAL_ADCEx_MultiModeStop_DMA(&hadc1);

}

void MEAS_ADC1_eval(uint8_t pos) {
	// @brief 	ADC Conversion Complete Interrupt Callback
	// @param	U8 pos	--> first half (pos=0) or 2nd half(pos>0) of buffer ...
	// @return	none, void
	// @description / usage
	// ADC+DMA using "double-buffer" technique:
	// the ADC buffer has the size: Channels * Buffer * 2 and
	// is separated into a lower half (--> uint8_t pos) and a upper half
	// there are 2 interrupts:
	// void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
	// which indicates that, the upper end of the Buffer has been reached
	// and
	// void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
	// which indicates, that the first half of the Buffer has been filled.
	// so: while we are working on the upper half, the lower half is filled
	// and vice versa. this makes sure, that there is no interference on the
	// values, while sampling and calculating the results

	uint16_t bP = 0; 					// "bP" = Buffer Position
	if(pos) bP = ADC1_BufferSize;		// use upper half of buffer (Dual ADC)

	// ADC channels are organized as follows:
	// e.g. 3 Channels a, b, c - as defined in the CubeMX ADC setup
	// the resulting buffer looks like
	// [a0,b0,c0,a1,b1,c1,a2,b2,c2 ...]
	// with a,b,c = channels - 0,1,2, = samples
	// in Dual Dual regular simultaneous mode
	// ADC1 uses the lower 2 bytes and ADC2 uses the upper two bytes

	uint16_t i,j;
	uint32_t val;

	for(i=0; i<ADC1_BufferSize; i++) {
		// split ADC1 and ADC2 samples in separate arrays
		// this step is not necessary for an real application,
		// it's just implemented here for the tutorial
		val = *(ADC1_Buffer + bP + i);
		ADC1_Brf[i] = (uint16_t)(val & 0xffff);
		ADC2_Brf[i] = (uint16_t)((val >> 16) & 0xffff);
	}


	// Dual ADC (ADC1 + ADC2 Synchronous)
	// accumulate all ADC readings
	for(i = 0; i<ADC1_BUFFER_SZ; i++) {
		for(j=0;j<(MEAS_ADC1_CHANNELS / 2);j++) {
			val = *(ADC1_Buffer + bP);
			if(i) {
				*(adc + (2* j)) += (val & 0xffff); 				// sum up all elements from ADC1
				*(adc + (2 *j) + 1) += ((val >> 16) & 0xffff); 	// sum up all elements from ADC2
			}
			else {
				*(adc + (2* j)) = (val & 0xffff); 				// initialize new on first run from ADC1
				*(adc + (2 *j) + 1) = ((val >> 16) & 0xffff); 	// initialize new on first run from ADC2
			}
			bP++;
		}
	}

	MEAS_samplectr++;

	// ADC Value to voltage conversion
	for(i=0; i<MEAS_ADC1_CHANNELS; i++) *(adcf+i) = ((float)((*(adc+i)) >> ADC1_OVERSAMLING))/ADC_MaxVal * ADV_RevVal; 		// Oversamling


	CPU_GetTemperature((adcf+4), &CPU_Temp);		// get CPU-Temperature, mesured on 5th channel (ADC1 Temperature sensor)


	if(MEAS_samplectr > ADC_ledctr_max) {
		MEAS_samplectr = 0;
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin); // Activity LED
	}

	MEAS_ACD1_update = 1;	// set ADC1 update flag
}

// ********************************
// *** FUNCTION IMPLEMENTATION ****
// ********************************
// ********* TEMPERATURE **********

void CPU_GetTemperature(float* Voltage, float* CPUTemp) {
	// @brief	convert the voltage reading from the CPU Temperature measurement into °C;
	// @param	SGL* Voltage	--> Pointer to CPU Temperature ADC Voltage reading
	// @param	SGL* CPUTemp	--> Pointer to Temperature variable
	// @return	none, void
	*CPUTemp = ((CPUTemp_V25 - *Voltage)/(CPUTemp_aSlope/1000.0F))+25.0F;
}

