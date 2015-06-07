/*----------------------------------------------------------------------------+
|                                                                             |
|                             Texas Instruments                               |
|                                                                             |
|                               Type definition                               |
|                                                                             |
+-----------------------------------------------------------------------------+
|  Source: types.h v00.01 1999/01/26 14:34:34                                 |
|  Author: Horng-Ming Lobo Tai lobotai@ti.com                                 |
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
|  Release Notes: (none)                                                      |
|                                                                             |
|  Logs:                                                                      |
|                                                                             |
|  WHEN     WHO   WHAT                                                        |
|  -------- ----- -----------------------------------------------------       |
|  19990126 HMT   born                                                        |
|                                                                             |
+----------------------------------------------------------------------------*/
#ifndef _TYPES_H_
#define _TYPES_H_

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
typedef char            CHAR;
typedef unsigned char   UCHAR;
typedef int             INT;
typedef unsigned int    UINT;
typedef short           SHORT;
typedef unsigned short  USHORT;
typedef long            LONG;
typedef unsigned long   ULONG;
typedef void            VOID;
typedef unsigned long   HANDLE;
typedef char*           PSTR;
typedef int             BOOL;
typedef float           DOUBLE;
typedef unsigned char   BYTE;
typedef unsigned char*  PBYTE;
typedef unsigned int    WORD;
typedef unsigned long   DWORD;

/*----------------------------------------------------------------------------+
| Constant Definition                                                         |
+----------------------------------------------------------------------------*/
#define YES 1
#define NO  0

#define TRUE    1
#define FALSE   0

#define NOERR   0
#define ERR 1

#define NO_ERROR    0
#define ERROR       1

#define DISABLE     0
#define ENABLE      1

/*----------------------------------------------------------------------------+
| End of header file                                                          |
+----------------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif /* _TYPES_H_ */
/*------------------------ Nothing Below This Line --------------------------*/
