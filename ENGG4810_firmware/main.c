#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/fpu.h"
#include "driverlib/systick.h"
#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/usbcdc.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcomp.h"
#include "usblib/device/usbdcdc.h"
#include "usblib/device/usbdhid.h"
#include "usb_structs.h"
#include "inc/hw_ints.h"
#include "driverlib/usb.h"
#include "driverlib/udma.h"
#include "driverlib/pin_map.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdmsc.h"
#include "third_party/fatfs/src/ff.h"
#include "third_party/fatfs/src/diskio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_ssi.h"
#include "driverlib/ssi.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "usbdsdcard.h"
#include "dac.h"
#include "adc.h"
#include "load.h"
#include "poll.h"

#define EVER ;;
int buffNum = 0;
extern unsigned long g_ulIdleTimeout;

//****************************************************************************
//
// The system tick timer rate.
//
//****************************************************************************
#define SYSTICKS_PER_SECOND     100
#define MS_PER_SYSTICK          (1000 / SYSTICKS_PER_SECOND)


extern volatile unsigned long g_ulFlags;
volatile unsigned long g_ulSysTickCount;



//Fatfs vars
FATFS g_sFatFs;


//****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif



//****************************************************************************
//
// This is the interrupt handler for the SysTick interrupt.  It is called
// periodically and updates a global tick counter then sets a flag to tell the
// main loop to move the mouse.
//
//****************************************************************************
void
SysTickHandler(void)
{
    //
    // Increment the current tick count.
    //
    g_ulSysTickCount++;


   //
   // Call the FatFs tick timer.
   //
   disk_timerproc();

   if(g_ulIdleTimeout != 0)
   {
	   g_ulIdleTimeout--;
   }

}


void errorLoop(const char *pcStr)
{
	while(1)
	{
		CommandPrint(pcStr);
		CommandPrint("\r\n");
	}
}
FIL fsrc, fdst;      /* file objects */

FRESULT res;         /* FatFs function common result code */
UINT br, bw;


extern int transfer;
BYTE buf2[30];
FIL fp;
//****************************************************************************
//
// This is the main loop that runs the application.
//
//****************************************************************************
int
main(void)
{
	ROM_FPULazyStackingEnable();

    //80 Mhz

    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / SYSTICKS_PER_SECOND);
    ROM_SysTickIntEnable();
    ROM_SysTickEnable();
    dac_init();



    msc_init();
    composite_device_init();
    SerialInit();
    disable_msc();
    adc_init();

    /*
      Connections for the little birdy SD card adapter
      D3 = Chip select = PA3
	  CMD = MOSI = PA5
	  CLK = CLK = PA2
	  D0 = MISO = PA4
     */

	if(f_mount(0, &g_sFatFs) != FR_OK)
	{
		errorLoop("Didnt mount");
	}

	FRESULT j =  f_open(&fp, "cfg.cfg", FA_OPEN_EXISTING | FA_READ);
    FRESULT res = f_read(&fp, (void *)&cfg, sizeof cfg, &br);

	load_init();
	dac_init();
	poll_init();
	ROM_IntMasterEnable();

	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	//GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);

	while(1)
    {
    	SerialMain();
    	do_work();
    }

}
