/*----------------------------------------------------------------------+
| Texas Instruments | | Bootcode 
| +-----------------------------------------------------------------------+
| Source: bootcode.c, v 1.0 2000/01/26 16:45:55 
| 
| Author: Horng-Ming Lobo Tai lobotai@ti.com
|
| 
| 
| For more information, contact 
| Lobo Tai | 
| Texas Instruments |
| 12500 TI Blvd, MS 8761 | 
| Dallas, TX 75243 | 
| USA | 
| Tel 214-480-3145 |
| Fax 214-480-3443 |
| | 
| External EEPROM Format | 
| |
| Offset Type Size Value & Remark | 
| 0 Signature0 1 0x52, FUNCTION_PID_L | 
| 1 Signature1 1 0x51, FUNCTION_PID_H | 
| 2 Data Type 1 0x00 = End | 
| 0x01 = USB Info Basic | 
| 0x02 = Application Code | 
| 0x03..0xEF Reserved | 
| 0xff = Reserved for Extended Data | 
| 3 Data Size 2 Size of Data | 
| 9 for TUSB5052 & TUSB2136 Usb Info| 
| 5 Check Sum 1 Check Sum of Data Section | 
| 6 Bit Setting 1 Bit 0: Bus/self power in bUSBCRL | 
| Bit 7: PWRSW | 
| 7 VID 2 Vendor ID | 
| 9 PID hub 2 Product ID for hub | 
| 11 PID device 2 Product ID for bootrom |
| 13 HUBPOTG 1 Time from power-on to power-good |
| 14 HUBCURT 1 HUB Current descriptor register | 
| | 
| The following examples is for application code | 
| 15 Data Type 1 0x00 = End | 
| 0x02 = Application Code | 
| 16 Data Size 2 Size of Data Section | 
| 18 Check Sum 1 Check Sum of Data Section |
| 19 App. Rev. 2 Application Code Revision | 
| | 
| 21 Application Code Starts here... | 
| Logs: | | WHO WHEN WHAT | 
| HMT 20000126 born | 
| HMT 20000301 add PWRSW and HUBPOTG |
| HMT 20000331 Set address request (check illigeal address) | | HMT 20000517 modified header and added bus power support |
| HMT 20000525 added HUBCURT |
| modified header |

+----------------------------------------------------------------------*/

#include <io51.h> // 8051 sfr definition
#include <stdio.h>
#include <stdlib.h>

#include "types.h" // Basic Type declarations
#include "usb.h" // USB-specific Data Structures
#include "i2c.h"
#include "tusb5052.h"
#include "delay.h"
#include "header.h"
#include "bootcode.h"

#ifdef SIMULATION

#include "gpio.h"

#endif

/*----------------------------------------------------------------------+
| Constant Definition |
+----------------------------------------------------------------------*/

// for double buffer pointer

#define X_BUFFER 0
#define Y_BUFFER 1

const BYTE code abromDeviceDescriptor[SIZEOF_DEVICE_DESCRIPTOR] = {
    SIZEOF_DEVICE_DESCRIPTOR, // Length of this descriptor (12h bytes)
    DESC_TYPE_DEVICE, // Type code of this descriptor (01h)
    0x10,0x01, // Release of USB spec (Rev 1.1)
    0xff, // Device?s base class code-vendor specific
    0, // Device?s sub class code
    0, // Device?s protocol type code
    EP0_MAX_PACKET_SIZE, // End point 0?s max packet size = 8
    HUB_VID_L,HUB_VID_H, // Vendor ID for device, TI=0x0451
    FUNCTION_PID_L,FUNCTION_PID_H,// Product ID for device, TUSB5052
    0x00,0x01, // Revision level of device, Rev=1.0
    0, // Index of manufacturer name string desc
    0, // Index of product name string desc
    0, // Index of serial number string desc
    1  // Number of configurations supported
};

const BYTE code abromConfigurationDescriptorGroup[SIZEOF_BOOTCODE_CONFIG_DESC_GROUP] =
{
    // Configuration Descriptor, size=0x09
    SIZEOF_CONFIG_DESCRIPTOR, // bLength
    DESC_TYPE_CONFIG, // bDescriptorType
    SIZEOF_BOOTCODE_CONFIG_DESC_GROUP, 0x00, // wTotalLength
    0x01, // bNumInterfaces
    0x01, // bConfigurationValue
    0x00, // iConfiguration
    0x80, // bmAttributes, bus-powered hub
    0x32, // Max. Power Consumption at 2mA unit
    // Interface Descriptor, size = 0x09
    SIZEOF_INTERFACE_DESCRIPTOR, // bLength
    DESC_TYPE_INTERFACE, // bDescriptorType
    0x00, // bInterfaceNumber
    0x00, // bAlternateSetting
    1, // bNumEndpoints
    0xFF, // bInterfaceClass - vendor-specific
    0, // bInterfaceSubClass, zero for hub
    0, // bInterfaceProtocol
    0x00, // iInterface
    // Endpoint Descriptor, size = 0x07 for OEP1
    SIZEOF_ENDPOINT_DESCRIPTOR, // bLength
    DESC_TYPE_ENDPOINT, // bDescriptorType
    0x01 , // bEndpointAddress; bit7=1 for IN, bits 3-0=1 for ep1
    EP_DESC_ATTR_TYPE_BULK, // bmAttributes, bulk transfer
    0x40, 0x00, // wMaxPacketSize, 64 bytes
    0x00 // bInterval
};

// Global Memory Map 
#pragma memory = idata

