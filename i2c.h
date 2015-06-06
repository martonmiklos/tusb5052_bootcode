/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
|                             Texas Instruments                               |
|                               I2C Header File                               |
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
|  Source: i2c.h, v 1.0 1999/11/24 16:01:49                                   |
|  Author: Horng−Ming Lobo Tai lobotai@ti.com                                 |
|  Header: (none)                                                             |
|                                                                             |
|  For more information, contact                                              |
|  Lobo Tai                                                                   |
|  Texas Instruments                                                          |
|  12500 TI Blvd, MS 8761                                                     |
|  Dallas, TX 75243                                                           |
|  USA                                                                        |
|  Tel 214−480−3145                                                           |
|  Fax 214−480−3443                                                           |
|                                                                             |
|  Logs:                                                                      |
|  WHO     WHEN         WHAT                                                  |
|  HMT     19991124     born                                                  |
|  HMT     20000614     revised function calls and cat i,ii and iii.          |
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
#ifndef _I2C_H_
#define _I2C_H_
#ifdef __cplusplus
extern "C"
{
#endif
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Release Notes:                                                              |
|  If no error occurs, return NO_ERROR(=0).                                   |
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Include files                                                               |
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
// If no error occurs, return NO_ERROR(=0).
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Function Prototype                                                          |
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
VOID i2cSetBusSpeed(BYTE bBusSpeed);
VOID i2cSetMemoryType(BYTE bType);
BYTE i2cRead(BYTE bDeviceAddress, WORD wAddress, WORD wNumber, PBYTE pbDataArray);
BYTE i2cWrite(BYTE bDeviceAddress, WORD wAddress, WORD wNumber, PBYTE pbDataArray);
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Type Definition & Macro                                                     |
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Constant Definition                                                         |
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
#define I2C_DEVICE_ADDRESS_DEFAULT 0
#define I2C_100KHZ          0
#define I2C_400KHZ          1
#define I2C_CATEGORY_1      1
#define I2C_CATEGORY_2      2
#define I2C_CATEGORY_3      3
#define I2C_CATEGORY_LAST   3
#define BIT_I2C_READ        1
#define BIT_I2C_DEVICE_TYPE_MEMORY  0xA0
#define MASK_I2C_DEVICE_ADDRESS     0x07
#ifdef __cplusplus
}
#endif
#endif
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−− Cut along the line −−−−−−−−−−−−−−−−−−−−−−−−−−−−
