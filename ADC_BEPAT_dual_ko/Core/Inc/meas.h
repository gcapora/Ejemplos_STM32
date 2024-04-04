/* ******************************************************************************
 * File:		meas.h		HEADER file
 * Project: 	Dual ADC DMA regular simultaneous mode Example 01
 * Description	Analog Input Measurement
 * Created on: 	Nov 26, 2020
 * Author: 		cb
 *******************************************************************************/

#ifndef INC_MEAS_H_
#define INC_MEAS_H_

#ifdef __cplusplus
 extern "C" {
#endif

 // ********************************
 // *********** INCLUDES ***********
 // ********************************

// ********* SYSTEM LIBS **********
#include "main.h"

// ******** Project LIBS **********
// none so far

// ********************************
// *********** DEFINES ************
// ********************************

#define MEAS_ADC1_CHANNELS 4			// ADC channels
   // En el ejemplo original eran 6, incluyendo temperatura.

// ********************************
// *********** TYPEDEFS ***********
// ********************************

// none so far

// ********************************
// ****** GLOBAL VARIABLES ********
// ********************************

 extern uint8_t MEAS_ACD1_update;				// measurement complete flag

// ********************************
// ***** Function Prototypes ******
// ********************************

uint8_t MEAS_ADC_start(void);
void MEAS_ADC_stop(void);
void MEAS_ADC1_eval(uint8_t pos);

#ifdef __cplusplus
}
#endif

#endif /* INC_MEAS_H_ */
