/*********************************************************************
 *
 *   HID Device Bootloader Firmware for PIC18F97J94 Family Devices
 *
 *********************************************************************
 * FileName:        Boot18FxxJxxFamily.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 3.43+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") for its PICmicro® Microcontroller is intended and
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Fritz Schlunder		04/12/08	Original.
 * Fritz Schlunder		02/17/09	Slight modifications for 
 *									MCHPFSUSB v2.4 release. 
 * Fritz Schlunder		06/27/12	Modifed for PIC18F97J94 Family devices
 * Brian Tompson        09/27/12    Adjusted flash erase block size,
 *                                  erase logic, config byte size and 
 *                                  starting location for PIC18F97J94 ONLY
 ********************************************************************/

/** C O N S T A N T S **********************************************************/

//Section defining the address range to erase for the erase device command, along with the valid programming range to be reported by the QUERY_DEVICE command.
#define StartPageToErase                8           //The 512 byte page starting at address 0x1000 will be erased.
#define ProgramMemStart					0x001000    //Beginning of application program memory (not occupied by bootloader).  **THIS VALUE MUST BE ALIGNED WITH 64 BYTE BLOCK BOUNDRY** Also, in order to work correctly, make sure the StartPageToErase is set to erase this section.
#define ConfigWordsSectionLength		0x10        //16 bytes worth of Configuration words on the PIC18F97J94 Family devices

#if defined(__18F97J94)||defined(__18F87J94)||defined(__18F67J94)       //128kB flash devices
    // 512 byte flash erase block size
	#define MaxPageToEraseNoConfigs		254		 //Last page of flash on the device, which does not contain the flash configuration words.
	#define MaxPageToEraseWithConfigs	255		 //Page 255 contains the flash configurations words on this device.
	#define ProgramMemStopNoConfigs		0x01FE00    //**MUST BE WORD ALIGNED (EVEN) ADDRESS.  
                                                    // This address does not get updated, 
                                                    // but the one just below it does: 
                                                    // IE: If ProgramMemStopNoConfig = 0x200, 
                                                    //      0x1FF is the last programmed 
                                                    //      address (0x200 not programmed)**	
	#define ProgramMemStopWithConfigs	0x01FFF0 //**MUST BE WORD ALIGNED (EVEN) ADDRESS.  This address does not get updated, but the one just below it does: IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)**	
	#define ConfigWordsStartAddress		0x01FFF0 //0xXXXF0 is CONFIG1L on this device
#elif defined(__18F96J99)||defined(__18F86J99)||defined(__18F66J99)     //96kB flash devices
	#define MaxPageToEraseNoConfigs		94		 //Last page of flash on the device, which does not contain the flash configuration words.
	#define MaxPageToEraseWithConfigs	95		 //Page 127 contains the flash configurations words on this device.
	#define ProgramMemStopNoConfigs		0x017C00 //**MUST BE WORD ALIGNED (EVEN) ADDRESS.  This address does not get updated, but the one just below it does: IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)**	
	#define ProgramMemStopWithConfigs	0x017FF8 //**MUST BE WORD ALIGNED (EVEN) ADDRESS.  This address does not get updated, but the one just below it does: IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)**	
	#define ConfigWordsStartAddress		0x017FF8 //0xXXXF8 is CONFIG1L on this device
#elif defined(__18F96J94)||defined(__18F86J94)||defined(__18F66J94)    //64kB flash devices
	#define MaxPageToEraseNoConfigs		62		 //Last page of flash on the device, which does not contain the flash configuration words.
	#define MaxPageToEraseWithConfigs	63		 //Page 127 contains the flash configurations words on this device.
	#define ProgramMemStopNoConfigs		0x00FC00 //**MUST BE WORD ALIGNED (EVEN) ADDRESS.  This address does not get updated, but the one just below it does: IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)**	
	#define ProgramMemStopWithConfigs	0x00FFF8 //**MUST BE WORD ALIGNED (EVEN) ADDRESS.  This address does not get updated, but the one just below it does: IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)**	
	#define ConfigWordsStartAddress		0x00FFF8 //0xXXXF8 is CONFIG1L on this device
