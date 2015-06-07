/*----------------------------------------------------------------------------+
|                                                                             |
|                             Texas Instruments                               |
|                                                                             |
|                                 Header                                      |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: header.c, v 1.0 2000/05/28 12:59:29                                |
|  Author: Horng-Ming Lobo Tai lobotai@ti.com                                 |
|  Header: (none)                                                             |
|                                                                             |
|  For more information, please contact                                       |
|                                                                             |
|  Lobo Tai                                                                   |
|  Texas Instruments                                                          |
|  12500 TI BLVD, MS8761                                                      |
|  Dallas, TX 75243                                                           |
|  USA                                                                        |
|                                                                             |
|  Tel 214-480-3145                                                           |
|  Fax 214-480-3443                                                           |
|                                                                             |
|  Logs:                                                                      |
|                                                                             |
|  WHO     WHEN         WHAT                                                  |
|  ---     --------     ----------------------------------------------------- |
|  HMT     20000528     born                                                  |
|                                                                             |
+----------------------------------------------------------------------------*/
#include <io51.h>       // 8051 sfr definition
#include "types.h"
#include "tusb5052.h"
#include "header.h"
#include "i2c.h"
#include "usb.h"
#include "delay.h"

#ifdef SIMULATION
#include "gpio.h"
#endif


extern BYTE xdata abDeviceDescriptor[SIZEOF_DEVICE_DESCRIPTOR];
extern BYTE xdata abConfigurationDescriptorGroup[SIZEOF_BOOTCODE_CONFIG_DESC_GROUP];
// BYTE xdata abEepromHeader[0x80] _at_ 0xFE30;

__no_init BYTE xdata abEepromHeader[0x80] @ 0xFE40;

extern BYTE xdata abDownloadFirmware[DOWNLOAD_FW_SIZE];

/*---------------------------------------------------------------------------+
|  Constant Definition                                                       |
+---------------------------------------------------------------------------*/

// Local Variables
BYTE   data bCurrentHeaderMediumType;
ULONG   data ulCurrentHeaderPointer;
BYTE    data bCurrentDataType;
WORD    data wCurrentDataSize;
BYTE    data bCurrentDataChecksum;
tFirmwareRevision    data tCurrentFirmwareRevision;
WORD    data wCurrentUploadPointer;
BYTE    data bi2cDeviceAddress;

/*---------------------------------------------------------------------------+
|  Sub-routines go here...                                                   |
+---------------------------------------------------------------------------*/
BYTE headerCheckProductIDonI2c(VOID)
{
    // in simulation, if error, abEepromHeader could be xx.
    abEepromHeader[0] = 0x00;
    abEepromHeader[1] = 0x00;
    abEepromHeader[2] = 0x00;
    abEepromHeader[3] = 0x00;
    
    if(i2cRead(bi2cDeviceAddress, 0x0000, 0x02, (PBYTE)&abEepromHeader[0]) == NO_ERROR){
        if((abEepromHeader[0] == FUNCTION_PID_L) && (abEepromHeader[1] == FUNCTION_PID_H)){
            bCurrentHeaderMediumType = DATA_MEDIUM_HEADER_I2C;
            #ifdef SIMULATION
            lcdPutString("Header Found!! DevAdr:");
            lcdPutByte(bi2cDeviceAddress);
            #endif
            return DATA_MEDIUM_HEADER_I2C;
        }
    }
    return DATA_MEDIUM_HEADER_NO;
}
//----------------------------------------------------------------------------
// only support i2c due to memory size
BYTE headerSearchForValidHeader(VOID)
{
    // check CAT I
    #ifdef SIMULATION
    lcdPutString("CAT I ");
    #endif 
    i2cSetMemoryType(I2C_CATEGORY_1);
    bi2cDeviceAddress = 0x0000;
    if(headerCheckProductIDonI2c()==DATA_MEDIUM_HEADER_I2C)
    return DATA_MEDIUM_HEADER_I2C;
        
    // check CAT II
    #ifdef SIMULATION
    lcdPutString("CAT II ");
    #endif
    i2cSetMemoryType(I2C_CATEGORY_2);
    for(bi2cDeviceAddress=0;bi2cDeviceAddress<8;bi2cDeviceAddress++)
        if(headerCheckProductIDonI2c()==DATA_MEDIUM_HEADER_I2C) return DATA_MEDIUM_HEADER_I2C;

    // check CAT III
    #ifdef SIMULATION
    lcdPutString("CAT III ");
    #endif
    i2cSetMemoryType(I2C_CATEGORY_3);
    for(bi2cDeviceAddress=0;bi2cDeviceAddress<8;bi2cDeviceAddress++)
        if(headerCheckProductIDonI2c()==DATA_MEDIUM_HEADER_I2C)return DATA_MEDIUM_HEADER_I2C; 

    return DATA_MEDIUM_HEADER_NO;
}

