/*********************************************************************
 *
 *   Microchip USB HID Bootloader v1.01 for PIC18F97J94 Family Devices
 *
 *********************************************************************
 * FileName:        main.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 3.44+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") for its PIC® Microcontroller is intended and
 * supplied to you, the Company's customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * File Version  Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 1.0			 04/09/2008	Started from MCHPFSUSB v1.3 HID Mouse
 *							demo project.  Main modifications include
 *							changes to HID report descriptor, and
 *							replacement of the "user_mouse.c" file
 *							with the contents contained in the
 *							Boot87J50Family.c file.
 * 2.9f			 02/17/2009	Modified for PIC18F97J94 Family devices.
 ********************************************************************/

/*********************************************************************
IMPORTANT NOTE: This code is currently configured to work with the
PIC18F97J94 FS USB Plug-In Module board.  It can be readily adapted to
work with other members of the PIC18F97J94 Family of USB microcontrollers 
as well.

To do so, modify the linker script for the appropriate FILES includes,
and the new memory ranges (assuming a different memory size device), and
click "Configure --> Select Device" and select the proper
microcontroller.  Also double check to verify that the io_cfg.h and
usbcfg.h are properly configured to match your desired application
platform.

It is also recommended to configure the default I/O pin usage in this code.
See the InitializeSystem() function.

This code is meant to be compiled with the C18 compiler version 3.42+
with all optimizations turned on.  If some of the optimizations are not
enabled, the total code size may grow to exceed the 0x000-0xFFF memory
region this code is designed to occupy.  In this case, linker errors will
occur.  If this happens, the vector remapping in the _entry() function 
will have to be modified, as will the application firmware projects that 
may get programmed using this bootloader firmware (to have an entry
point higher than 0x1000).  Additionally, the linker script in this
project will have to be modified to make the BootPage section larger.
*********************************************************************/


//----------------------------------------------------
//Usage tips for this HID USB bootloader firwmare
//----------------------------------------------------

//To enter this bootloader firmware, hold the RB4 I/O pin low at power
//up or after a reset.  Alternatively, application firmware may enter
//the bootloader firmware by clearing the INTCON<GIE> bit and then
//executing an "_asm goto 0x001C _endasm" instruction.

//If a high priority interrupt occurs, the PC will jump to 0x1008
//If a low priority interrupt occurs, the PC will jump to 0x1018

//If RB4 is high at power up/after reset, this code will jump to
//the application firmware, instead of staying in this bootloader firmware.
//The start of the application firmware should be at 0x1000
//In other words, when developing the application firmware which will be
//programmed with this bootloader, place the following in the code, if
//it is a C18 based project:

//extern void _startup (void);    // See c018i.c in your C18 compiler dir
//#pragma code AppFirmwareStartLocation = 0x1000
//void _reset (void)
//{
//    _asm goto _startup _endasm
//}

//Build the application project with a linker script that marks
//the address range 0x000-0xFFF as "PROTECTED".  This is the program
//memory region that this bootloader is currently configured to occupy.

//Although the bootloader can re-program the program memory page that
//contains the configuration bits (the last page of implemented flash)
//it is not always preferrable to do so in case a user attempts to
//program a hex file with configuration bit settings that are not compatible
//with USB operation.  This would prevent further entry into the bootloader.
//If the bootloader will not be used to program the configuration
//words page, the application firmware's linker script should mark
//the entire page as PROTECTED.


/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "typedefs.h"                   
#include "usb.h"                         
#include "io_cfg.h"                     
#include "Boot18FxxJxxFamily.h"

/** C O N F I G U R A T I O N ************************************************/
// Note: For a complete list of the available config pragmas and their values, 
// see the compiler documentation, and/or click "Help --> Topics..." and then 
// select "PIC18 Config Settings" in the Language Tools section.