#elif defined(__18F95J94)||defined(__18F85J94)||defined(__18F65J94)     //32kB flash devices
	#define MaxPageToEraseNoConfigs		30		 //Last page of flash on the device, which does not contain the flash configuration words.
	#define MaxPageToEraseWithConfigs	31		 //Page 127 contains the flash configurations words on this device.
	#define ProgramMemStopNoConfigs		0x007C00 //**MUST BE WORD ALIGNED (EVEN) ADDRESS.  This address does not get updated, but the one just below it does: IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)**	
	#define ProgramMemStopWithConfigs	0x007FF8 //**MUST BE WORD ALIGNED (EVEN) ADDRESS.  This address does not get updated, but the one just below it does: IE: If AddressToStopPopulating = 0x200, 0x1FF is the last programmed address (0x200 not programmed)**	
	#define ConfigWordsStartAddress		0x007FF8 //0xXXXF8 is CONFIG1L on this device
#endif

//Switch State Variable Choices
#define	QUERY_DEVICE				0x02	//Command that the host uses to learn about the device (what regions can be programmed, and what type of memory is the region)
#define	UNLOCK_CONFIG				0x03	//Note, this command is used for both locking and unlocking the config bits (see the "//Unlock Configs Command Definitions" below)
#define ERASE_DEVICE				0x04	//Host sends this command to start an erase operation.  Firmware controls which pages should be erased.
#define PROGRAM_DEVICE				0x05	//If host is going to send a full RequestDataBlockSize to be programmed, it uses this command.
#define	PROGRAM_COMPLETE			0x06	//If host send less than a RequestDataBlockSize to be programmed, or if it wished to program whatever was left in the buffer, it uses this command.
#define GET_DATA					0x07	//The host sends this command in order to read out memory from the device.  Used during verify (and read/export hex operations)
#define	RESET_DEVICE				0x08	//Resets the microcontroller, so it can update the config bits (if they were programmed, and so as to leave the bootloader (and potentially go back into the main application)

//Unlock Configs Command Definitions
#define UNLOCKCONFIG				0x00	//Sub-command for the ERASE_DEVICE command
#define LOCKCONFIG					0x01	//Sub-command for the ERASE_DEVICE command

//Query Device Response "Types" 
#define	TypeProgramMemory			0x01	//When the host sends a QUERY_DEVICE command, need to respond by populating a list of valid memory regions that exist in the device (and should be programmed)
#define TypeEEPROM					0x02
#define TypeConfigWords				0x03
#define	TypeEndOfTypeList			0xFF	//Sort of serves as a "null terminator" like number, which denotes the end of the memory region list has been reached.


//BootState Variable States
#define	IDLE						0x00
#define NOT_IDLE    				0x01

//OtherConstants
#define INVALID_ADDRESS				0xFFFFFFFF
#define CORRECT_UNLOCK_KEY          0xB5

//Application and Microcontroller constants
#define BytesPerAddressPIC18		0x01		//One byte per address.  PIC24 uses 2 bytes for each address in the hex file.

#define	TotalPacketSize				0x40
#define WORDSIZE					0x02	//PIC18 uses 2 byte words, PIC24 uses 3 byte words.
#define	FlashBlockSize				0x40	//For PIC18F97J94 family devices, a flash block is 64 bytes
#define RequestDataBlockSize 		0x3A	//Number of data bytes in a standard request to the PC.  Must be an even number from 2-58 (0x02-0x3A).  Larger numbers make better use of USB bandwidth and 
											//yeild shorter program/verify times, but require more micrcontroller RAM for buffer space.
#define BufferSize 					0x40

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "typedefs.h"
#include "usb.h"
#include "io_cfg.h"             // I/O pin mapping


typedef union 
{
		unsigned char Contents[64];

		//General command (with data in it) packet structure used by PROGRAM_DEVICE and GET_DATA commands 		
		struct{
			unsigned char Command;
			unsigned long Address;
			unsigned char Size;
//			unsigned char PadBytes[58-RequestDataBlockSize];	//Uncomment this if using a smaller than 0x3A RequestDataBlockSize.  Compiler doesn't like 0 byte array when using 58 byte data block size.
			unsigned char Data[RequestDataBlockSize];
		};
		
		//This struct used for responding to QUERY_DEVICE command (on a device with four programmable sections)
		struct{
			unsigned char Command;
			unsigned char PacketDataFieldSize;
			unsigned char BytesPerAddress;
			unsigned char Type1;
			unsigned long Address1;
			unsigned long Length1;
			unsigned char Type2;
			unsigned long Address2;
			unsigned long Length2;
			unsigned char EndOfTypes;
			unsigned char ExtraPadBytes[42];
		};
		
		struct{						//For UNLOCK_CONFIG command
			unsigned char Command;
			unsigned char LockValue;
		};
} PacketToFromPC;		
	