BYTE bEp0TxBytesRemaining; // For endpoint zero transmitter only
// Holds count of bytes remaining to be
// transmitted by endpoint 0. A value
// of 0 means that a 0-length data packet
// A value of 0xFF means that transfer // is complete.
BYTE bHostAskMoreDataThanAvailable;
// If the host ask more data then TUSB2136 has
// It sends one zero-length packet
// if the asked lenght is a multiple of
// max. size of endpoint 0
BYTE bConfiguredFlag; // Set to 1 when USB device has been
// configured, set to 0 when unconfigured
PBYTE pbEp0Buffer; // A pointer to end point 0
WORD wCurrentFirmwareAddress; // for firmware downloading
WORD wFirmwareLength;
BYTE bFirmwareChecksum;
BYTE bRAMChecksum; BOOL bExecuteFirmware; // flag set by USB request to run the firmware
BOOL bRAMChecksumCorrect;
BOOL bCurrentBuffer; BYTE abBootCodeStatus[4];
extern WORD wCurrentUploadPointer; // in header.c extern
BYTE bi2cDeviceAddress; // in header.c

#pragma memory = default

/*---------------------------------------------------------------------------+

| TUSB5052 Register Structure Definition |
+---------------------------------------------------------------------------*/
#pragma memory = dataseg(TUSB5052_SETUPPACKET_SEG) // 0xff00
tDEVICE_REQUEST tSetupPacket;
#pragma memory = default 
#pragma memory = dataseg(TUSB5052_EP0_EDB_SEG) // 0xff80
tEDB0 tEndPoint0DescriptorBlock;
#pragma memory = default 
#pragma memory = dataseg(TUSB5052_IEP_EDB_SEG) // 0xff48
tEDB tInputEndPointDescriptorBlock[1];
#pragma memory = default
#pragma memory = dataseg(TUSB5052_OEP_EDB_SEG) // 0xf940
tEDB tOutputEndPointDescriptorBlock[1];
#pragma memory = default 
#pragma memory = dataseg(TUSB5052_IEP0BUFFER_SEG) // 0xfef8
BYTE abIEP0Buffer[EP0_MAX_PACKET_SIZE];
#pragma memory = default 
#pragma memory = dataseg(TUSB5052_OEP0BUFFER_SEG) // 0xfef0
BYTE abOEP0Buffer[EP0_MAX_PACKET_SIZE];
#pragma memory = default
#pragma memory = dataseg(TUSB5052_DESC_SEG) // 0xfc00
BYTE abDeviceDescriptor[SIZEOF_DEVICE_DESCRIPTOR];
BYTE abConfigurationDescriptorGroup[SIZEOF_BOOTCODE_CONFIG_DESC_GROUP];
#pragma memory = default
#pragma memory = dataseg(TUSB5052_OEP1_X_BUFFER_SEG)
BYTE abXBufferAddress[EP_MAX_PACKET_SIZE];
#pragma memory = default
#pragma memory = dataseg(TUSB5052_OEP1_Y_BUFFER_SEG)
BYTE abYBufferAddress[EP_MAX_PACKET_SIZE];
#pragma memory = default
#pragma memory = dataseg(TUSB5052_EXTERNAL_RAM_SEG) // 0x0000
BYTE abDownloadFirmware[DOWNLOAD_FW_SIZE];
#pragma memory = default

/*---------------------------------------------------------------------------+
| Sub-routines go here... |
+---------------------------------------------------------------------------*/
//----------------------------------------------------------------------------

VOID FillEp0TxWithNextDataPacket(VOID)
{
    BYTE bPacketSize,bIndex;
    // First check if there are bytes remaining to be transferred
    if (bEp0TxBytesRemaining != 0xFF)
    {
        if (bEp0TxBytesRemaining > EP0_MAX_PACKET_SIZE)
        {
            // More bytes are remaining than are capable of fitting in one packet
            bPacketSize = EP0_MAX_PACKET_SIZE;
            bEp0TxBytesRemaining -= EP0_MAX_PACKET_SIZE;
            // there are more IN Stage
        }
        else if (bEp0TxBytesRemaining < EP0_MAX_PACKET_SIZE)
        {
            // The remaining data fits in one packet.
            // This case properly handles bEp0TxBytesRemaining == 0
            bPacketSize = bEp0TxBytesRemaining;
            bEp0TxBytesRemaining = 0xFF; // No more data need to be Txed
        }
        else //bEp0TxBytesRemaining == EP0_MAX_PACKET_SIZE
        {
            bPacketSize = EP0_MAX_PACKET_SIZE;
            if(bHostAskMoreDataThanAvailable == TRUE) bEp0TxBytesRemaining = 0;
            else bEp0TxBytesRemaining = 0xFF;
        }

        for (bIndex=0; bIndex<bPacketSize; bIndex++)
            abIEP0Buffer[bIndex] = *pbEp0Buffer++;
        tEndPoint0DescriptorBlock.bIEPBCNT = bPacketSize & EPBCT_BYTECNT_MASK;
    }
} //----------------------------------------------------------------------------

VOID TransmitBufferOnEp0(PBYTE pbBuffer)
{
    pbEp0Buffer = pbBuffer; // Limit wLength to FEh
    if (tSetupPacket.bLengthH != 0) {
        tSetupPacket.bLengthH = 0;
        tSetupPacket.bLengthL = 0xFE;
    }

    // Limit transfer size to wLength if needed
    // this prevent USB device sending ?more than require? data back to the host
    if (bEp0TxBytesRemaining > tSetupPacket.bLengthL)
        bEp0TxBytesRemaining = tSetupPacket.bLengthL;

    if(bEp0TxBytesRemaining < tSetupPacket.bLengthL)
        bHostAskMoreDataThanAvailable = TRUE;
    else
        bHostAskMoreDataThanAvailable = FALSE;

    FillEp0TxWithNextDataPacket();

}

//----------------------------------------------------------------------------
VOID TransmitNullResponseOnEp0 (VOID)
{
    pbEp0Buffer = NULL; // to indicate a partial packet
    bEp0TxBytesRemaining = 0; // or ACK during standard USB request
    FillEp0TxWithNextDataPacket();
}

//----------------------------------------------------------------------------
VOID StallEndPoint0(VOID)
{
    tEndPoint0DescriptorBlock.bIEPCNFG |= EPCNF_STALL;
    tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;
}