#if defined(PIC18F97J94_FS_USB_PIM) || defined(PIC18F87J94_FS_USB_PIM)			

        // Configuration bits for PIC18F97J94 FS USB Plug-In Module board
        #pragma config STVREN   = ON      	// Stack overflow reset
        #pragma config XINST    = OFF   	// Extended instruction set
        #pragma config BOREN    = ON        // BOR Enabled
        #pragma config BORV     = 0         // BOR Set to "2.0V" nominal setting
        #pragma config CP0      = OFF      	// Code protect disabled
        #pragma config FOSC     = FRCPLL    // Firmware should also enable active clock tuning for this setting
        #pragma config SOSCSEL  = LOW       // SOSC circuit configured for crystal driver mode
        #pragma config CLKOEN   = OFF       // Disable clock output on RA6
        #pragma config IESO     = OFF      	// Internal External (clock) Switchover
        #pragma config PLLDIV   = NODIV     // 4 MHz input (from 8MHz FRC / 2) provided to PLL circuit
        #pragma config POSCMD   = NONE      // Primary osc disabled, using FRC
        #pragma config FSCM     = CSECMD    // Clock switching enabled, fail safe clock monitor disabled
        #pragma config WPDIS    = WPDIS     // Program memory not write protected
        #pragma config WPCFG    = WPCFGDIS  // Config word page of program memory not write protected
        #pragma config IOL1WAY  = OFF       // IOLOCK can be set/cleared as needed with unlock sequence
        #pragma config LS48MHZ  = SYSX2     // Low Speed USB clock divider
        #pragma config WDTCLK   = LPRC      // WDT always uses INTOSC/LPRC oscillator
        #pragma config WDTEN    = ON        // WDT disabled; SWDTEN can control WDT
        #pragma config WINDIS   = WDTSTD    // Normal non-window mode WDT.
        #pragma config VBTBOR   = OFF       // VBAT BOR disabled

//If using the YOUR_BOARD hardware platform (see usbcfg.h), uncomment below and add pragmas
//#elif defined(YOUR_BOARD)
		//Add the configuration pragmas here for your hardware platform
		//#pragma config ... 		= ...

#else
	#error Not a supported board.  If using a different microcontroller family, see one of other workspaces.  Otherwise, make sure the proper board is selected in usbcfg.h, and if so, set configuration bits in __FILE__, line __LINE__
#endif

/** V A R I A B L E S ********************************************************/
#pragma udata
word led_count;
unsigned int pll_startup_counter;	//Used for software delay while pll is starting up


/** P R I V A T E  P R O T O T Y P E S ***************************************/
void Main(void);
void _entry (void);
static void InitializeSystem(void);
void USBTasks(void);
void BlinkUSBStatus(void);

//externs
extern void LongDelay(void);



//--------------------------------------------------------
//Entry point code and vector remapping
//--------------------------------------------------------
#pragma code
//Be careful if modifying the below code.  The below code is absolute address sensitive, since it contains
//the reset and high/low priority interrupt vectors, as well as the bootloader entry point "BootAppStart:" at address 0x001C.
//note: On PIC18, "goto" instructions take 4 byte of program memory, while most other instructions take only 2 bytes each
#pragma code _entry_scn=0x000000		//Reset vector is at 0x00.  Device begins executing code from 0x00 after a reset or POR event
void
_entry (void)
{
	_asm
	bsf		mInitSwitch2		//Address: 0x00		//Configure sw2 as input pin (not strictly necessary since reset state is already = 1)
	nop							//Address: 0x02		//Filler to waste 2 bytes of program memory
	bra		BootEntryIOCheck	//Address: 0x04		//bra BootEntryIOCheck skips past the high-priority interrupt redirect
	nop							//Address: 0x06		//Filler to waste 2 bytes of program memory

	//HIGH PRIORITY INTRRUPT VECTOR at address 0x08
    goto 0x1008					//Address: 0x08		//If a high priority interrupt occurs, PC first goes to 0x0008, then executes "goto 0x1008" redirect

BootEntryIOCheck:
	//Perform an I/O pin check to see if we should enter either the main application firmware, or this bootloader firmware.
	//io_cfg.h defines sw2 I/O pin for this purpose.  If sw2 is pressed (sw2 = 0), enter bootloader firmware.
	//If not pressed (sw2 = 1, due to pull up resistor), enter the main application firmware instead.
    btfss	sw2        			//Address: 0x0C		//Check sw2 I/O pin state	
    bra		BootAppStart		//Address: 0x0E		//If pushbutton pressed, enter bootloader	

	//Otherwise, need to enter the main application firmware.
	nop							//Address: 0x10		//Filler to waste 2 bytes of program memory
	nop							//Address: 0x12		//Filler to waste 2 bytes of program memory
    goto	0x1000				//Address: 0x14		//Goto the main application firmware, user was not pressing the pushbutton to enter bootloader
    
	//LOW PRIORITY INTERRUPT VECTOR at address 0x18    
    goto	0x1018				//Address: 0x18		//If a low-priority interrupt occurs, PC first goes to 0x0018, then executes "goto 0x1018" redirect
	_endasm

BootAppStart:					//Address: 0x1C		//If executing the main application firmware, and user wishes to enter the bootloader
    												//simply execute an "_asm goto 0x001C _endasm" instruction.  This will go to this BootAppStart section,
       												//which in turn will enter the bootloader firmware.
	STKPTR = 0x00;				//Reset the stack pointer = 0x00, in case the calling code left a bunch of entries on the return stack.
	Main();
}	



