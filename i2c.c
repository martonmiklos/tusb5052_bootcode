/*----------------------------------------------------------------------------+
|                                                                             |
|                             Texas Instruments                               |
|                                                                             |
|                                   I2C                                       |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: i2c.c, v 1.0 99/01/28 11:00:36                                     |
|  Author: Horng-Ming Lobo Tai lobotai@ti.com                                 |
|  Header: (none)                                                             |
|                                                                             |
|  For more information, please contact                                       |
|                                                                             |
|  Lobo Tai                                                                   |
|  Texas Instruments                                                          |
|  12500 TI Blvd, MS 8761                                                     |
|  Dallas, TX 75243                                                           |
|  USA                                                                        |
|                                                                             |
|  Tel 214-480-3145                                                           |
|  Fax 214-480-3443                                                           |
|                                                                             |
|       Notes:                                                                |
|                                                                             |
|  WHO     WHEN         WHAT                                                  |
|  ---     --------     ----------------------------------------------------- |
|  HMT     19990128     born                                                  |
|  HMT     19990414     add the following functions (SiCore's format)         |
|  HMT     19990423     modified i2cWaitForRead & i2cWaitForWrite             |
|                       to write '1' clear and removed eUMP functions         |
|  HMT     19990611     fixed bug in i2cRead when bNumber is 1                |
|  HMT     19991124     port to usb keyboard                                  |
|  HMT     20000122     add control code 0xa0                                 |
|  HMT     20000614     add to support cat i,ii, and iii devices              |
|  HMT     20000615     i2c on cat 2 device:                                  |
|                       some cat 2 uses A0,A1 and A2(or partially) but some   |
|                       don't. Therefore, in the i2c routine, if the          |
|                       address is more than 0xff, it will overwirte the      |
|                       device address by higher data address. In this way,   |
|                       routine can cover most of devices with minor issues.  |
|  HMT     20000630     Bug fixed on wait for read for last byte in high      |
|                       speed. uC is too slow to set SRD bit.                 |
|                                                                             |
+----------------------------------------------------------------------------*/
#include "types.h"
#include "i2c.h"
#include "tusb5052.h"

#ifdef I2C_TEST
#include "delay.h"
#include "gpio.h"
#endif

static BYTE bDeviceCategory;

//----------------------------------------------------------------------------
VOID i2cSetBusSpeed(BYTE bBusSpeed)
{
    if(bBusSpeed == I2C_400KHZ) 
      bI2CSTA |= I2CSTA_400K;     // set bus speed at 400Khz
    else 
      bI2CSTA &= ~I2CSTA_400K;                           // set bus speed at 100Khz
}

//----------------------------------------------------------------------------
VOID i2cSetMemoryType(BYTE bType)
{
    if( bType > I2C_CATEGORY_LAST) return;                          // invalid memory type
    else bDeviceCategory = bType;
}

//----------------------------------------------------------------------------
BYTE i2cWaitForRead(VOID)
{
    // wait until data is ready or ERR=1
    while((bI2CSTA & I2CSTA_RXF) != I2CSTA_RXF)
        if((bI2CSTA & I2CSTA_ERR) == I2CSTA_ERR){
            bI2CSTA |= I2CSTA_ERR;          // clear error flag
            return ERROR;
        }
    return NO_ERROR;
}

//----------------------------------------------------------------------------
BYTE i2cWaitForWrite(VOID)
{
    // wait until TXE bit is cleared or ERR=1
    while((bI2CSTA & I2CSTA_TXE) != I2CSTA_TXE)
        if((bI2CSTA & I2CSTA_ERR) == I2CSTA_ERR){
            bI2CSTA |= I2CSTA_ERR;          // clear error flag
            return ERROR;
        }
    return NO_ERROR;
}