//----------------------------------------------------------------------------
VOID Endpoint0Control(VOID)
{
    BYTE bTemp;
    WORD wIndex;
    BYTE abReturnBuffer[3];
    BOOL InTransaction;

    // copy the MSB of bmRequestType to DIR bit of USBCTL
    if((tSetupPacket.bmRequestType & USB_REQ_TYPE_INPUT) != 0x00) {
        InTransaction = TRUE;
        bUSBCTL |= USBCTL_DIR;
    } else {
        InTransaction = FALSE;
        bUSBCTL &= ~USBCTL_DIR;
    }

    // Set setup bit in USB control register bUSBCTL |= USBCTL_SIR; // on bit 1
    // clear endpoint stall here
    // If hardware in setup stage(hardware clears stall) but firmware still
    // in data stage(stall at the last packet), sometimes hardware clears stall
    // but firmware later on stall again. This causes problem in the new transfer
    // while firmware still in the previous transfer.
    tEndPoint0DescriptorBlock.bIEPCNFG &= ~EPCNF_STALL;
    tEndPoint0DescriptorBlock.bOEPCNFG &= ~EPCNF_STALL;
    abReturnBuffer[0] = 0;
    abReturnBuffer[1] = 0;
    abReturnBuffer[2] = 0;

    switch(tSetupPacket.bmRequestType & USB_REQ_TYPE_MASK)  {
    case USB_REQ_TYPE_STANDARD:
        // check if high byte of wIndex is p184 of spec 1.1
        if((tSetupPacket.bIndexH != 0x00)){
            StallEndPoint0();
            return;
        }

        switch (tSetupPacket.bRequest) {
        case USB_REQ_GET_STATUS:
            // check if it is a read command
            if(InTransaction == FALSE){
                // control read but direction is OUT
                StallEndPoint0();
                return;
            }

            // check if wValue is zero
            if((tSetupPacket.bValueH != 0x00) || (tSetupPacket.bValueL != 0x00)){
                StallEndPoint0();
                return;
            }

            // check if bLengthL = 0x02, wLength = 0x00
            if((tSetupPacket.bLengthL != 0x02) || (tSetupPacket.bLengthH != 0x00)) {
                StallEndPoint0();
                return;
            } else {
                tEndPoint0DescriptorBlock.bOEPBCNT = 0x00; // for status stage
            }

            switch (tSetupPacket.bmRequestType & USB_REQ_TYPE_RECIP_MASK) {
            case USB_REQ_TYPE_DEVICE:
                // check if wIndex is zero
                if(tSetupPacket.bIndexL != 0x00){
                    StallEndPoint0();
                    return;
                }

                // Return self power status, no remote wakeup
                bEp0TxBytesRemaining = 2;

                if((bUSBCTL & USBCTL_SELF) == USBCTL_SELF)
                    abReturnBuffer[0] = DEVICE_STATUS_SELF_POWER;
                TransmitBufferOnEp0((PBYTE)abReturnBuffer);
                break;
            case USB_REQ_TYPE_INTERFACE: // return all zeros
                if(tSetupPacket.bIndexL != 0x00){
                    StallEndPoint0();
                    return;
                }

                bEp0TxBytesRemaining = 2;

                TransmitBufferOnEp0 ((PBYTE)abReturnBuffer);

                break;
            case USB_REQ_TYPE_ENDPOINT:

                // Endpoint number is in low byte of wIndex
                bTemp = tSetupPacket.bIndexL & EP_DESC_ADDR_EP_NUM;
                if (bTemp==0) { // EndPoint 0
                    if(tSetupPacket.bIndexL & EP_DESC_ADDR_DIR_IN) {
                        // input endpoint
                        abReturnBuffer[0] = (BYTE)(tEndPoint0DescriptorBlock.bIEPCNFG & EPCNF_STALL);
                    } else {
                        // output endpoint
                        abReturnBuffer[0] = (BYTE)(tEndPoint0DescriptorBlock.bOEPCNFG & EPCNF_STALL);
                    }
                } else {
                    if(bTemp > MAX_ENDPOINT_NUMBER){
                        StallEndPoint0();
                        return;
                    } bTemp--;

                    if(tSetupPacket.bIndexL & EP_DESC_ADDR_DIR_IN)

                        // input endpoint
                        abReturnBuffer[0] = (BYTE)(tInputEndPointDescriptorBlock[bTemp].bEPCNF & EPCNF_STALL); else

                        // output endpoint
                        abReturnBuffer[0] = (BYTE)(tOutputEndPointDescriptorBlock[bTemp].bEPCNF & EPCNF_STALL);

                }

                abReturnBuffer[0] >>= 3; // STALL is on bit 3 bEp0TxBytesRemaining = 2;
                TransmitBufferOnEp0 ((PBYTE)abReturnBuffer); break; case USB_REQ_TYPE_OTHER: default:
                StallEndPoint0();
                break;
            } break;
        case USB_REQ_CLEAR_FEATURE: // check if it is a write command
            if(InTransaction == TRUE){
                // control write but direction is IN
                StallEndPoint0();
                return;
            }

            // check if bLengthL = 0x00, wLength = 0x00

            if((tSetupPacket.bLengthL != 0x00) || (tSetupPacket.bLengthH != 0x00)){
                StallEndPoint0();
                return;
            }

            // control write, stall output endpoint 0 // wLength should be 0 in all cases
            tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;
            switch (tSetupPacket.bmRequestType & USB_REQ_TYPE_RECIP_MASK) {
            case USB_REQ_TYPE_ENDPOINT:
                // Endpoint number is in low byte of wIndex
                if(tSetupPacket.bValueL == FEATURE_ENDPOINT_STALL){
                    bTemp = tSetupPacket.bIndexL & EP_DESC_ADDR_EP_NUM;
                    if(bTemp){
                        if(bTemp > MAX_ENDPOINT_NUMBER){
                            StallEndPoint0();
                            return;
                        }
                        bTemp--; // EP is from EP1 to EP7 while C language start from 0
                        if(tSetupPacket.bIndexL & EP_DESC_ADDR_DIR_IN) {
                            // input endpoint
                            tInputEndPointDescriptorBlock[bTemp].bEPCNF &=~EPCNF_STALL;
                        } else {
                            // output endpoint
                            tOutputEndPointDescriptorBlock[bTemp].bEPCNF &= ~EPCNF_STALL;
                        }
                    }else{ // EP0
                        // clear both in and out stall
                        // no reason to have one stall while the other is not for EP0
                        tEndPoint0DescriptorBlock.bIEPCNFG &= ~EPCNF_STALL;
                        tEndPoint0DescriptorBlock.bOEPCNFG &= ~EPCNF_STALL;
                    }
                } else {
                    StallEndPoint0();
                    return;
                }

                TransmitNullResponseOnEp0();
                break;
            case USB_REQ_TYPE_DEVICE:
            case USB_REQ_TYPE_INTERFACE:
            case USB_REQ_TYPE_OTHER:
            default:
                StallEndPoint0();
                break;
            } break;
        case USB_REQ_SET_FEATURE:
            // check if bLengthL = 0x00, wLength = 0x00
            if((tSetupPacket.bLengthL != 0x00) || (tSetupPacket.bLengthH != 0x00)){
                StallEndPoint0();
                return;
            }

            // check if it is a write command
            if(InTransaction == TRUE){
                // control write but direction is IN
                StallEndPoint0();
                return;
            }

            // control write, stall output endpoint 0
            // wLength should be 0 in all cases
            tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;
            switch (tSetupPacket.bmRequestType & USB_REQ_TYPE_RECIP_MASK)
            {
            // Feature selector is in wValue
            case USB_REQ_TYPE_ENDPOINT:
                // Endpoint number is in low byte of wIndex
                if (tSetupPacket.bValueL == FEATURE_ENDPOINT_STALL){
                    bTemp = tSetupPacket.bIndexL & EP_DESC_ADDR_EP_NUM;
                    // Ignore EP0 STALL, no reason to have EP0 STALL
                    if(bTemp){ // other endpoints
                        if(bTemp > MAX_ENDPOINT_NUMBER){
                            StallEndPoint0();
                            return;
                        }
                        bTemp--; // EP is from EP1 to EP3 while C language start from 0
                        if(tSetupPacket.bIndexL & EP_DESC_ADDR_DIR_IN) {
                            // input endpoint
                            tInputEndPointDescriptorBlock[bTemp].bEPCNF |= EPCNF_STALL;
                        } else {
                            // output endpoint
                            tOutputEndPointDescriptorBlock[bTemp].bEPCNF |= EPCNF_STALL;
                        }
                    }
                } else {
                    StallEndPoint0();
                    return;
                }

                TransmitNullResponseOnEp0();
                break;
            case USB_REQ_TYPE_DEVICE:
            case USB_REQ_TYPE_INTERFACE:
            case USB_REQ_TYPE_OTHER:
            default:
                StallEndPoint0();
                break;
            } break;
        case USB_REQ_SET_ADDRESS: // check if recipient is device
            if((tSetupPacket.bmRequestType & USB_REQ_TYPE_RECIP_MASK) != USB_REQ_TYPE_DEVICE){
                StallEndPoint0();
                return;
            }

            // check if bLengthL = 0x00, wLength = 0x00
            if((tSetupPacket.bLengthL != 0x00) || (tSetupPacket.bLengthH != 0x00)){
                StallEndPoint0();
                return;

            }
            // check if wIndex is zero
            if(tSetupPacket.bIndexL != 0x00){
                StallEndPoint0();
                return;
            }

            // check if it is a write command
            if(InTransaction == TRUE){
                // control write but direction is IN
                StallEndPoint0();
                return;
            }

            // control write, stall output endpoint 0
            // wLength should be 0 in all cases
            tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;

            if(tSetupPacket.bValueL < 128){
                bFUNADR = tSetupPacket.bValueL;
                TransmitNullResponseOnEp0();
            } else {
                StallEndPoint0();
            }
            break;
        case USB_REQ_GET_DESCRIPTOR: // check if recipient is device
            if((tSetupPacket.bmRequestType & USB_REQ_TYPE_RECIP_MASK) != USB_REQ_TYPE_DEVICE){
                StallEndPoint0();
                return;
            }

            // check if it is a read command
            if(InTransaction == FALSE){
                // control read but direction is OUT
                StallEndPoint0();
                return;
            }

            // check if wLength = 0
            if((tSetupPacket.bLengthL | tSetupPacket.bLengthH)==0x00){
                // control read but wLength = 0
                StallEndPoint0();
                return;
            } else { tEndPoint0DescriptorBlock.bOEPBCNT = 0x00;}
            switch (tSetupPacket.bValueH)

            {
            case DESC_TYPE_DEVICE:
                bEp0TxBytesRemaining = SIZEOF_DEVICE_DESCRIPTOR;
                TransmitBufferOnEp0((PBYTE)&abDeviceDescriptor);
                break;
            case DESC_TYPE_CONFIG:
                bEp0TxBytesRemaining = SIZEOF_BOOTCODE_CONFIG_DESC_GROUP;
                TransmitBufferOnEp0((PBYTE)&abConfigurationDescriptorGroup);
                break;
            case DESC_TYPE_STRING:
            case DESC_TYPE_INTERFACE:
            case DESC_TYPE_ENDPOINT:
            default:
                StallEndPoint0();
                return;
            }
            break;
        case USB_REQ_GET_CONFIGURATION:
            // check if recipient is device
            if((tSetupPacket.bmRequestType & USB_REQ_TYPE_RECIP_MASK) != USB_REQ_TYPE_DEVICE){
                StallEndPoint0();
                return;
            }

            // check if it is a read command
            if(InTransaction == FALSE){
                // control read but direction is OUT
                StallEndPoint0();
                return;
            }

            // check if wIndex = 0x00
            if(tSetupPacket.bIndexL != 0x00){
                StallEndPoint0();
                return;
            }

            // check if wValue = 0x00
            if((tSetupPacket.bValueL != 0x00) || (tSetupPacket.bValueH != 0x00)){
                StallEndPoint0();
                return;
            }

            // check if wLength = 1
            if((tSetupPacket.bLengthL != 0x01) || (tSetupPacket.bLengthH!=0x00)) {
                StallEndPoint0();
                return;
            } else {
                tEndPoint0DescriptorBlock.bOEPBCNT = 0x00; bEp0TxBytesRemaining = 1;
            }

            TransmitBufferOnEp0 ((PBYTE)&bConfiguredFlag);
            break;
        case USB_REQ_SET_CONFIGURATION: // check if recipient is device

            if((tSetupPacket.bmRequestType & USB_REQ_TYPE_RECIP_MASK) != USB_REQ_TYPE_DEVICE){
                StallEndPoint0();
                return;
            }
            // check if it is a write command
            if(InTransaction == TRUE){
                // control write but direction is IN
                StallEndPoint0();
                return;

            }

            // check if wIndex = 0x00
            if(tSetupPacket.bIndexL != 0x00) {
                StallEndPoint0();
                return;
            }

            // check if wLength = 0x00

            if((tSetupPacket.bLengthH != 0x00) || (tSetupPacket.bLengthL != 0x00)){
                StallEndPoint0();
                return;
            }

            // control write, stall output endpoint 0
            // wLength should be 0 in all cases
            tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;

            // check if bValueL is greater than 1
            if(tSetupPacket.bValueL > 0x01){ StallEndPoint0(); return; }

            bConfiguredFlag = tSetupPacket.bValueL; TransmitNullResponseOnEp0();
            return;
        case USB_REQ_GET_INTERFACE:

            // check if recipient is interface

            if((tSetupPacket.bmRequestType & USB_REQ_TYPE_RECIP_MASK) != USB_REQ_TYPE_INTERFACE){
                StallEndPoint0(); return;
            }

            if(tSetupPacket.bIndexL != 0x00){
                StallEndPoint0(); return;

            }

            // check if it is a read command
            if(InTransaction == FALSE){
                // control read but direction is OUT
                StallEndPoint0(); return;

            }

            // check if wValue = 0x00

            if((tSetupPacket.bValueL != 0x00) || (tSetupPacket.bValueH != 0x00)){
                StallEndPoint0(); return;

            }

            // check if wLength = 1

            if((tSetupPacket.bLengthL != 0x01) || (tSetupPacket.bLengthH != 0x00)){ StallEndPoint0(); return;

            } else tEndPoint0DescriptorBlock.bOEPBCNT = 0x00; bEp0TxBytesRemaining = 1;

            TransmitBufferOnEp0 ((PBYTE)abReturnBuffer);
            break;
        case USB_REQ_SET_INTERFACE:

            // check if recipient is interface

            if((tSetupPacket.bmRequestType & USB_REQ_TYPE_RECIP_MASK) != USB_REQ_TYPE_INTERFACE){ StallEndPoint0(); return; }

            if(tSetupPacket.bIndexL != 0x00){
                StallEndPoint0(); return;

            }

            // check if it is a write command
            if(InTransaction == TRUE){

                // control write but direction is IN
                StallEndPoint0(); return;

            }

            // check if wLength = 0x00

            if((tSetupPacket.bLengthL != 0x00) || (tSetupPacket.bLengthH != 0x00)){ StallEndPoint0(); return;

            }

            // check if wValue = 0x00

            if((tSetupPacket.bValueL != 0x00) || (tSetupPacket.bValueH != 0x00)){ StallEndPoint0(); return;

            }

            // control write, stall output endpoint 0
            // wLength should be 0 in all cases

            tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL; TransmitNullResponseOnEp0(); return;
        case USB_REQ_SET_DESCRIPTOR:
        case USB_REQ_SYNCH_FRAME:
        default:
            // stall input and output endpoint 0
            StallEndPoint0(); return;

        } break; case USB_REQ_TYPE_VENDOR: // check if recipient is device

        if((tSetupPacket.bmRequestType & USB_REQ_TYPE_RECIP_MASK) != USB_REQ_TYPE_DEVICE){ StallEndPoint0(); return;

        }

        // general requests related to fiemware
        switch(tSetupPacket.bRequest){

        case 0x80: // get bootcode status
            bEp0TxBytesRemaining = 4;
            TransmitBufferOnEp0 ((PBYTE)abBootCodeStatus); break;

        case 0x81: // run firmware
            if(bFirmwareChecksum == bRAMChecksum){ bExecuteFirmware = TRUE; bRAMChecksumCorrect = TRUE;

                TransmitNullResponseOnEp0(); }else StallEndPoint0(); break;

        case 0x82: // Get firmware version
            wIndex = headerReturnFirmwareRevision(); abReturnBuffer[0] = (BYTE)(wIndex & 0x00ff); abReturnBuffer[1] = (BYTE)(wIndex >> 8); bEp0TxBytesRemaining = 2;

            TransmitBufferOnEp0 ((PBYTE)abReturnBuffer); break;

        case 0x83: // prepare for update header
            wCurrentUploadPointer = 0x0000;
            wCurrentFirmwareAddress = 0x0000;
            bRAMChecksum = 0x00;
            wFirmwareLength = 0xffff; // skip firmware length/checksum
            TransmitNullResponseOnEp0();
            break;
        case 0x84: // Update Header

            // bIndexH(BlockSize)
            // bIndexL(wait time)

            // wValueH(device type)

            // wValueL(device id)

            i2cSetMemoryType(tSetupPacket.bValueH); bi2cDeviceAddress = tSetupPacket.bValueL; if(UpdateHeader(wCurrentFirmwareAddress, tSetupPacket.bIndexH,tSetupPacket.bIndexL)==ERROR) StallEndPoint0(); else TransmitNullResponseOnEp0(); break;

        case 0x85: // reboot
            bExecuteFirmware = TRUE; TransmitNullResponseOnEp0(); break;

        case 0x8f: // run firmware (forced run)
            bExecuteFirmware = TRUE; bRAMChecksumCorrect = TRUE; TransmitNullResponseOnEp0(); break;

            // for memory access for advanced feature

        case 0x90: // external memory read
            wIndex = (WORD)(tSetupPacket.bIndexH << 0x08) +

                    (WORD)tSetupPacket.bIndexL; abReturnBuffer[0] = *(pbExternalRAM+wIndex); bEp0TxBytesRemaining = 1;

            TransmitBufferOnEp0 ((PBYTE)abReturnBuffer); break;

        case 0x91: // external memory write
            wIndex = (WORD)(tSetupPacket.bIndexH << 0x08) | (WORD)tSetupPacket.bIndexL; // address write, bValueL(data)

            *(pbExternalRAM+wIndex) = tSetupPacket.bValueL; TransmitNullResponseOnEp0(); break;

        case 0x92: // i2c memory read
            wIndex = (WORD)(tSetupPacket.bIndexH << 0x08) + (WORD)tSetupPacket.bIndexL;

            // bValueL : memory type (cat 1, cat 2, or cat 3), bit7:speed
            // bValueH : device number (A2-A0)
            // wIndex : Address

            i2cSetMemoryType((tSetupPacket.bValueL & MASK_I2C_DEVICE_ADDRESS)); if((tSetupPacket.bValueL & 0x80) != 0x00) i2cSetBusSpeed(I2C_400KHZ); else i2cSetBusSpeed(I2C_100KHZ);

            if(i2cRead(tSetupPacket.bValueH &

                       MASK_I2C_DEVICE_ADDRESS,wIndex,1,&abReturnBuffer[0])==NO_ERROR){ bEp0TxBytesRemaining = 1;

                TransmitBufferOnEp0 ((PBYTE)abReturnBuffer);
            } else StallEndPoint0();

            break;

        case 0x93: // i2c memory write
            wIndex = (WORD)(tSetupPacket.bIndexH << 0x08) + (WORD)tSetupPacket.bIndexL;
            
            // address write, bValueL(data), bValueH(device number)
            abReturnBuffer[0] = tSetupPacket.bValueL;
            if(i2cWrite(tSetupPacket.bValueH & MASK_I2C_DEVICE_ADDRESS,wIndex,1,&abReturnBuffer[0])== NO_ERROR){
                DelaymSecond(0x05);
                TransmitNullResponseOnEp0();
            } else {
                StallEndPoint0();
            }
            break;
        case 0x94: // internal ROM memory read
            wIndex = (WORD)(tSetupPacket.bIndexH << 0x08) + (WORD)tSetupPacket.bIndexL;
            abReturnBuffer[0] = *(pbInternalROM+wIndex);
            bEp0TxBytesRemaining = 1;
            TransmitBufferOnEp0 ((PBYTE)abReturnBuffer);
            break;
#ifdef SIMULATION
            // for internal testing only! The host driver should NOT make these requests
        case 0xe0: // get current checksum
            abReturnBuffer[0] = bRAMChecksum;
            bEp0TxBytesRemaining = 1;
            TransmitBufferOnEp0 ((PBYTE)abReturnBuffer);
            break;
        case 0xe1: // get downloaded size
            abReturnBuffer[0] = (BYTE)(wCurrentFirmwareAddress & 0x00ff);
            abReturnBuffer[1] = (BYTE)((wCurrentFirmwareAddress & 0xff00) >> 8);
            bEp0TxBytesRemaining = 2;
            TransmitBufferOnEp0 ((PBYTE)abReturnBuffer);
            break;
        case 0xe2: // set download size and checksum
            // wValue(firmware size)
            // bIndexL(checksum)
            wCurrentFirmwareAddress = (WORD)(tSetupPacket.bValueH << 0x08) + (WORD)tSetupPacket.bValueL;
            bRAMChecksum = tSetupPacket.bIndexL;
            TransmitNullResponseOnEp0();
            break;
        case 0xF0: // ROM Address Dump
            wIndex = (WORD)(tSetupPacket.bIndexH << 0x08) + (WORD)tSetupPacket.bIndexL; lcdRomDump(wIndex,4); TransmitNullResponseOnEp0();
            break;
        case 0xF1: // External Memory Dump
            wIndex = (WORD)(tSetupPacket.bIndexH << 0x08) + (WORD)tSetupPacket.bIndexL; lcdExternalMemoryDump(wIndex,4); TransmitNullResponseOnEp0();
            break;
        case 0xF2: // I2C Dump
            // wIndexH(Data Address)
            // wValueH(device type)
            // wValueL(device id)
            i2cSetMemoryType(tSetupPacket.bValueH);
            bi2cDeviceAddress = tSetupPacket.bValueL;
            wIndex = (WORD)(tSetupPacket.bIndexH << 0x08) + (WORD)tSetupPacket.bIndexL; lcdI2cDump(wIndex,4); TransmitNullResponseOnEp0();
            break;
#endif 
        default:
            // stall input and output endpoint 0
            StallEndPoint0();
            return;
        }
        break;
    case USB_REQ_TYPE_CLASS:
    default:
        StallEndPoint0();
        return;
    }

} //----------------------------------------------------------------------------