/** D E C L A R A T I O N S **************************************************/
#pragma code
/******************************************************************************
 * Function:        void Main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *****************************************************************************/
void Main(void)
{   
	//NOTE: The c018.o file is not included in the linker script for this project.
	//The C initialization code in the c018.c (comes with C18 compiler in the src directory)
	//file is instead modified and included here manually.  This is done so as to provide
	//a more convenient entry method into the bootloader firmware.  Ordinarily the _entry_scn
	//program code section starts at 0x00 and is created by the code of c018.o.  However,
	//the linker will not work if there is more than one section of code trying to occupy 0x00.
	//Therefore, must not use the c018.o code, must instead manually include the useful code
	//here instead.

    // Initialize the C stack pointer, and other compiler managed items as normally done in the c018.c file.
	_asm
    lfsr 1, _stack
    lfsr 2, _stack
    clrf TBLPTRU, 0 
	_endasm
	// End of the important parts of the C initializer.  This bootloader firmware does not use
	// any C initialized user variables (idata memory sections).  Therefore, the above is all
	// the initialization that is required.

    InitializeSystem();		//Some USB, I/O pins, and other initialization

    while(1)
    {
		ClrWdt();		
	    USBTasks();		        // Need to call USBTasks() periodically
	    						// it handles SETUP packets needed for enumeration

		BlinkUSBStatus();		//Blink the LEDs based on current USB state
		
	    if((usb_device_state == CONFIGURED_STATE) && (UCONbits.SUSPND != 1))
	    {
 	       ProcessIO();   // This is where all the actual bootloader related data transfer/self programming takes place
 	    }				  // see ProcessIO() function in the Boot87J50Family.c file.
    }//end while    
    
}//end main

/******************************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization routine.
 *                  All required USB initialization routines are called from
 *                  here.
 *
 *                  User application initialization routine should also be
 *                  called from here.                  
 *
 * Note:            None
 *****************************************************************************/
