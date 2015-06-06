/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
|
Keyboard Hub Micro−Controller
|
|
Header
|
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Source: header.h, v 1.0 2000/05/28 12:59:29
|
| Author: Horng−Ming Lobo Tai lobotai@ti.com
|
| Header: (none)
|
|
|
| For more information, contact
|
| Lobo Tai
|
| Texas Instruments
|
| 12500 TI BLVD, MS8761
|
| Dallas, TX 75243
|
| USA
|
|
|
| Tel 214−480−3145
|
| Fax 214−480−3443
|
|
|
| Logs:
|
WHEN
WHAT
|
| WHO
| HMT
20000528
born
|
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
#ifndef _HEADER_H_
#define _HEADER_H_
#ifdef __cplusplus
extern ”C”
{
#endif
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Release Notes:
|
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Include files
|
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Function Prototype
|
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
BYTE headerSearchForValidHeader(VOID);
BYTE headerGetDataType(WORD wNumber);
BYTE headerProcessCurrentDataType(VOID);
WORD headerReturnFirmwareRevision(VOID);
BOOL UpdateHeader(WORD wHeaderSize, BYTE bBlockSize, BYTE bWaitTime);
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Type Definition & Macro
|
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
typedef struct _tHeaderPrefix
{
    BYTE
    bDataType;
    BYTE
    bDataSize_L;
    BYTE
    bDataSize_H;
    BYTE
    bDataChecksum;
} tHeaderPrefix, *ptHeaderPrefix;
typedef struct _tFirmwareRevision
{
    BYTE bMinor;
    BYTE bMajor;
} tFirmwareRevision, *ptFirmwareRevision;
typedef struct _tHeaderUsbInfoBasic
{
    BYTE
    bBitSetting;
// Bit 0: Bus/self power in bUSBCRL
// Bit 6: Individual/Gang Power Control
// Bit 7: PWRSW
    BYTE
    bVID_L;
// Vendor ID
    BYTE
    bVID_H;
    BYTE
    bPID_HUB_L;
// Hub Product ID
    BYTE
    bPID_HUB_H;
    BYTE
    bPID_FUNC_L;
// Function Product ID
    BYTE
    bPID_FUNC_H;
    BYTE
    bHubPotg;
// Time from power−on to power−good
    BYTE
    bHubCurt;
// HUB Current descriptor register
} tHeaderUsbInfoBasic, *ptHeaderUsbInfoBasic;
typedef struct _tHeaderFirmwareBasic
{
    BYTE
    bFirmwareRev_L;
// Application Revision
    BYTE
    bFirmwareRev_H;
    PBYTE
    pbFirmwareCode;
} tHeaderFirmwareBasic, *ptHeaderFirmwareBasic;
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Constant Definition
|
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
#define OFFSET_HEADER_SIGNATURE0 0x00
#define OFFSET_HEADER_SIGNATURE1 0x01
#define OFFSET_HEADER_FIRST_DATA_SECTION 0x02
#define DATA_TYPE_HEADER_END 0x00
#define DATA_TYPE_HEADER_HUB_INFO_BASIC 0x01
#define DATA_TYPE_HEADER_FIRMWARE_BASIC 0x02
#define DATA_TYPE_HEADER_RESERVED 0xFF
#define BIT_HEADER_PWRSW 0x80 // Hub Power Switching
#define BIT_HEADER_IG 0x40 // Hub Power Ind or Group
#define BIT_HEADER_BSPWR 0x01 // Bus or Self Powered
#define DATA_MEDIUM_HEADER_NO 0x00
#define DATA_MEDIUM_HEADER_I2C 0x01
#define DATA_MEDIUM_HEADER_FLASH 0x02
#define DATA_MEDIUM_HEADER_ROM 0x03
#define DATA_MEDIUM_HEADER_RAM 0x04
#define MSG_HEADER_NO_ERROR 0x00
#define MSG_HEADER_CHECKSUM_ERROR 0x01
#define MSG_HEADER_DATA_TYPE_ERROR 0x02
#define MSG_HEADER_DATA_MEDIUM_ERROR 0x03
#ifdef __cplusplus
}
#endif
#endif /* _HEADER_H_ */
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−− Cut along the line −−−−−−−−−−−−−−−−−−−−−−−−−−−−