//----------------------------------------------------------------------------
BYTE i2cRead(BYTE bDeviceAddress, WORD wAddress, WORD wNumber, PBYTE pbDataArray)
{
    BYTE bTemp,bHiAddress;

    // If error, return a value other than zero.
    if(wNumber == 0x00) return NO_ERROR;

    bI2CSTA &= ~(I2CSTA_SRD | I2CSTA_SWR);  // clear SRD abnd SWR bit

    if(bDeviceCategory == I2C_CATEGORY_1){
        // cat 1, not tested!
        bI2CADR = (BYTE)((wAddress << 1) | BIT_I2C_READ);
    }else{
        // cat 2 or 3
        bTemp    = bDeviceAddress & MASK_I2C_DEVICE_ADDRESS;    // write device address and RW=0
        bTemp    = bTemp << 1;
        bTemp   |= BIT_I2C_DEVICE_TYPE_MEMORY;                  // add control code

        // check if data address is higher than 0xff in cat 2 device
        if((bDeviceCategory == I2C_CATEGORY_2) && (wAddress > 0x00ff)){
            bHiAddress = (wAddress >> 8) & MASK_I2C_DEVICE_ADDRESS;
            bHiAddress  = bHiAddress << 1;
            bTemp      |= bHiAddress;
        }
        bI2CADR = bTemp;                                // write out device address

        if(bDeviceCategory == I2C_CATEGORY_3){
            bI2CDAO  =  (BYTE)(wAddress >> 8);          // write out high byte of address
            if(i2cWaitForWrite() != NO_ERROR) return ERROR; // bus error
        }

        bI2CDAO  =  (BYTE)(wAddress & 0xff);            // write out low byte of address
        if(i2cWaitForWrite() != NO_ERROR) return ERROR; // bus error

        bI2CADR = (bTemp | BIT_I2C_READ);               // setup read
    }
    bI2CDAO = 0x00;                                     // start read

    // SRD should be cleared
    if(wNumber > 1){
        while(wNumber > 1){
            if(i2cWaitForRead() != NO_ERROR) return ERROR;  // bus error
            if(wNumber == 2) bI2CSTA |= I2CSTA_SRD;
            *pbDataArray++ = bI2CDAI;
            wNumber--;
        }
    }else bI2CSTA |= I2CSTA_SRD;

    // read the last byte
    if(i2cWaitForRead() != NO_ERROR) return ERROR;      // bus error
    *pbDataArray = bI2CDAI;

    return NO_ERROR;
}

//----------------------------------------------------------------------------
BYTE i2cWrite(BYTE bDeviceAddress, WORD wAddress, WORD wNumber, PBYTE pbDataArray)
{
    BYTE bTemp,bHiAddress;


    bI2CSTA &= ~(I2CSTA_SRD | I2CSTA_SWR);          // clear SRD abnd SWR bit

    // If error, return a value other than zero.
    if(wNumber == 0x00) return NO_ERROR;

    if(bDeviceCategory == I2C_CATEGORY_1){
        // cat 1, not tested!
        bI2CADR = (BYTE)(wAddress << 1);
    }else{
        // cat 2 or 3
        bTemp    = bDeviceAddress & MASK_I2C_DEVICE_ADDRESS;    // write device address and RW=0
        bTemp    = bTemp << 1;
        bTemp   |= BIT_I2C_DEVICE_TYPE_MEMORY;                  // add control code

        // check if data address is higher than 0xff in cat 2 device
        if((bDeviceCategory == I2C_CATEGORY_2) && (wAddress > 0x00ff)){
            bHiAddress  = (BYTE)(wAddress >> 8) & MASK_I2C_DEVICE_ADDRESS;
            bHiAddress  = bHiAddress << 1;
            bTemp      |= bHiAddress;
        }
        bI2CADR = bTemp;                                    // write out device address

        if(bDeviceCategory == I2C_CATEGORY_3){
            bI2CDAO  =  (BYTE)(wAddress >> 8);              // write out high byte of address
            if(i2cWaitForWrite() != NO_ERROR) return ERROR; // bus error
        }

        bI2CDAO  =  (BYTE)(wAddress & 0xff);                // write out low byte of address
        if(i2cWaitForWrite() != NO_ERROR) return ERROR;     // bus error
    }

    // SRD should be cleared.
    while(wNumber > 1){
        bI2CDAO = *pbDataArray++;
        if(i2cWaitForWrite() != NO_ERROR) return ERROR;     // bus error
        wNumber--;
    }

    // write the last byte
    bI2CSTA |= I2CSTA_SWR;                          // set SWR bit
    bI2CDAO  = *pbDataArray;                        // write out the data
    if(i2cWaitForWrite() != NO_ERROR) return ERROR; // bus error
    return NO_ERROR;
}

