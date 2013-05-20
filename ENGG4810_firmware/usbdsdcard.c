//*****************************************************************************
//
// usbdsdcard.c - Routines supplied for use by the mass storage class device
// class.
//
// Copyright (c) 2012 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 9453 of the EK-LM4F232 Firmware Package.
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/systick.h"
#include "usblib/usblib.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdmsc.h"
#include "usb_structs.h"
#include "third_party/fatfs/src/diskio.h"
#include "third_party/fatfs/src/ff.h"

#include "usblib/usblib.h"
#include "usblib/usbhid.h"
#include "usblib/usbcdc.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbdcomp.h"
#include "usblib/device/usbdcdc.h"
#include "usblib/device/usbdhid.h"
#include "usblib/device/usbdhidmouse.h"
#include "usb_structs.h"

#include "driverlib/udma.h"

#define FLAG_UPDATE_STATUS      1
//****************************************************************************
//
// The memory allocated to hold the composite descriptor that is created by
// the call to USBDCompositeInit().
//
//****************************************************************************
#define DESCRIPTOR_DATA_SIZE    (COMPOSITE_DCDC_SIZE + COMPOSITE_DHID_SIZE)
unsigned char g_pucDescriptorData[DESCRIPTOR_DATA_SIZE];

unsigned long g_ulIdleTimeout;

//****************************************************************************
//
// External MSC definitions used by the composite main routine.
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
tCompositeEntry backUpCompDevices[];

#define SDCARD_PRESENT          0x00000001
#define SDCARD_IN_USE           0x00000002

extern volatile unsigned long g_ulFlags;
extern volatile unsigned long g_ulSysTickCount;

//*****************************************************************************
//
// The number of ticks to wait before falling back to the idle state.  Since
// the tick rate is 100Hz this is approximately 3 seconds.
//
//*****************************************************************************
#define USBMSC_ACTIVITY_TIMEOUT 300


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


void *store = 0;


void composite_device_init()
{

	tUSBDMSCDevice *psDevice;
	//
	// Pass the USB library our device information, initialize the USB
	// controller and connect the device to the bus.
	//
	g_psCompDevices[0].pvInstance =
			USBDMSCInit(0, (tUSBDMSCDevice *)&g_sMSCDevice);

	g_psCompDevices[1].pvInstance =
		USBDCDCCompositeInit(0, (tUSBDCDCDevice *)&g_sCDCDevice);


	backUpCompDevices[0].pvInstance = g_psCompDevices[0].pvInstance;
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

	psDevice = g_psCompDevices[0].pvInstance;
	store = psDevice->psPrivateData->pvMedia; //make a copy of the media to restore later
}

void myUSBDMSCTerm(void *pvInstance)
{
    const tUSBDMSCDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Cleanly exit device mode.
    //
    USBDCDTerm(0);

    //
    // Create a device instance pointer.
    //
    psDevice = pvInstance;

    //
    // If the media was opened the close it out.
    //


    if(psDevice->psPrivateData->pvMedia != 0)
    {
        psDevice->psPrivateData->pvMedia = 0;
        psDevice->sMediaFunctions.Close(0);
    }


    psDevice->psPrivateData->bConnected = 0;

}

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

int transfer = 0;

extern FATFS g_sFatFs;

/* Only call after disabling msc */
void enable_msc()
{
	transfer = 1;
	tUSBDMSCDevice *psDevice;
	psDevice = g_psCompDevices[0].pvInstance;

	//reconnect the device and readd the media
	psDevice->psPrivateData->pvMedia = store;
	psDevice->psPrivateData->bConnected = 1;

	//dont think i have to do this actually
	f_mount(0, 0);
	f_mount(0, &g_sFatFs);

}

/* Very ineloquently disables msc by destroying the pvInstance. This makes the drive still mount
 * but doesn't allow the sd card to be written to/read from
 */
void disable_msc()
{
	transfer = 0;
	tUSBDMSCDevice *psDevice;

	psDevice = g_psCompDevices[0].pvInstance;


	psDevice->psPrivateData->pvMedia = 0; //delete the media
    psDevice->psPrivateData->bConnected = 0; //disconnect the device
}

struct
{
    unsigned long ulFlags;
}
g_sDriveInformation;

//*****************************************************************************
//
// This function opens the drive number and prepares it for use by the Mass
// storage class device.
//
// /param ulDrive is the driver number to open.
//
// This function is used to initialize and open the physical drive number
// associated with the parameter /e ulDrive.  The function will return zero if
// the drive could not be opened for some reason.  In the case of removable
// device like an SD card this function should return zero if the SD card is
// not present.
//
// /return Returns a pointer to data that should be passed to other APIs or it
// will return 0 if no drive was found.
//
//*****************************************************************************
void *
USBDMSCStorageOpen(unsigned long ulDrive)
{
    unsigned long ulTemp;

    ASSERT(ulDrive == 0);

    //
    // Return if already in use.
    //
    if(g_sDriveInformation.ulFlags & SDCARD_IN_USE)
    {
        return(0);
    }

    //
    // Initialize the drive if it is present.
    //


    ulTemp = disk_initialize(0);


    if(ulTemp == RES_OK)
    {
        //
        // Card is present and in use.
        //
        g_sDriveInformation.ulFlags = SDCARD_PRESENT | SDCARD_IN_USE;
    }
    else if(ulTemp == STA_NODISK)
    {
        //
        // Allocate the card but it is not present.
        //
        g_sDriveInformation.ulFlags = SDCARD_IN_USE;
    }
    else
    {
        return(0);
    }

    return((void *)&g_sDriveInformation);
}

