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
#include "utils/uartstdio.h"
#include "inc/hw_ssi.h"
#include "driverlib/ssi.h"

#define FREQ 100

void adc_init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

	//Set what the timer runs to
	TimerLoadSet(TIMER0_BASE, TIMER_A, ROM_SysCtlClockGet() / FREQ );

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
	unsigned long adcValue[4];

	ADCIntClear(ADC0_BASE, 1);

	ADCSequenceDataGet(ADC0_BASE, 1, adcValue);

	ADCIntClear(ADC0_BASE, 1);
}