#ifdef I2C_TEST

#define TEST_PATTERN_SIZE   2
#define TEST_PAGE_SIZE      8

#define TEST_SIZE_CAT3      0x4000
#define TEST_SIZE_CAT2      0x0100

BYTE code abTestPattern[8]      = {0x51,0xa2,0x93,0xf0,0x0f,0xff,0x81,0x00};
BYTE code abTestPatternClear[8] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
BYTE code abTestPattern1[8]     = {0x12,0x34,0x56,0x78,0x9a,0xbd,0xde,0xef};


//----------------------------------------------------------------------------
VOID TestRandom(BYTE bCat,WORD wSize)
{
    WORD wAddress;
    BYTE bTemp,bRC;

    i2cSetBusSpeed(I2C_100KHZ);
    i2cSetMemoryType(bCat);

    for(wAddress=0;wAddress < wSize;wAddress++){
        for(bTemp=0;bTemp<TEST_PATTERN_SIZE;bTemp++){
            lcdPutStringXY(0,1,"Address : ");
            lcdPutWord(wAddress);
            lcdPutStringXY(0,2,"Data : ");
            lcdPutByte(abTestPattern[bTemp]);
            i2cWrite(0x00,wAddress,1,&abTestPattern[bTemp]);
            DelaymSecond(5);
            i2cWrite(0x00,wAddress+1,1,&abTestPattern[bTemp+1]);
            DelaymSecond(5);

            i2cRead(0x00,0X0000,1,&bRC);
            i2cRead(0x00,wAddress,1,&bRC);

            if(bRC != abTestPattern[bTemp]){
                lcdPutStringXY(0,3,"ERROR!!");
                lcdPutString("read(");
                lcdPutByte(bRC);
                lcdPutString(")");
                lcdPutString("write(");
                lcdPutByte(abTestPattern[bTemp]);
                lcdPutString(")");
                while(1);
            }
        }
    }
}
//----------------------------------------------------------------------------
VOID TestPage(BYTE bCat,WORD wSize,BYTE bTestPassternSize)
{
    WORD wAddress;
    BYTE bTemp,abRc[0x50];

    i2cSetBusSpeed(I2C_400KHZ);
    i2cSetMemoryType(bCat);

    for(wAddress = 0;wAddress < wSize;wAddress+=bTestPassternSize){
        lcdPutStringXY(0,1,"Address : ");
        lcdPutWord(wAddress);
        i2cWrite(0x00,wAddress,bTestPassternSize,&abTestPatternClear[0]);
        DelaymSecond(5);
        i2cWrite(0x00,wAddress,bTestPassternSize,&abTestPattern[0]);
        DelaymSecond(5);
        for(bTemp=0;bTemp<0x25;bTemp++) abRc[bTemp]=0x00;
        i2cRead(0x00,wAddress,bTestPassternSize,&abRc[0]);
        for(bTemp=0;bTemp<bTestPassternSize;bTemp++){
            if(abRc[bTemp] != abTestPattern[bTemp]){
                lcdPutStringXY(0,3,"ERROR!!");
                lcdPutString("read(");
                lcdPutByte(abRc[bTemp]);
                lcdPutString(")");
                lcdPutString("write(");
                lcdPutByte(abTestPattern[bTemp]);
                lcdPutString(")");
                while(1);
            }
        }
    }
}

