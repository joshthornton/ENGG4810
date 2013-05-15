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

#define FREQ 100

#define NUM_LEVELS		(128.0f)
#define FREQ_OFFSET		(20.0f)
#define FREQ_GRADIENT	( (20000.0f-FREQ_OFFSET) / NUM_LEVELS )
#define Q_GRADIENT		( 1.0f / NUM_LEVELS )
#define SIZE_GRADIENT	( BUFFER_SIZE / NUM_LEVELS )

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

	for ( i = 3; i < 4; i++ )
	{
		temp[i] = temp[i] >> 5; // 7-bit number (128 steps)
		if ( temp[i] != adcValues[i] )
		{

			if ( i <= 2 ) // Effect one parameters changed
			{
				if ( cfg.effectOne & COEFF_MASK )
					load_generate_coeffs( cfg.effectOne, temp[0] * FREQ_GRADIENT + FREQ_OFFSET , temp[1] * Q_GRADIENT );
				else if ( cfg.effectOne == EFFECT_DELAY )
					load_set_delay( temp[0] * SIZE_GRADIENT, temp[1] * Q_GRADIENT ); 
				else if ( cfg.effectOne == EFFECT_ECHO )
					load_set_echo( temp[0] * SIZE_GRADIENT, temp[1] * Q_GRADIENT ); 

			} else { // Effect two parameters changed

				if ( cfg.effectTwo & COEFF_MASK )
					load_generate_coeffs( cfg.effectTwo, temp[2] * FREQ_GRADIENT + FREQ_OFFSET , temp[3] * Q_GRADIENT );
				else if ( cfg.effectTwo == EFFECT_DELAY )
					load_set_delay( temp[2] * SIZE_GRADIENT, temp[3] * Q_GRADIENT ); 
				else if ( cfg.effectTwo == EFFECT_ECHO )
					load_set_echo( temp[2] * SIZE_GRADIENT, temp[3] * Q_GRADIENT ); 
			}
			
			adcValues[i] = (unsigned char)temp[i];
		}

	}



}