//*****************************************************************************
//
// This function close the drive number in use by the mass storage class device.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
//
// This function is used to close the physical drive number associated with the
// parameter /e pvDrive.  This function will return 0 if the drive was closed
// successfully and any other value will indicate a failure.
//
// /return Returns 0 if the drive was successfully closed or non-zero for a
// failure.
//
//*****************************************************************************
void
USBDMSCStorageClose(void * pvDrive)
{
    //unsigned char ucPower;

    ASSERT(pvDrive != 0);

    //
    // Clear all flags.
    //
    g_sDriveInformation.ulFlags = 0;

    //
    // Power up the card.
    //
    //ucPower = 0;

    //
    // Turn off the power to the card.
    //
    //disk_ioctl(0, CTRL_POWER, &ucPower);
}

//*****************************************************************************
//
// This function will read a block from a device opened by the
// USBDMSCStorageOpen() call.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
// /param pucData is the buffer that data will be written into.
// /param ulNumBlocks is the number of blocks to read.
//
// This function is use to read blocks from a physical device and return them
// in the /e pucData buffer.  The data area pointed to by /e pucData should be
// at least /e ulNumBlocks * Block Size bytes to prevent overwriting data.
//
// /return Returns the number of bytes that were read from the device.
//
//*****************************************************************************
unsigned long USBDMSCStorageRead(void * pvDrive,
                                 unsigned char *pucData,
                                 unsigned long ulSector,
                                 unsigned long ulNumBlocks)
{
    ASSERT(pvDrive != 0);

    if(disk_read (0, pucData, ulSector, ulNumBlocks) == RES_OK)
    {
        // TODO remove fixed 512
        return(ulNumBlocks * 512);
    }
    return(0);
}

//*****************************************************************************
//
// This function will write a block to a device opened by the
// USBDMSCStorageOpen() call.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
// /param pucData is the buffer that data will be used for writing.
// /param ulNumBlocks is the number of blocks to write.
//
// This function is use to write blocks to a physical device from the buffer
// pointed to by the /e pucData buffer.  If the number of blocks is greater than
// one then the block address will increment and write to the next block until
// /e ulNumBlocks * Block Size bytes have been written.
//
// /return Returns the number of bytes that were written to the device.
//
//*****************************************************************************
unsigned long USBDMSCStorageWrite(void * pvDrive,
                                  unsigned char *pucData,
                                  unsigned long ulSector,
                                  unsigned long ulNumBlocks)
{
    ASSERT(pvDrive != 0);

    if(disk_write(0, pucData, ulSector, ulNumBlocks) == RES_OK)
    {
        return(ulNumBlocks * 512);
    }
    return(0);
}

//*****************************************************************************
//
// This function will return the number of blocks present on a device.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
//
// This function is used to return the total number of blocks on a physical
// device based on the /e pvDrive parameter.
//
// /return Returns the number of blocks that are present in a device.
//
//*****************************************************************************
unsigned long
USBDMSCStorageNumBlocks(void * pvDrive)
{
    unsigned long ulSectorCount;

    //
    // Read the number of sectors.
    //
    disk_ioctl(0, GET_SECTOR_COUNT, &ulSectorCount);

    return(ulSectorCount);
}

//*****************************************************************************
//
// This function will return the current status of a device.
//
// /param pvDrive is the pointer that was returned from a call to
// USBDMSCStorageOpen().
//
// This function is used to return the current status of the device indicated by
// the /e pvDrive parameter.  This can be used to see if the device is busy,
// or if it is present.
//
// /return Returns the size in bytes of blocks that in a device.
//
//*****************************************************************************
#define USBDMSC_IDLE            0x00000000
#define USBDMSC_NOT_PRESENT     0x00000001
unsigned long USBDMSCStorageStatus(void * pvDrive);


volatile unsigned long g_ulFlags;

//****************************************************************************
//
// The memory allocated to hold the composite descriptor that is created by
// the call to USBDCompositeInit().
//
//****************************************************************************
#define DESCRIPTOR_DATA_SIZE    (COMPOSITE_DCDC_SIZE + COMPOSITE_DHID_SIZE)
unsigned char g_pucDescriptorData[DESCRIPTOR_DATA_SIZE];





//*****************************************************************************
//
// This function is the call back notification function provided to the USB
// library's mass storage class.
//
//*****************************************************************************
unsigned long USBDMSCEventCallback(void *pvCBData, unsigned long ulEvent,
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