VOID UsbDataInitialization(VOID)

{

    bFUNADR = 0x00; // no device address
    bEp0TxBytesRemaining = 0xff; // no data remaining
    pbEp0Buffer = NULL;

    bConfiguredFlag = 0x00; // device unconfigured
    bExecuteFirmware = FALSE; // a flag set by USB request

    // before bootocode hands over // control to firmware
    bCurrentBuffer = X_BUFFER; // for firmware download
    bRAMChecksumCorrect = FALSE; wCurrentFirmwareAddress = 0x0000; bRAMChecksum = 0x00;
    wFirmwareLength = 0x0000; // enable endpoint 0 interrupt

    tEndPoint0DescriptorBlock.bIEPCNFG = EPCNF_USBIE | EPCNF_UBME; tEndPoint0DescriptorBlock.bOEPCNFG = EPCNF_USBIE | EPCNF_UBME; // enable endpoint 1 interrupt

    tOutputEndPointDescriptorBlock[0].bEPCNF = EPCNF_USBIE | EPCNF_UBME | EPCNF_DBUF; tOutputEndPointDescriptorBlock[0].bEPBBAX = (BYTE)(OEP1_X_BUFFER_ADDRESS >> 3 &

                                                                                                                                         0x00ff);

    tOutputEndPointDescriptorBlock[0].bEPBCTX = 0x0000;

    tOutputEndPointDescriptorBlock[0].bEPBBAY = (BYTE)(OEP1_Y_BUFFER_ADDRESS >> 3 &

                                                       0x00ff);

    tOutputEndPointDescriptorBlock[0].bEPBCTY = 0x0000; tOutputEndPointDescriptorBlock[0].bEPSIZXY = EP_MAX_PACKET_SIZE; // Enable the USB-specific Interrupts; SETUP, RESET and STPOW bUSBMSK = USBMSK_STPOW | USBMSK_SETUP | USBMSK_RSTR; // disable all four downstream port bHUBCNF1 = 0x00;

    // enable port 6 and keep power switch setting
    bHUBCNF2 |= HUBCNF2_P6_EN;

}
VOID CopyDefaultSettings(VOID)

