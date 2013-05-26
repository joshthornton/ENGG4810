#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/adc.h"

#include "inc/hw_ssi.h"
#include "driverlib/ssi.h"


#include "adc.h"
#include "config.h"
#include "load.h"
#include "buffer.h"

// Create global symbol
unsigned char adcValues[4];

#define FREQ 2


void adc_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

	//Set what the timer runs to
	TimerLoadSet(TIMER1_BASE, TIMER_A, ROM_SysCtlClockGet() / FREQ );

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);

	ADCSequenceDisable(ADC0_BASE, 1);
	ADCSequenceConfigure(ADC_BASE, 1, ADC_TRIGGER_TIMER, 0);

	ADCSequenceStepConfigure(ADC_BASE, 1, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC_BASE, 1, 1, ADC_CTL_CH1);
	ADCSequenceStepConfigure(ADC_BASE, 1, 2, ADC_CTL_CH2);
	ADCSequenceStepConfigure(ADC_BASE, 1, 3, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END);

	ADCSequenceEnable(ADC0_BASE, 1);
	ADCIntEnable(ADC0_BASE, 1);

	TimerEnable(TIMER1_BASE, TIMER_A);

	//Set timer 1A to trigger the ADC
	TimerControlTrigger(TIMER1_BASE, TIMER_A, 1);

	//May be useful:
	//SysCtlADCSpeedGet();

	adcValues[0] = 0;
	adcValues[1] = 0;
	adcValues[2] = 0;
	adcValues[3] = 0;

	ADCIntClear(ADC0_BASE, 1);
	ADCIntEnable(ADC0_BASE, 1);
	IntEnable(INT_ADC0SS1);
}

//triggers when it's got all the channels read
void adc_interrupt(void)
{
	unsigned long temp[4];
	unsigned long i;
	ADCIntClear(ADC0_BASE, 1);
	ADCSequenceDataGet(ADC0_BASE, 1, temp);
	ADCIntClear(ADC0_BASE, 1);

	/*
	 * 3
	 * 2
	 * 1
	 * 0
	 */

	for (i = 0; i < 4; i++)
	{
		//need to shift them down first
		temp[i] = temp[i] >> 5; // 7-bit number (128 steps)
	}
	for ( i = 0; i < 4; i++ )
	{

		if (temp[i] > (adcValues[i] + 2) || temp[i] < (adcValues[i] - 2)) // temp[i] != adcValues[i] )
		{
			if ( i <= 2 ) // Effect one parameters changed
			{
				load_set_params(0, temp[0], temp[1]);

			} else { // Effect two parameters changed

				load_set_params(1, temp[2], temp[3]);
			}
			
			adcValues[i] = (unsigned char)temp[i];
		}

	}



}
