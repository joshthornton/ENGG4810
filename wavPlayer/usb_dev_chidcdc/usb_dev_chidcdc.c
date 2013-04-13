//****************************************************************************
//
// usb_dev_mouse.c - Main routines for the enumeration example.
//
// Copyright (c) 2010-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the EK-LM3S9D92 Firmware Package.
//
//****************************************************************************

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
#include "usblib/device/usbdhidmouse.h"
#include "usb_structs.h"
#include "inc/hw_ints.h"
#include "driverlib/usb.h"
#include "driverlib/udma.h"
#include "driverlib/pin_map.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdmsc.h"
#include "third_party/fatfs/src/ff.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "inc/hw_ssi.h"
#include "driverlib/ssi.h"

#define EVER ;;
#define DUMMY (0x0000)
#define IO_READ (0x41)
#define IO_WRITE (0x40)
#define A_MASK (0x00)
#define B_MASK (0x10)
#define IODIR (0x00)

BYTE buffer[2][4096];   /* file copy buffer */

int buffNum = 0;

void pcm56p_write(signed long val )
{
	SSIDataPut( SSI2_BASE, val ); // Write Value
	while( ROM_SSIBusy( SSI2_BASE ) );
}

void Timer0IntHandler(void)
{

	static long i = 0;
	static signed long val = 0;
	static signed long val1 = 0;
	static signed long val2 = 0;
	//static int dir = 0;

    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    HWREGBITW(&g_ulFlags, 0) ^= 1;


    val1 = buffer[buffNum][i] << 8;
    i++;
    val2 = buffer[buffNum][i];
    val = val1 | val2;

    pcm56p_write(val);

    i++;

    if (i >= 4096)
    {
    	buffNum = !buffNum;
    	i = 0;
    }


    ROM_IntMasterDisable();
    ROM_IntMasterEnable();
}

void InitDACSSI(void)
{
	unsigned long temp;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinConfigure(GPIO_PB4_SSI2CLK); // pin 5
	GPIOPinConfigure(GPIO_PB5_SSI2FSS); // pin 6
	GPIOPinConfigure(GPIO_PB6_SSI2RX);
	GPIOPinConfigure(GPIO_PB7_SSI2TX); //pin 7
	//GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3 ); //latch

	GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5  | GPIO_PIN_6 | GPIO_PIN_7);

	SSIConfigSetExpClk(SSI2_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, // Falling edge clock == 3
	  SSI_MODE_MASTER, 10000000, 16); // 10MHz clock

	SSIEnable(SSI2_BASE);

	while(SSIDataGetNonBlocking(SSI2_BASE, &temp));
}

void InitTimer0( unsigned long hertz )
{
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ROM_SysCtlClockGet() / hertz );
	ROM_IntEnable(INT_TIMER0A);
	ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerEnable(TIMER0_BASE, TIMER_A);
}

//****************************************************************************
//
//! \addtogroup example_list
//! <h1>USB composite HID Mouse and CDC serial Device (usb_dev_chidcdc)</h1>
//!
//! This example application turns the evaluation board into a composite USB
//! mouse supporting the Human Interface Device class and a CDC serial device
//! The mouse pointer will move in a square pattern for the duration of the
//! time it is plugged in.  The serial port is used as a command prompt to
//! change the behavior of the board.  By default the mouse will simply
//! enumerate and not move.  The serial port can then be opened and a command
//! can be issued to start the mouse moving or stop it again.
//!
//! The commands supported by the UART are the following:
//!
//! ? or help or h - Will display the help message.
//!
//! led <on|off|toggle|activity>
//! * on       - Turns on the LED.
//! * off      - Turns off the LED
//! * toggle   - Toggle the LED
//! * activity - Toggle the LED due to serial activity.
//!
//! mouse <on|off>
//! * on  - Starts the mouse moving in a square pattern.
//! * off - Stops the mouse moving.
//!
//! Assuming you installed StellarisWare in the default directory, a driver
//! information (INF) file for use with Windows XP, Windows Vista and
//! Windows7 can be found in C:/StellarisWare/windows_drivers. For Windows
//! 2000, the required INF file is in C:/StellarisWare/windows_drivers/win2K.
//
//****************************************************************************
#define FLAG_UPDATE_STATUS      1
static unsigned long g_ulIdleTimeout;

//****************************************************************************
//
// The system tick timer rate.
//
//****************************************************************************
#define SYSTICKS_PER_SECOND     100
#define MS_PER_SYSTICK          (1000 / SYSTICKS_PER_SECOND)

//****************************************************************************
//
// Holds command bits used to signal the main loop to perform various tasks.
//
//****************************************************************************
volatile unsigned long g_ulFlags;