{

    BYTE bTemp;

    // clear out status bits
    abBootCodeStatus[0] = 0x00; abBootCodeStatus[1] = 0x00; abBootCodeStatus[2] = 0x00; abBootCodeStatus[3] = 0x00;

    // disconnect from USB
    bUSBCTL = 0x00; // Disable endpoints EP1

    tOutputEndPointDescriptorBlock[0].bEPCNF = 0x00;

    // set default values for hub
    bHUBPIDL = HUB_PID_L; bHUBPIDH = HUB_PID_H; bHUBVIDL = HUB_VID_L; bHUBVIDH = HUB_VID_H;

    // copy descriptor to allocated address // copy device and configuration descriptor to external memory
    for(bTemp=0;bTemp<SIZEOF_DEVICE_DESCRIPTOR;bTemp++)
        abDeviceDescriptor[bTemp] = abromDeviceDescriptor[bTemp];

    for(bTemp=0;bTemp<SIZEOF_BOOTCODE_CONFIG_DESC_GROUP;bTemp++)
        abConfigurationDescriptorGroup[bTemp] = abromConfigurationDescriptorGroup[bTemp];

    // set power wait time for the hub

    bHUBPOTG = HUBPOTG_100MS; // 100 ms from power-on to power-good

    // set power rating for the hub

    bHUBCURT = HUBCURT_100MA; // 100 ms from power-on to power-good

    // set i2c speed
    i2cSetBusSpeed(I2C_100KHZ);

}

