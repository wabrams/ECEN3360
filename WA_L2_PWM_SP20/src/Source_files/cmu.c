//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// prototypes
//***********************************************************************************
void cmu_open(void)
{
	CMU_ClockEnable(cmuClock_HFPER, true);

	// By default, LFRCO is enabled, disable the LFRCO oscillator
	CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);	// using LFXO or ULFRCO

	// Route LF clock to the LF clock tree
	// No requirement to enable the ULFRCO oscillator.  It is always enabled in EM0-4H

	CMU_OscillatorEnable(cmuOsc_LFXO, false, false); // Disable LFXO
	CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);	// route ULFRCO to proper Low Freq clock tree

	// Enabling the High Frequency Peripheral Clock Tree
	//HFLE is CORELE
	CMU_ClockEnable(cmuClock_HFLE, true); // Enable the High Frequency Peripheral clock
}

