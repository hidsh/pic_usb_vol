/************************************************************************
* Copyright (c) 2009-2010,  Microchip Technology Inc.
*
* Microchip licenses this software to you solely for use with Microchip
* products.  The software is owned by Microchip and its licensors, and
* is protected under applicable copyright laws.  All rights reserved.
*
* SOFTWARE IS PROVIDED "AS IS."  MICROCHIP EXPRESSLY DISCLAIMS ANY
* WARRANTY OF ANY KIND, WHETHER EXPRESS OR IMPLIED, INCLUDING BUT
* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  IN NO EVENT SHALL
* MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
* CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, HARM TO YOUR
* EQUIPMENT, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY
* OR SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED
* TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION,
* OR OTHER SIMILAR COSTS.
*
* To the fullest extent allowed by law, Microchip and its licensors
* liability shall not exceed the amount of fees, if any, that you
* have paid directly to Microchip to use this software.
*
* MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE
* OF THESE TERMS.
*
* Author        Date        Comment
*************************************************************************
* T. Lawrence  2011/01/24  Initial code ported from AN1310.
************************************************************************/

#ifndef COMM_H
#define COMM_H

#include <QThread>
#include <QTimer>

#include "../HIDAPI/hidapi.h"
#include "Device.h"

// Device Vendor and Product IDs
#define VID 0x04d8
#define PID 0x003C

// Packet commands
#define QUERY_DEVICE        0x02
#define UNLOCK_CONFIG       0x03
#define ERASE_DEVICE        0x04
#define PROGRAM_DEVICE      0x05
#define PROGRAM_COMPLETE    0x06
#define GET_DATA            0x07
#define RESET_DEVICE        0x08

// Maximum number of memory regions that can be bootloaded
#define MAX_DATA_REGIONS    0x06

/*!
 * Provides low level HID bootloader communication.
 */
class Comm : public QObject
{
    Q_OBJECT

signals:
    void SetProgressBar(int newValue);


protected:
    hid_device *boot_device;
    bool connected;

public:

    explicit Comm();
    ~Comm();

    static const int SyncWaitTime;

    enum ErrorCode
    {
        Success = 0, NotConnected, Fail, IncorrectCommand, Timeout, Other = 0xFF
    };

    QString ErrorString(ErrorCode errorCode) const;

    #pragma pack(1)
    struct MemoryRegion
    {
        unsigned char type;
        unsigned long int address;
        unsigned long int size;
    };

    struct BootInfo
    {
        unsigned char command;
        unsigned char bytesPerPacket;
        unsigned char deviceFamily;
        MemoryRegion memoryRegions[MAX_DATA_REGIONS];
        unsigned char pad[8];
    };

    struct WritePacket
    {
        unsigned char report;
        unsigned char command;
        union {
            unsigned long int address;
            unsigned char LockedValue;
        };
        unsigned char bytesPerPacket;
        unsigned char data[58];
    };
    struct ReadPacket
    {
        unsigned char command;
        unsigned long int address;
        unsigned char bytesPerPacket;
        unsigned char data[59];
    };
    #pragma pack()

    void PollUSB(void);

    ErrorCode open(void);

    void close(void);
    bool isConnected(void);
    void Reset(void);

    ErrorCode GetData(unsigned long address, unsigned char bytesPerPacket, unsigned char bytesPerAddress,
                      unsigned char bytesPerWord, unsigned long endAddress, unsigned char *data);
    ErrorCode Program(unsigned long address, unsigned char bytesPerPacket, unsigned char bytesPerAddress,
                      unsigned char bytesPerWord, unsigned char deviceFamily, unsigned long endAddress, unsigned char *data);
    ErrorCode Erase(void);
    ErrorCode LockUnlockConfig(bool lock);
    ErrorCode ReadBootloaderInfo(BootInfo* bootInfo);
    ErrorCode SendPacket(unsigned char *data, int size);
    ErrorCode ReceivePacket(unsigned char *data, int size);
};

#endif // COMM_H