/*---------------------------------------------------------------------------+
| Interrupt Sub-routines |
+---------------------------------------------------------------------------*/
//----------------------------------------------------------------------------

VOID SetupPacketInterruptHandler(VOID)

{

    bEp0TxBytesRemaining = 0xFF; // setup packet received successfully
    pbEp0Buffer = NULL; // clear remaining to be transmitted on endpoint 0 Endpoint0Control();

} //----------------------------------------------------------------------------

VOID Ep0InputInterruptHandler(VOID)

{

    // check if the last packet sent

    tEndPoint0DescriptorBlock.bOEPBCNT = 0x00; // is set by the hardware
    if(bEp0TxBytesRemaining == 0xff){

        // last packet just sent, stall input endpoint 0

        // so error conditions would occurs when the host asks more data
        tEndPoint0DescriptorBlock.bIEPCNFG |= EPCNF_STALL;

    }else FillEp0TxWithNextDataPacket();

}

//----------------------------------------------------------------------------

VOID Ep0OutputInterruptHandler(VOID) {

    // happened only in status stage
    // Bootrom doesn?t handle data stage of control write.
    // stall for any OUT, this is cleared in the setup stage.

    tEndPoint0DescriptorBlock.bOEPCNFG |= EPCNF_STALL;

} //----------------------------------------------------------------------------