//----------------------------------------------------------------------------
// only support i2c due to memory size
BYTE headerGetDataType(WORD wNumber)
{
    WORD wAddress;
    tHeaderPrefix tData;

    bCurrentDataType        = DATA_TYPE_HEADER_END;
    wAddress                = OFFSET_HEADER_FIRST_DATA_SECTION;

    if(bCurrentHeaderMediumType == DATA_MEDIUM_HEADER_I2C){
        while(wNumber != 0x0000){
            i2cRead(bi2cDeviceAddress, wAddress, sizeof(tHeaderPrefix),(PBYTE)&tData);
            bCurrentDataType     = tData.bDataType;
            wCurrentDataSize     = (WORD)tData.bDataSize_L;
            wCurrentDataSize    += (WORD)(tData.bDataSize_H << 8);
            bCurrentDataChecksum = tData.bDataChecksum;
            wAddress            += (wCurrentDataSize+sizeof(tHeaderPrefix));
            wNumber--;
        }

        if(wAddress != OFFSET_HEADER_FIRST_DATA_SECTION)
            ulCurrentHeaderPointer = (ULONG)(wAddress-wCurrentDataSize);
        else ulCurrentHeaderPointer = OFFSET_HEADER_FIRST_DATA_SECTION;
    }
    return bCurrentDataType;
}

//----------------------------------------------------------------------------
BYTE LoadFirmwareBasicFromI2c(VOID)
{
    BYTE bChecksum;
    WORD wAddressI2c;
    WORD wProgramSize,wAddress;

    wAddressI2c = (WORD)ulCurrentHeaderPointer;
    
    if(i2cRead(bi2cDeviceAddress, wAddressI2c, sizeof(tCurrentFirmwareRevision), 
               (PBYTE)&tCurrentFirmwareRevision) == ERROR) return ERROR;

    wProgramSize         = wCurrentDataSize - sizeof(tFirmwareRevision);
    wAddressI2c          = wAddressI2c + sizeof(tFirmwareRevision);
         
    if(i2cRead(bi2cDeviceAddress,wAddressI2c, wProgramSize, (PBYTE)&abDownloadFirmware[0x0000]) != NO_ERROR) 
      return ERROR;
       
    // get Checksum from RAM
    bChecksum  = tCurrentFirmwareRevision.bMinor;
    bChecksum += tCurrentFirmwareRevision.bMajor;
    
    for(wAddress=0x0000;wAddress<wProgramSize;wAddress++)
        bChecksum += abDownloadFirmware[wAddress];
           
    if(bCurrentDataChecksum != bChecksum) return ERROR;
    else return NO_ERROR;
}
//----------------------------------------------------------------------------
// only support i2c due to memory size
BYTE LoadUsbInfoBasicFromI2c(VOID)
{
    BYTE bTemp,bChecksum;
    WORD wTemp,wAddress;
    tHeaderUsbInfoBasic *ptUsbInfoBasic;

    wAddress = ulCurrentHeaderPointer;

    ptUsbInfoBasic = (tHeaderUsbInfoBasic *)abEepromHeader;
    if(i2cRead(bi2cDeviceAddress,wAddress, wCurrentDataSize, (PBYTE)&abEepromHeader[0x0000]) != NO_ERROR) 
       return ERROR;
          
    // get check sum
    bChecksum = 0x00;   
    for(wTemp=0x0000;wTemp<wCurrentDataSize;wTemp++)
        bChecksum += abEepromHeader[wTemp];
 
    // check if the data is for hub info
    if(bChecksum == bCurrentDataChecksum){

        // download VID and VIP Information from EEPROM
        bHUBVIDL = ptUsbInfoBasic->bVID_L;
        bHUBVIDH = ptUsbInfoBasic->bVID_H;
        bHUBPIDL = ptUsbInfoBasic->bPID_HUB_L;
        bHUBPIDH = ptUsbInfoBasic->bPID_HUB_H;

        // update device descriptor
        abDeviceDescriptor[OFFSET_DEVICE_DESCRIPTOR_PID_L]
                  = ptUsbInfoBasic->bPID_FUNC_L;
        abDeviceDescriptor[OFFSET_DEVICE_DESCRIPTOR_PID_H]
                  = ptUsbInfoBasic->bPID_FUNC_H;
        abDeviceDescriptor[OFFSET_DEVICE_DESCRIPTOR_VID_L] = bHUBVIDL;
        abDeviceDescriptor[OFFSET_DEVICE_DESCRIPTOR_VID_H] = bHUBVIDH;

        // Time for power-on to power-good
        bHUBPOTG = ptUsbInfoBasic->bHubPotg;
        #ifdef SIMULATION
        lcdPutString("POTG=");
        lcdPutByte(ptUsbInfoBasic->bHubPotg);
        #endif

        // set power rating for the hub
        bHUBCURT = ptUsbInfoBasic->bHubCurt;
        #ifdef SIMULATION
        lcdPutString("CURT=");
        lcdPutByte(ptUsbInfoBasic->bHubCurt);
        #endif

        // set PWRSW
        bTemp = ptUsbInfoBasic->bBitSetting & BIT_HEADER_PWRSW;
        if(bTemp == BIT_HEADER_PWRSW){
            bHUBCNF2 |= HUBCNF2_PWRSW;
            #ifdef SIMULATION
            lcdPutString("PWRSW!");
            #endif
        }

       // bus-powered or self-powered
        bTemp = ptUsbInfoBasic->bBitSetting & BIT_HEADER_BSPWR;
        if(bTemp == BIT_HEADER_BSPWR){ // self-powered
            bUSBCTL |= USBCTL_SELF;
            abConfigurationDescriptorGroup[OFFSET_CONFIG_DESCRIPTOR_POWER]= (BYTE)(0x80 | CFG_DESC_ATTR_SELF_POWERED);
            #ifdef SIMULATION
            lcdPutString("Self!");
            #endif

        }else{
            #ifdef SIMULATION
            lcdPutString("Bus!");
            #endif
        }
    }else{
            #ifdef SIMULATION
            lcdPutString("IDCSERR!");
            //lcdPutByte(bCurrentDataChecksum);
            //lcdPutByte(bChecksum);
            //lcdExternalMemoryDump((WORD)&abEepromHeader[0],4);
            //while(1);
            #endif
            return ERROR;
    }
    return NO_ERROR;
}