/** V A R I A B L E S ********************************************************/
#pragma udata 
unsigned char MaxPageToErase;
unsigned short long ProgramMemStopAddress;
PacketToFromPC PacketFromPC;
PacketToFromPC PacketToPC;
unsigned char BootState;
unsigned int ErasePageTracker;
unsigned char ProgrammingBuffer[BufferSize];
unsigned char BufferedDataIndex;
unsigned short long ProgrammedPointer;


/***************** P R O T O T Y P E S ***************************************/
void BlinkUSBStatus(void);
void UserInit(void);
void EraseFlash(void);
void WriteFlashSubBlock(void);
void LongDelay(void);
void UnlockAndActivate(unsigned char UnlockKey);
void ResetDeviceCleanly(void);
void ClearWatchdog(void);
void TableReadPostIncrement(void);


/** D E C L A R A T I O N S **************************************************/
#pragma code
void UserInit(void)
{
	//Initialize bootloader state variables
	MaxPageToErase = MaxPageToEraseNoConfigs;		//Assume we will not allow erase/programming of config words (unless host sends override command)
	ProgramMemStopAddress = ProgramMemStopNoConfigs;
	BootState = IDLE;
	ProgrammedPointer = INVALID_ADDRESS;	
	BufferedDataIndex = 0;
}//end UserInit


/******************************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user routines.
 *                  It is a mixture of both USB and non-USB tasks.
 *
 * Note:            None
 *****************************************************************************/
