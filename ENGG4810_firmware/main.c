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

#define EVER ;;

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

#define BUTTON_1 GPIO_PIN_0
#define BUTTON_2 GPIO_PIN_4

BYTE buffer[2][4096];   /* file copy buffer */

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
static DIR g_sDirObject;
static FILINFO g_sFileInfo;
static FIL g_sFileObject;

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


extern int transfer;;

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


    //
    // Set the system tick to fire 100 times per second.
    //
    ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / SYSTICKS_PER_SECOND);
    ROM_SysTickIntEnable();
    ROM_SysTickEnable();

    init_dac();

    msc_init();
    composite_device_init();
    SerialInit();
    disable_msc();
    adc_init();

    //
    // Drop into the main loop.
    //
	if(f_mount(0, &g_sFatFs) != FR_OK)
	{
		errorLoop("Didnt mount");
	}

	//starts at 0 with 1 full
	//changes to 1 fill 0
	//changes to 0 fill 1

	//get ahead by two buffers
	res = f_open(&fsrc, "test.txt", FA_OPEN_EXISTING | FA_READ);

	int lastBuffNum = buffNum;


	ROM_IntMasterEnable();

	int i = 0;
	while(1)
    {

		if (!transfer)
		{
			/*res = f_open(&fsrc, "test.txt", FA_OPEN_EXISTING | FA_READ);
			res = f_read(&fsrc, buffer[0], sizeof buffer[0], &br);

			if (res || br == 0)
			{

			}
			else
				CommandPrint(buffer[0]);


			f_close(&fsrc);

			for (i = 0; i < 100; i++)
			{
				buffer[0][i] = 0;
			}*/
    	}

    	SerialMain();
    }

}