//****************************************************************************
//
// Global system tick counter holds elapsed time since the application started
// expressed in 100ths of a second.
//
//****************************************************************************
volatile unsigned long g_ulSysTickCount;

//****************************************************************************
//
// The memory allocated to hold the composite descriptor that is created by
// the call to USBDCompositeInit().
//
//****************************************************************************
#define DESCRIPTOR_DATA_SIZE    (COMPOSITE_DCDC_SIZE + COMPOSITE_DHID_SIZE)
unsigned char g_pucDescriptorData[DESCRIPTOR_DATA_SIZE];

//****************************************************************************
//
// External mouse definitions used by the composite main routine.
//
//****************************************************************************
extern const tMSCInstance g_sMSCDevice;

//****************************************************************************
//
// External CDC serial definitions used by the composite main routine.
//
//****************************************************************************
extern const tUSBDCDCDevice g_sCDCDevice;

//****************************************************************************
//
// External composite device definitions used by the composite main routine.
//
//****************************************************************************
extern tUSBDCompositeDevice g_sCompDevice;
extern tCompositeEntry g_psCompDevices[];

//Fatfs vars
static FATFS g_sFatFs;
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
// Generic event handler for the composite device.
//
//****************************************************************************
unsigned long
EventHandler(void *pvCBData, unsigned long ulEvent, unsigned long ulMsgData,
             void *pvMsgData)
{
    switch(ulEvent)
    {
        //
        // The host has connected to us and configured the device.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // Now connected.
            //
            HWREGBITW(&g_ulFlags, FLAG_CONNECTED) = 1;

            break;
        }

        //
        // Handle the disconnect state.
        //
        case USB_EVENT_DISCONNECTED:
        {
            //
            // No longer connected.
            //
            HWREGBITW(&g_ulFlags, FLAG_CONNECTED) = 0;

            break;
        }

        default:
        {
            break;
        }
    }

    return(0);
}


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



//*****************************************************************************
//
// The number of ticks to wait before falling back to the idle state.  Since
// the tick rate is 100Hz this is approximately 3 seconds.
//
//*****************************************************************************
#define USBMSC_ACTIVITY_TIMEOUT 300



//*****************************************************************************
//
// This enumeration holds the various states that the device can be in during
// normal operation.
//
//*****************************************************************************
volatile enum
{
    //
    // Unconfigured.
    //
    MSC_DEV_DISCONNECTED,

    //
    // Connected but not yet fully enumerated.
    //
    MSC_DEV_CONNECTED,

    //
    // Connected and fully enumerated but not currently handling a command.
    //
    MSC_DEV_IDLE,

    //
    // Currently reading the SD card.
    //
    MSC_DEV_READ,

    //
    // Currently writing the SD card.
    //
    MSC_DEV_WRITE,
}
g_eMSCState;


//******************************************************************************
//
// The DMA control structure table.
//
//******************************************************************************
#ifdef ewarm
#pragma data_alignment=1024
tDMAControlTable sDMAControlTable[64];
#elif defined(ccs)
#pragma DATA_ALIGN(sDMAControlTable, 1024)
tDMAControlTable sDMAControlTable[64];
#else
tDMAControlTable sDMAControlTable[64] __attribute__ ((aligned(1024)));
#endif


//*****************************************************************************
//
// This function is the call back notification function provided to the USB
// library's mass storage class.
//
//*****************************************************************************
unsigned long
USBDMSCEventCallback(void *pvCBData, unsigned long ulEvent,
                     unsigned long ulMsgParam, void *pvMsgData)
{
    //
    // Reset the time out every time an event occurs.
    //
    g_ulIdleTimeout = USBMSC_ACTIVITY_TIMEOUT;

    switch(ulEvent)
    {
        //
        // Writing to the device.
        //
        case USBD_MSC_EVENT_WRITING:
        {
            //
            // Only update if this is a change.
            //
            if(g_eMSCState != MSC_DEV_WRITE)
            {
                //
                // Go to the write state.
                //
                g_eMSCState = MSC_DEV_WRITE;

                //
                // Cause the main loop to update the screen.
                //
                g_ulFlags |= FLAG_UPDATE_STATUS;
            }

            break;
        }

        //
        // Reading from the device.
        //
        case USBD_MSC_EVENT_READING:
        {
            //
            // Only update if this is a change.
            //
            if(g_eMSCState != MSC_DEV_READ)
            {
                //
                // Go to the read state.
                //
                g_eMSCState = MSC_DEV_READ;

                //
                // Cause the main loop to update the screen.
                //
                g_ulFlags |= FLAG_UPDATE_STATUS;
            }

            break;
        }
        //
        // The USB host has disconnected from the device.
        //
        case USB_EVENT_DISCONNECTED:
        {
            //
            // Go to the disconnected state.
            //
            g_eMSCState = MSC_DEV_DISCONNECTED;

            //
            // Cause the main loop to update the screen.
            //
            g_ulFlags |= FLAG_UPDATE_STATUS;

            break;
        }
        //
        // The USB host has connected to the device.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // Go to the idle state to wait for read/writes.
            //
            g_eMSCState = MSC_DEV_IDLE;

            break;
        }
        case USBD_MSC_EVENT_IDLE:
        default:
        {
            break;
        }
    }

    return(0);
}