void ProcessIO(void)
{
	static unsigned char i;

	if(BootState == IDLE)
	{
		if(!mHIDRxIsBusy())	//Did we receive a command?
		{
    		//Copy the newly received USB packet into our local buffer, and
    		//rearm the endpoint so it can receive the next packet.
			HIDRxReport((char *)&PacketFromPC, 64);
			BootState = NOT_IDLE;
			
			for(i = 0; i < TotalPacketSize; i++)		//Prepare the next packet we will send to the host, by initializing the entire packet to 0x00.
				PacketToPC.Contents[i] = 0;				//This saves code space, since we don't have to do it independently in the QUERY_DEVICE and GET_DATA cases.
		}
	}
	else //(BootState must be NOT_IDLE)
	{	
    	//Check the latest command we received from the PC app, to determine what
    	//we should be doing.
		switch(PacketFromPC.Command)
		{
			case QUERY_DEVICE:
			{
				//Prepare a response packet, which lets the PC software know about the memory ranges of this device.
				PacketToPC.Command = QUERY_DEVICE;
				PacketToPC.PacketDataFieldSize = RequestDataBlockSize;
				PacketToPC.BytesPerAddress = BytesPerAddressPIC18;
				PacketToPC.Type1 = TypeProgramMemory;
				PacketToPC.Address1 = (unsigned long)ProgramMemStart;
				PacketToPC.Length1 = (unsigned long)(ProgramMemStopAddress - ProgramMemStart);	//Size of program memory area
				PacketToPC.Type2 = TypeConfigWords;
				PacketToPC.Address2 = (unsigned long)ConfigWordsStartAddress;
				PacketToPC.Length2 = (unsigned long)ConfigWordsSectionLength;
				PacketToPC.EndOfTypes = TypeEndOfTypeList;
				//Init pad bytes to 0x00...  Already done after we received the QUERY_DEVICE command (just after calling HIDRxReport()).
	
				if(!mHIDTxIsBusy())
				{
					HIDTxReport((char *)&PacketToPC, 64);
					BootState = IDLE;
				}
			}
				break;
			case UNLOCK_CONFIG:
			{
				if(PacketFromPC.LockValue == UNLOCKCONFIG)
				{
					MaxPageToErase = MaxPageToEraseWithConfigs;		//Assume we will not allow erase/programming of config words (unless host sends override command)
					ProgramMemStopAddress = ProgramMemStopWithConfigs;
				}
				else	//LockValue must be == LOCKCONFIG
				{
					MaxPageToErase = MaxPageToEraseNoConfigs;		
					ProgramMemStopAddress = ProgramMemStopNoConfigs;
				}
				BootState = IDLE;
			}
				break;
			case ERASE_DEVICE:
			{
				for(ErasePageTracker = StartPageToErase; ErasePageTracker < (MaxPageToErase + (unsigned int)1); ErasePageTracker++)
				{
					ClearWatchdog();
					EraseFlash();
					USBDriverService(); 	//Call USBDriverService() periodically to prevent falling off the bus if any SETUP packets should happen to arrive.
				}
				BootState = IDLE;				
			}
				break;
			case PROGRAM_DEVICE:
			{
				if(ProgrammedPointer == (unsigned short long)INVALID_ADDRESS)
					ProgrammedPointer = PacketFromPC.Address;
				
				if(ProgrammedPointer == (unsigned short long)PacketFromPC.Address)
				{
					for(i = 0; i < PacketFromPC.Size; i++)
					{
						ProgrammingBuffer[BufferedDataIndex] = PacketFromPC.Data[i+(RequestDataBlockSize-PacketFromPC.Size)];	//Data field is right justified.  Need to put it in the buffer left justified.
						BufferedDataIndex++;
						ProgrammedPointer++;
						if(BufferedDataIndex == RequestDataBlockSize)
						{
							WriteFlashSubBlock();
						}
					}
				}
				//else host sent us a non-contiguous packet address...  to make this firmware simpler, host should not do this without sending a PROGRAM_COMPLETE command in between program sections.
				BootState = IDLE;
			}
				break;
			case PROGRAM_COMPLETE:
			{
				WriteFlashSubBlock();
				ProgrammedPointer = INVALID_ADDRESS;		//Reinitialize pointer to an invalid range, so we know the next PROGRAM_DEVICE will be the start address of a contiguous section.
				BootState = IDLE;
			}
				break;
			case GET_DATA:
			{
				//Init pad bytes to 0x00...  Already done after we received the 
				//QUERY_DEVICE command (just after calling HIDRxReport()).
				PacketToPC.Command = GET_DATA;
				PacketToPC.Address = PacketFromPC.Address;
				PacketToPC.Size = PacketFromPC.Size;

				TBLPTR = (unsigned short long)PacketFromPC.Address;
				for(i = 0; i < PacketFromPC.Size; i++)
				{
                    TableReadPostIncrement();

					PacketToPC.Data[i+((TotalPacketSize - 6) - PacketFromPC.Size)] = TABLAT;					
				}

				if(!mHIDTxIsBusy())
				{
					HIDTxReport((char *)&PacketToPC, 64);
					BootState = IDLE;
				}
				
			}
				break;
			case RESET_DEVICE:
			    ResetDeviceCleanly();
		}//End switch
	}//End if/else

}//End ProcessIO()


//Before resetting the microcontroller, we should shut down the USB module 
//gracefully, to make sure the host correctly recognizes that we detached
//from the bus.
void ResetDeviceCleanly(void)
{
	UCONbits.SUSPND = 0;		
	UCON = 0x00;				//Disable USB module
	//And wait awhile for the USB cable capacitance to discharge down to disconnected (SE0) state. 
	//Otherwise host might not realize we disconnected/reconnected when we do the reset.
	//A basic for() loop decrementing a 16 bit number would be simpler, but seems to take more code space for
	//a given delay.  So do this instead:
    LongDelay();

	Reset();    
} 


void EraseFlash(void)
{
    //Really want this: TBLPTR = ((unsigned short long)ErasePageTracker <<  9);  // for  512 byte erase blocks
    //However compiler is not very efficient at this, so we do this instead:
	TBLPTRL = 0x00;
	TBLPTRH = ErasePageTracker;
	TBLPTRU = 0x00;

	_asm
	bcf		STATUS, 0, 0
	rlcf	TBLPTRH, 1, 0
	rlcf	TBLPTRU, 1, 0
	_endasm 

	EECON1 = 0b00010100;                    //Prepare to erase a flash page
    UnlockAndActivate(CORRECT_UNLOCK_KEY);  //Now make the operation take place
	ClrWdt();                               //Erase operations take awhile, refresh the watchdog

}	