static void InitializeSystem(void)
{
    #if defined(PIC18F97J94_FS_USB_PIM) || defined(PIC18F87J94_FS_USB_PIM)			

        //Make sure to select oscillator settings consistent with USB operation.
        //If the user application firmware entered the bootloader through the absolute
        //entry point, it is possible the clock source may not have already been compatible
        //with USB operation.  In this case we need to switch as appropriate.
        OSCCON2bits.CLKLOCK = 0;    //Deassert clock setting lock
        OSCCON3 = 0x01;             //FRC/2 setting (4MHz)
        OSCCON4 = 0x00;             //1:1 
        OSCCON = 0x01;              //FRC+PLL selected
       
        //Enable INTOSC active clock tuning if full speed
        ACTCON = 0x90; //Enable active clock self tuning for USB operation
        while(OSCCON2bits.LOCK == 0)      //Make sure PLL is locked/frequency is compatible
        {
            ClrWdt();
        } 

    #else

        #error Double Click this message.  Please make sure the InitializeSystem() function correctly configures your hardware platform.  
		//Also make sure the correct board is selected in usbcfg.h.  If 
		//everything is correct, comment out the above "#error ..." line
		//to suppress the error message.

    #endif

	//USB module may have already been on if the application firmware calls the bootloader
	//without first disabling the USB module.  If this happens, need
	//to temporarily soft-detach from the host, wait a delay (allows cable capacitance
	//to discharge, and to allow host software to recognize detach), then
	//re-enable the USB module, so the host knows to re-enumerate the
	//USB device.
	if(UCONbits.USBEN == 1)
	{
		UCONbits.SUSPND = 0;
		UCON = 0;
		LongDelay();
	}	


//	The USB specifications require that USB peripheral devices must never source
//	current onto the Vbus pin.  Additionally, USB peripherals should not source
//	current on D+ or D- when the host/hub is not actively powering the Vbus line.
//	When designing a self powered (as opposed to bus powered) USB peripheral
//	device, the firmware should make sure not to turn on the USB module and D+
//	or D- pull up resistor unless Vbus is actively powered.  Therefore, the
//	firmware needs some means to detect when Vbus is being powered by the host.
//	A 5V tolerant I/O pin can be connected to Vbus (through a resistor), and
// 	can be used to detect when Vbus is high (host actively powering), or low
//	(host is shut down or otherwise not supplying power).  The USB firmware
// 	can then periodically poll this I/O pin to know when it is okay to turn on
//	the USB module/D+/D- pull up resistor.  When designing a purely bus powered
//	peripheral device, it is not possible to source current on D+ or D- when the
//	host is not actively providing power on Vbus. Therefore, implementing this
//	bus sense feature is optional.  This firmware can be made to use this bus
//	sense feature by making sure "USE_USB_BUS_SENSE_IO" has been defined in the
//	usbcfg.h file.    
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See io_cfg.h
    #endif

//	If the host PC sends a GetStatus (device) request, the firmware must respond
//	and let the host know if the USB peripheral device is currently bus powered
//	or self powered.  See chapter 9 in the official USB specifications for details
//	regarding this request.  If the peripheral device is capable of being both
//	self and bus powered, it should not return a hard coded value for this request.
//	Instead, firmware should check if it is currently self or bus powered, and
//	respond accordingly.  If the hardware has been configured like demonstrated
//	on the PICDEM FS USB Demo Board, an I/O pin can be polled to determine the
//	currently selected power source.  On the PICDEM FS USB Demo Board, "RA2" 
//	is used for	this purpose.  If using this feature, make sure "USE_SELF_POWER_SENSE_IO"
//	has been defined in usbcfg.h, and that an appropriate I/O pin has been mapped
//	to it in io_cfg.h.    
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;
    #endif
    
    mInitializeUSBDriver();         // See usbdrv.h
    
    UserInit();                     // See user.c & .h

	led_count = 0;			//Initialize variable used to toggle LEDs
    mInitAllLEDs();			//Init them off.

	//Turn off digital input buffers on analog pins to minimize power consumption
	//if the I/O pins happen to be floating in the target application.
	ANCON1 = 0xFF;			//All analog, digital input buffers off
	ANCON2 = 0xFF;			//All analog, digital input buffers off
	ANCON3 = 0xFF;			//All analog, digital input buffers off

}//end InitializeSystem

/******************************************************************************
 * Function:        void USBTasks(void)
 *
 * PreCondition:    InitializeSystem has been called.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Service loop for USB tasks.
 *
 * Note:            None
 *****************************************************************************/
void USBTasks(void)
{
    /*
     * Servicing Hardware
     */
    USBCheckBusStatus();                    // Must use polling method
    USBDriverService();              	    // Interrupt or polling method

}// end USBTasks


/******************************************************************************
 * Function:        void BlinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BlinkUSBStatus turns on and off LEDs corresponding to
 *                  the USB device state.
 *
 * Note:            mLED macros can be found in io_cfg.h
 *                  usb_device_state is declared in usbmmap.c and is modified
 *                  in usbdrv.c, usbctrltrf.c, and usb9.c
 *****************************************************************************/
void BlinkUSBStatus(void)
{
    if(led_count == 0)led_count = 10000U;
    led_count--;

    #define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
    #define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
    #define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
    #define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}

	 if(usb_device_state < CONFIGURED_STATE)
		mLED_Only_1_On();
	 if(usb_device_state == CONFIGURED_STATE)
        {
            if(led_count==0)
            {
                mLED_1_Toggle();
                mLED_2 = !mLED_1;       // Alternate blink
            }//end if
        }//end if(...)

}//end BlinkUSBStatus



//Placeholder code at address 0x1000 (the start of the non-bootloader firmware space)
//This gets overwritten when a real hex file gets programmed by the bootloader.
//If however no hex file has been programmed, might as well stay in the bootloader
//firmware, even if the RB4 pushbutton was not pressed after coming out of reset.
#pragma code user_app_vector=0x1000	
void userApp(void)
{
	_asm goto 0x001C _endasm 	//Goes to the "BootAppStart:" section which will enter the bootloader firmware				
}

/** EOF main.c ***************************************************************/
