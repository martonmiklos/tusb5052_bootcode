/*----------------------------------------------------------------------------+
|                                                                             |
|                             Texas Instruments                               |
|                                                                             |
|                            Bootcode Header File                             |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: bootcode.h v00.01 2000/06/13 16:02:35                              |
|  Author: Horng-Ming Lobo Tai lobotai@ti.com                                 |
|  Header: (none)                                                             |
|                                                                             |
|  For more information, please contact                                       |
|                                                                             |
|  Lobo Tai                                                                   |
|  Texas Instruments                                                          |
|  12500 TI BLVD, MS 8761                                                     |
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
|  HMT     20000613     born                                                  |
|                                                                             |
+----------------------------------------------------------------------------*/

#ifndef _BOOTCODE_H_
#define _BOOTCODE_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*----------------------------------------------------------------------------+
| Include files                                                               |
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| Function Prototype                                                          |
+----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------+
| Type Definition & Macro                                                     |
+----------------------------------------------------------------------------*/
typedef enum
{
    // firmware
    USB_VENDOR_REQ_FIRST      = 0x80,
    USB_VENDOR_REQ_GET_STATUS = 0x80,
    USB_VENDOR_REQ_EXECUTE_FIRMARE,
    USB_VENDOR_REQ_GET_FIRMWARE_VERSION,
    USB_VENDOR_REQ_PREPARE_FOR_UPDATE_HEADER,
    USB_VENDOR_REQ_UPDATE_HEADER,
    USB_VENDOR_REQ_REBOOT,
    USB_VENDOR_REQ_FORCE_EXECUTE_FIRMWARE = 0x8f,

    // memory features
    USB_VENDOR_REQ_EXTERNAL_MEMORY_READ = 0x90,
    USB_VENDOR_REQ_EXTERNAL_MEMORY_WRITE,
    USB_VENDOR_REQ_I2C_MEMORY_READ,
    USB_VENDOR_REQ_I2C_MEMORY_WRITE,
    USB_VENDOR_REQ_INTERNAL_ROM_MEMORY_READ,

    USB_VENDOR_REQ_EXTERNAL_MEMORY_BULK, // 0x95
    USB_VENDOR_REQ_HEADER_DOWNLOAD_SETTING,

    USB_VENDOR_REQ_LAST,

    // for debuggin only firmware
    USB_VENDOR_REQ_GET_CURRENT_CHECKSUM = 0xe0,
    USB_VENDOR_REQ_GET_DOWNLOAD_SIZE,
    USB_VENDOR_REQ_SET_DOWNLOAD_SIZE_AND_CHECKSUM,

    // for debuggin only lcd display
    USB_VENDOR_REQ_ROM_ADDRESS_DUMP = 0xf0,
    USB_VENDOR_REQ_EXTERNAL_DUMP,
    USB_VENDOR_REQ_I2C_DUMP

}
tUSB_VENDOR_REQ_LIST;

/*----------------------------------------------------------------------------+
| Constant Definition                                                         |
+----------------------------------------------------------------------------*/
// TUSB3410 VID and PID Definition
//#define FUNCTION_VID_L  0x51
//#define FUNCTION_VID_H  0x04
//#define FUNCTION_PID_L  0x10
//#define FUNCTION_PID_H  0x34

#define BIT_BOOTCODE_STATUS_BUSY            0x01

#define SIZEOF_BOOTCODE_CONFIG_DESC_GROUP   SIZEOF_CONFIG_DESCRIPTOR+SIZEOF_INTERFACE_DESCRIPTOR+SIZEOF_ENDPOINT_DESCRIPTOR
#define SIZEOF_BOOTCODE_STRING_DESC_GROUP   116
/*----------------------------------------------------------------------------+
| End of header file                                                          |
+----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif /* _BOOTCODE_H_ */
/*------------------------ Nothing Below This Line --------------------------*/