VOID Ep1OutputInterruptHandler(VOID)

{

    BYTE bTemp,bSize,bCode; // check if it is the first packet
    if(wFirmwareLength == 0x0000){

        wFirmwareLength = (WORD)abXBufferAddress[0]; wFirmwareLength += (WORD)(abXBufferAddress[1] << 8); bFirmwareChecksum = abXBufferAddress[2];

        bSize = tOutputEndPointDescriptorBlock[0].bEPBCTX & EPBCT_BYTECNT_MASK; for(bTemp=3;bTemp<bSize;bTemp++){ bCode = abXBufferAddress[bTemp];

            abDownloadFirmware[wCurrentFirmwareAddress] = bCode; bRAMChecksum += bCode; wCurrentFirmwareAddress++;

        }

        bCurrentBuffer = Y_BUFFER; // clear NAK bit

        tOutputEndPointDescriptorBlock[0].bEPBCTX = 0x00;

    }else{

        if(bCurrentBuffer == X_BUFFER){ // firgure out the size of packet

            bSize = tOutputEndPointDescriptorBlock[0].bEPBCTX & EPBCT_BYTECNT_MASK; for(bTemp=0;bTemp<bSize;bTemp++){ bCode = abXBufferAddress[bTemp];

                abDownloadFirmware[wCurrentFirmwareAddress] = bCode; bRAMChecksum += bCode; wCurrentFirmwareAddress++;

            }

            bCurrentBuffer = Y_BUFFER; // clear NAK bit

            tOutputEndPointDescriptorBlock[0].bEPBCTX = 0x00;

        }else{ // data in y buffer // firgure out the size of packet

            bSize = tOutputEndPointDescriptorBlock[0].bEPBCTY & EPBCT_BYTECNT_MASK; for(bTemp=0;bTemp<bSize;bTemp++){ bCode = abYBufferAddress[bTemp];

                abDownloadFirmware[wCurrentFirmwareAddress] = bCode; bRAMChecksum += bCode; wCurrentFirmwareAddress++;

            }

            bCurrentBuffer = X_BUFFER; // clear NAK bit

            tOutputEndPointDescriptorBlock[0].bEPBCTY = 0x00;

        } }

    // check if firmware is ready

    if((WORD)wCurrentFirmwareAddress >= wFirmwareLength){ // check is checksum is correct
        if(bRAMChecksum == bFirmwareChecksum){

#ifdef SIMULATION

            lcdPutString("USB Checksum Correct!");

            DelaymSecond(2000);

#endif

            bRAMChecksumCorrect = TRUE; bExecuteFirmware = TRUE;

        }else{

#ifdef SIMULATION
            lcdPutString("USB Checksum Incorrect!");

            DelaymSecond(2000);

#endif

            bRAMChecksumCorrect = FALSE;

        }

    }

}