void msc_main()
{

    switch(g_eMSCState)
	{
		case MSC_DEV_READ:
		{
			//
			// Update the screen if necessary.
			//
			if(g_ulFlags & FLAG_UPDATE_STATUS)
			{

				g_ulFlags &= ~FLAG_UPDATE_STATUS;
			}

			//
			// If there is no activity then return to the idle state.
			//
			if(g_ulIdleTimeout == 0)
			{

				g_eMSCState = MSC_DEV_IDLE;
			}

			break;
		}
		case MSC_DEV_WRITE:
		{
			//
			// Update the screen if necessary.
			//
			if(g_ulFlags & FLAG_UPDATE_STATUS)
			{

				g_ulFlags &= ~FLAG_UPDATE_STATUS;
			}

			//
			// If there is no activity then return to the idle state.
			//
			if(g_ulIdleTimeout == 0)
			{

				g_eMSCState = MSC_DEV_IDLE;
			}
			break;
		}
		case MSC_DEV_DISCONNECTED:
		{
			//
			// Update the screen if necessary.
			//
			if(g_ulFlags & FLAG_UPDATE_STATUS)
			{

				g_ulFlags &= ~FLAG_UPDATE_STATUS;
			}
			break;
		}
		case MSC_DEV_IDLE:
		{
			break;
		}
		default:
		{
			break;
		}
	}
}

void msc_init()
{
	//
	// Configure and enable uDMA
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
	SysCtlDelay(10);
	ROM_uDMAControlBaseSet(&sDMAControlTable[0]);
	ROM_uDMAEnable();

	//
	// Initialize the idle timeout and reset all flags.
	//
	g_ulIdleTimeout = 0;
	g_ulFlags = 0;
	g_eMSCState = MSC_DEV_DISCONNECTED;


}

void composite_device_init()
{
	//
	// Pass the USB library our device information, initialize the USB
	// controller and connect the device to the bus.
	//
	g_psCompDevices[0].pvInstance =
			USBDMSCInit(0, (tUSBDMSCDevice *)&g_sMSCDevice);
	g_psCompDevices[1].pvInstance =
		USBDCDCCompositeInit(0, (tUSBDCDCDevice *)&g_sCDCDevice);

	//
	// Set the USB stack mode to Device mode with VBUS monitoring.
	//
	USBStackModeSet(0, USB_MODE_DEVICE, 0);

	//
	// Pass the device information to the USB library and place the device
	// on the bus.
	//
	USBDCompositeInit(0, &g_sCompDevice, DESCRIPTOR_DATA_SIZE,
					  g_pucDescriptorData);

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

//****************************************************************************
//
// This is the main loop that runs the application.
//
//****************************************************************************
int
main(void)
{
	ROM_FPULazyStackingEnable();
	ROM_IntMasterEnable();


    //80 Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);


    //
    // Set the system tick to fire 100 times per second.
    //
    ROM_SysTickPeriodSet(ROM_SysCtlClockGet() / SYSTICKS_PER_SECOND);
    ROM_SysTickIntEnable();
    ROM_SysTickEnable();


    msc_init();
    composite_device_init();
    SerialInit();

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
	res = f_open(&fsrc, "jazz.dat", FA_OPEN_EXISTING | FA_READ);
	res = f_read(&fsrc, buffer[buffNum], sizeof buffer[buffNum], &br);
	res = f_read(&fsrc, buffer[!buffNum], sizeof buffer[buffNum], &br);

	int lastBuffNum = buffNum;

	InitDACSSI();
	InitTimer0( 48000 );

    while(1)
    {

    		//check if buffNum changed
    		if (lastBuffNum != buffNum)
    		{
    			res = f_read(&fsrc, buffer[!buffNum], sizeof buffer[0], &br);

				if (br < sizeof buffer[0])
				{
					f_lseek(&fsrc, 0);
				}

				lastBuffNum = buffNum;
    		}

    		/*res = f_open(&fsrc, "guitar.dat", FA_OPEN_EXISTING | FA_READ);



    	    for (;;) {
    	        res = f_read(&fsrc, buffer, sizeof buffer, &br);
    	        if (res || br == 0) break;

    	        CommandPrint(buffer);
    	    }


    	    f_close(&fsrc);*/


        SerialMain();
        msc_main();
    }
}