void WriteFlashSubBlock(void)		//Use word writes to write code chunks less than a full 64 byte block size.
{
	unsigned char i = 0;

	while(BufferedDataIndex > 0)		//While data is still in the buffer.
	{
		TBLPTR = (ProgrammedPointer - BufferedDataIndex);
		//Below section will need to be modified if the WORDSIZE of your processor is not 2 bytes.
		TABLAT = ProgrammingBuffer[i];
		_asm tblwtpostinc _endasm
		i++;		
		TABLAT = ProgrammingBuffer[i];
		_asm tblwt _endasm				//Do not increment TBLPTR on the second write.  See datasheet.
		i++;
		
		EECON1 = 0b00100100;	                //Word programming mode
		UnlockAndActivate(CORRECT_UNLOCK_KEY);  //Now make the operation take place
	
		BufferedDataIndex = BufferedDataIndex - WORDSIZE;	//Used up one word from the buffer.
		ClrWdt();
	}
}


//A long delay function, on the order of roughly 100ms at 64MHz.
void LongDelay(void)
{
	unsigned char i;
	//A basic for() loop decrementing a 16 bit number would be simpler, but seems to take more code space for
	//a given delay.  So do this instead:	
	for(i = 0; i < 0xFF; i++)
	{
		WREG = 0xFF;
		while(WREG)
		{
			WREG--;
			_asm
			bra	0	//Equivalent to bra $+2, which takes half as much code as 2 nop instructions
			bra	0	//Equivalent to bra $+2, which takes half as much code as 2 nop instructions
			clrwdt
			bra	0	//Equivalent to bra $+2, which takes half as much code as 2 nop instructions
			_endasm	
		}
	}
}	


//It is preferrable to only place this sequence in only one place in the flash memory.
//This reduces the probabilty of the code getting executed inadvertently by
//errant code.
void UnlockAndActivate(unsigned char UnlockKey)
{
	INTCONbits.GIE = 0;		//Make certain interrupts disabled for unlock process.

    //Check to make sure the caller really was trying to call this function.
    //If they were, they should always pass us the CORRECT_UNLOCK_KEY.
    if(UnlockKey != CORRECT_UNLOCK_KEY)
    {
        //Warning!  Errant code execution detected.  Somehow this 
        //UnlockAndActivate() function got called by someone that wasn't trying
        //to actually perform an NVM erase or write.  This could happen due to
        //microcontroller overclocking (or undervolting for an otherwise allowed
        //CPU frequency), or due to buggy code (ex: incorrect use of function 
        //pointers, etc.).  In either case, we should execute some fail safe 
        //code here to prevent corruption of the NVM contents.
        OSCCON = 0x05;  //Switch to LPRC at low frequency
        while(1)
        {
            Sleep();
        }    
        Reset();
    }    
    
	_asm
	//Now unlock sequence to set WR (make sure interrupts are disabled before executing this)
	MOVLW 0x55
	MOVWF EECON2, 0
	MOVLW 0xAA
	MOVWF EECON2, 0
	BSF EECON1, 1, 0		//Performs write
	_endasm	
	while(EECON1bits.WR);	//Wait until complete (relevant when programming EEPROM, not important when programming flash since processor stalls during flash program)	
	EECON1bits.WREN = 0;  	//Good practice now to clear the WREN bit, as further protection against any accidental activation of self write/erase operations.
}	


//Note: The ClrWdt() and "_asm tblrdpostinc _endasm" are inline assembly language
//instructions.  The ClearWatchdog() and  TableReadPostIncrement() functions are 
//theoretically extraneous, since the operations being accomplished could be
//done without calling them as separate functions.  However, when using inline
//assembly language, the C compiler normally doesn't know what the code will
//actually do (ex: will it modify STATUS reg, WREG, BSR contents??).  As a 
//result, it is potentially dangerous for the C compiler to make assumptions,
//that might turn out not to be correct.  Therefore, the C18 compiler disables
//the compiler optimizations for a function, when one or more inline asm 
//instructions are located within the C function.  Therefore, to promote best
//code size optimizations from the C compiler, it is best to locate inline
//assembly sequences in their own separate C functions, that do not contain much
//other code (which could otherwise be optimized by the C compiler).  This often
//results in the smallest code size, and is the reason it is being done here.
void ClearWatchdog(void)
{
    ClrWdt();
}    
void TableReadPostIncrement(void)
{
	_asm tblrdpostinc _endasm    
}    

/** EOF Boot18FxxJxxFamily.c *********************************************************/