/*---------------------------------------------------------------------------+

| Interrupt Service Routines | +---------------------------------------------------------------------------*/
//interrupt [0x03] void EX0_int(void) // External Interrupt 0
//void EX0_int(void) __interrupt 0 using 2 // External Interrupt 0
__interrupt void EX0_int(void) // External Interrupt 0
{
    IE_bit.EA = DISABLE; // Disable any further interrupts

    // always clear the interrupt source first and then bVECINT
    switch (bVECINT){ // Identify Interrupt ID
    case VECINT_OUTPUT_ENDPOINT0: bVECINT = 0x00; Ep0OutputInterruptHandler(); break;
    case VECINT_INPUT_ENDPOINT0: bVECINT = 0x00; Ep0InputInterruptHandler(); break;
    case VECINT_OUTPUT_ENDPOINT1: bVECINT = 0x00; Ep1OutputInterruptHandler(); break;
    case VECINT_STPOW_PACKET_RECEIVED:

        SetupPacketInterruptHandler();

        // clear setup packet flag (clear source first)
        bUSBSTA = USBSTA_STPOW; bVECINT = 0x00;
        break;
    case VECINT_SETUP_PACKET_RECEIVED:

        SetupPacketInterruptHandler(); // clear setup packet flag
        bUSBSTA = USBSTA_SETUP; bVECINT = 0x00; break;
    case VECINT_RSTR_INTERRUPT:

        UsbDataInitialization();

        // clear reset flag
        bUSBSTA = USBSTA_RSTR; bVECINT = 0x00;
        break;

    default:break; // unknown interrupt ID

    }

    IE_bit.EA = ENABLE; // Enable the interrupts again

} //----------------------------------------------------------------------------

#ifdef SIMULATION

BYTE code abTestHeader[16] = {
    0x52,0x51, // product ID
    0x01, // Data Type (USB Info)
    0x09,0x00, // Data Size (9 bytes)
    0x80, // checksum
    0x81, // bit setting (slef and switching)
    0x51,0x04, // VID
    0x34,0x12, // PID for hub
    0x78,0x56, // PID for function
    0x32, // for HUBPOTG
    0x64, // for HUBCURT
    0x00 // End of descriptor
};

BYTE code abNullHeader[8] = {0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};

#endif //----------------------------------------------------------------------------

VOID main(VOID)
{

#ifdef SIMULATION
    WORD wAddress;
    for(wAddress = 0; wAddress < 0x8000; wAddress++) {
        abDownloadFirmware[wAddress] = 0x00;
        gpioInitialization();
        lcdPutString("UMP BootSim ");
    }

    // clear i2
    // for(wAddress=0x0100; wAddress < 0x200;wAddress+=8){
    // i2cWrite(0x00,wAddress,8,abNullHeader);
    // DelaymSecond(20);
    // }

    i2cSetBusSpeed(I2C_100KHZ); i2cSetMemoryType(I2C_CATEGORY_3); for(wAddress=0;wAddress<16;wAddress++){ // i2cWrite(0x00,wAddress,1,&abTestHeader[wAddress]);

        DelaymSecond(20);

    }

    // lcdI2cDump(0x0000,4);
    // ledPutChar(0x01,0x64);
    // while(1);
#endif

    CopyDefaultSettings();
    UsbDataInitialization();
    // partial support due to memory size

    if(headerSearchForValidHeader() == DATA_MEDIUM_HEADER_I2C) {
        if(headerGetDataType(1) == DATA_TYPE_HEADER_HUB_INFO_BASIC){
            if(headerProcessCurrentDataType()==MSG_HEADER_NO_ERROR){

#ifdef SIMULATION
                lcdPutString("usbinfo");
#endif
                bRAMChecksumCorrect = TRUE;
            }
        }

        if(headerGetDataType(2) == DATA_TYPE_HEADER_FIRMWARE_BASIC){
            if(headerProcessCurrentDataType()==MSG_HEADER_NO_ERROR){

#ifdef SIMULATION
                lcdPutString("firmware");
#endif
                bRAMChecksumCorrect = TRUE;
                bExecuteFirmware = TRUE;
            }
        }
    }

    if(bExecuteFirmware == FALSE){
        // use default value
        IE_bit.EA = ENABLE; // Enable global interrupt
        IE_bit.EX0 = ENABLE; // Enable interrupt 0
        bUSBCTL |= USBCTL_CONT; // connect to upstream port
#ifdef SIMULATION
        lcdPutString("Connecting");
        bUSBCTL |= 0xc0;
#endif
        while(bExecuteFirmware == FALSE);
    }

    // Disable all interrupts

    IE_bit.EA = DISABLE; // disable global interrupt
    // disconnection
    bUSBCTL = USBCTL_FRSTE;
    // map xdata to code space if checksum correct
    // now application code is in code space
    if(bRAMChecksumCorrect == TRUE){
#ifdef SIMULATION
        lcdPutStringXY(0,2,"Set Shadow Bit");
        DelaymSecond(4000);
#endif
        bROMS |= ROMS_SDW;
    } else {
#ifdef SIMULATION
        lcdPutStringXY(0,2,"Shadow Bit Not Set!");
        DelaymSecond(4000);
#endif
    }
    (*(void(*)(void))0x0000)(); // run firmware now
}