//----------------------------------------------------------------------------
VOID TestCat3Random(VOID)
{
    gpioInitialization();
    lcdPutString("Test i2c memory");

    lcdPutString(":Random R/W Test on CAT3");
    TestRandom(I2C_CATEGORY_3,TEST_SIZE_CAT3);
}

//----------------------------------------------------------------------------
VOID TestCat3Page(VOID)
{
    gpioInitialization();
    lcdPutString("Test i2c memory");
    lcdPutString(":Page R/W Test on CAT3");

    TestPage(I2C_CATEGORY_3,TEST_SIZE_CAT3,TEST_PATTERN_SIZE);
}
//----------------------------------------------------------------------------
VOID TestCat2Random(VOID)
{
    gpioInitialization();
    lcdPutString("Test i2c memory");

    lcdPutString(":Random R/W Test on CAT2");
    TestRandom(I2C_CATEGORY_2,TEST_SIZE_CAT2);
}

//----------------------------------------------------------------------------
VOID TestCat2Page(VOID)
{
    gpioInitialization();
    lcdPutString("Test i2c memory");
    lcdPutString(":Page R/W Test on CAT2");
    TestPage(I2C_CATEGORY_2,TEST_SIZE_CAT2,TEST_PATTERN_SIZE);
}

//----------------------------------------------------------------------------
VOID LargePageReadPrepare(BYTE bCat,WORD wSize)
{
    WORD wAddress;
    BYTE bTemp;

    gpioInitialization();
    lcdPutString("Test i2c memory");
    lcdPutString(":Large Page Test");

    i2cSetBusSpeed(I2C_400KHZ);
    i2cSetMemoryType(bCat);

    // write test pattern to i2c
    for(wAddress = 0;wAddress < wSize;wAddress++){
        bTemp = (BYTE)(wAddress & 0xff);
        i2cWrite(0x00,wAddress,1,&bTemp);
        lcdPutStringXY(0,1,"Address : ");
        lcdPutWord(wAddress);
        DelaymSecond(2);
        *(pbExternalRAM+wAddress) = 0x05;
    }
}

//----------------------------------------------------------------------------
VOID LargePageRead(BYTE bCat,WORD wSize)
{
    WORD wAddress;
    BYTE bTemp;

    i2cSetBusSpeed(I2C_400KHZ);
    i2cSetMemoryType(bCat);

    // clear externla memory
     for(wAddress = 0;wAddress < 0x8000;wAddress++){
        *(pbExternalRAM+wAddress) = 0x05;
    }

    lcdPutStringXY(0,2,"Large Page Read to external Memory : ");

    i2cRead(0x00,0x0000,wSize,pbExternalRAM);

    bTemp = 0x00;
    for(wAddress = 0;wAddress < wSize; wAddress++){
        if(*(pbExternalRAM+wAddress) != bTemp){
            lcdPutStringXY(0,3,"Bad Addr: ");
            lcdPutWord(wAddress);
            DelaymSecond(2000);
            lcdExternalMemoryDump(0x0000,4);
            ledPutChar(0x01,bI2CSTA);
            while(1);
        }
        bTemp++;
    }
     lcdExternalMemoryDump(wSize-0x10,4);
     ledPutChar(0x01,bI2CSTA);
     ledPutChar(0x02,bI2CDAI);
}

//----------------------------------------------------------------------------
VOID main(VOID)
{
    DelaymSecond(10);
    gpioInitialization();
//    TestCat3Random();
    TestCat3Page();
//  TestCat2Random();
//    TestCat2Page();

//    LargePageReadPrepare(I2C_CATEGORY_3,0x400);
//    while(1);
//    LargePageRead(I2C_CATEGORY_3,0x1010);

    lcdPutStringXY(0,3,"Done!");
    while(1);
}
#endif
//----------------------------- Cut along the line ----------------------------