//----------------------------------------------------------------------------
// only support i2c due to memory size
BYTE headerProcessCurrentDataType(VOID)
{
    if(bCurrentHeaderMediumType == DATA_MEDIUM_HEADER_I2C){
        if(bCurrentDataType == DATA_TYPE_HEADER_HUB_INFO_BASIC){
            if(LoadUsbInfoBasicFromI2c() == ERROR) return MSG_HEADER_CHECKSUM_ERROR;
        }else if(bCurrentDataType == DATA_TYPE_HEADER_FIRMWARE_BASIC){
            if(LoadFirmwareBasicFromI2c() == ERROR) return MSG_HEADER_CHECKSUM_ERROR;
        }else return MSG_HEADER_DATA_TYPE_ERROR;
    }else return MSG_HEADER_DATA_MEDIUM_ERROR;
    
    return MSG_HEADER_NO_ERROR;
}

//----------------------------------------------------------------------------
WORD headerReturnFirmwareRevision(VOID)
{
    WORD wCurrentFirmwareRevision;
    
    wCurrentFirmwareRevision  = (WORD)tCurrentFirmwareRevision.bMajor << 8;
    wCurrentFirmwareRevision |= tCurrentFirmwareRevision.bMinor;
    
    return wCurrentFirmwareRevision;
}
//----------------------------------------------------------------------------
// only support i2c due to memory size
BOOL UpdateHeader(WORD wHeaderSize, BYTE bBlockSize, BYTE bWaitTime)
{
    WORD wAddress;

#ifdef SIMULATION
    lcdPutString("upload");
    lcdPutByte(bBlockSize);
    lcdPutByte(bWaitTime);
    lcdPutWord(wHeaderSize);
    lcdPutWord(wCurrentUploadPointer);
#endif

    wAddress        = 0x0000;
    if(wHeaderSize >= (WORD)bBlockSize){
        // writer each block
        do{
            if(i2cWrite(bi2cDeviceAddress,wCurrentUploadPointer, 
                (WORD)bBlockSize,(PBYTE)&abDownloadFirmware[wAddress]) 
                != NO_ERROR) return ERROR;

            wCurrentUploadPointer = wCurrentUploadPointer + (WORD)bBlockSize;
            wAddress    = wAddress + (WORD)bBlockSize;

            DelaymSecond((WORD)bWaitTime);
        }while((wAddress + (WORD)bBlockSize) <= wHeaderSize);
    }
    
    // writer partial block
    while(wAddress < wHeaderSize){           
        if(i2cWrite(bi2cDeviceAddress,wCurrentUploadPointer,0x0001,
            (PBYTE)&abDownloadFirmware[wAddress]) != NO_ERROR) return ERROR;
        DelaymSecond((WORD)bWaitTime);
        wAddress++;
        wCurrentUploadPointer++;
    }
    return NO_ERROR;
}
//----------------------------- Cut along the line ----------------------------
