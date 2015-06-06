/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
|                             Texas Instruments                               |
|                            TUSB5052 Header File                             |
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
|  Source: tusb5052.h, v 1.0 2000/01/21 15:12:03                              |
|  Author: Horng−Ming Lobo Tai lobotai@ti.com                                 |
|  Header: (none)                                                             |
|                                                                             |
|  For more information, contact                                              |
|                                                                             |
|  Lobo Tai                                                                   |
|  Texas Instruments                                                          |
|  8505 Forest Lane, MS 8761                                                  |
|  Dallas, TX 75243                                                           |
|  USA                                                                        |
|  Tel 972−480−3145                                                           |
|  Fax 972−480−3443                                                           |
|                                                                             |
|  Logs:                                                                      |
|  WHO     WHEN         WHAT                                                  |
|  HMT     20000121     born                                                  |
|  HMT     20000228     achange USBCTL, add USBCNF1, USBCNF2 and HUBPOTG      |
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
#ifndef _TUSB5052_H_
#define _TUSB5052_H_
#ifdef __cplusplus
extern "C"
{
#endif
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
| Release Notes:                                                              |
+−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
                | Include files                                                               |
                +−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
                | Function Prototype                                                          |
                +−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
                | Type Definition & Macro                                                     |
                +−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
// EDB Data Structure
typedef struct _tEDB
{
    BYTE    bEPCNF;             // Endpoint Configuration
    BYTE    bEPBBAX;            // Endpoint X Buffer Base Address
    BYTE    bEPBCTX;            // Endpoint X Buffer byte Count
    BYTE    bSPARE0;            // no used
    BYTE    bSPARE1;            // no used
    BYTE    bEPBBAY;            // Endpoint Y Buffer Base Address
    BYTE    bEPBCTY;            // Endpoint Y Buffer byte Count
    BYTE    bEPSIZXY;           // Endpoint XY Buffer Size
} tEDB, *tpEDB;
typedef struct _tEDB0
{
    BYTE    bIEPCNFG;           // Input Endpoint 0 Configuration Register
    BYTE    bIEPBCNT;           // Input Endpoint 0 Buffer Byte Count
    BYTE    bOEPCNFG;           // Output Endpoint 0 Configuration Register
    BYTE    bOEPBCNT;           // Output Endpoint 0 Buffer Byte Count
} tEDB0, *tpEDB0;
/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−+
                        | Constant Definition                                                         |
                        +−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
#define SIZEOF_BOOTCODE_CONFIG_DESC_GROUP SIZEOF_CONFIG_DESCRIPTOR+SIZEOF_INTERFACE_DESCRIPTOR+SIZEOF_ENDPOINT_DESCRIPTOR
// Power Control Register (@ SFR 87h)
// PCON            0x87    // sfr 0x87
#define PCON_IDL        0x01    // MCU idle bit
// 0: MCU NOT in idle, 1:MCU idle
#define PCON_GF0        0x04    // General purpose bit
#define PCON_GF1        0x08    // General purpose bit
#define PCON_SMOD       0x80    // Double baud rate control bit
// External Memory Pointer
// don’t use this one because it is not efficient in the binary code.
// char *pbExternalRAM = (char *)0x010000;
#define pbExternalRAM  ((char xdata *)0x0000)    // use this for the future design
#define pbInternalROM  ((char code *)0x0000)

// TUSB5052 VID and PID Definition
#define HUB_VID_L       0x51        // TI = 0x0451
#define HUB_VID_H       0x04
#define HUB_PID_L       0x45
#define HUB_PID_H       0x31

#define FUNCTION_PID_L  0x52        // TUSB5052
#define FUNCTION_PID_H  0x51
// USB related Constant
#define MAX_ENDPOINT_NUMBER     0x07
#define EP0_MAX_PACKET_SIZE     0x08
#define EP_MAX_PACKET_SIZE      0x40

// on−board I/O address are c00x. This is the same as f80x.
#define IEP1_X_BUFFER_ADDRESS   0xF800  // Input  Endpoint 1 X Buffer Base−address
#define IEP1_Y_BUFFER_ADDRESS   0xF840  // Input  Endpoint 1 Y Buffer Base−address
#define OEP1_X_BUFFER_ADDRESS   0xF880  // Output Endpoint 1 X Buffer Base−address
#define OEP1_Y_BUFFER_ADDRESS   0xF8C0  // Output Endpoint 1 Y Buffer Base−address
#define IEP2_X_BUFFER_ADDRESS   0xF900  // Input  Endpoint 2 X Buffer Base−address
#define IEP2_Y_BUFFER_ADDRESS   0xF940  // Input  Endpoint 2 Y Buffer Base−address
#define OEP2_X_BUFFER_ADDRESS   0xF980  // Output Endpoint 2 X Buffer Base−address
#define OEP2_Y_BUFFER_ADDRESS   0xF9C0  // Output Endpoint 2 Y Buffer Base−address
#define IEP3_X_BUFFER_ADDRESS   0xFA00  // Input  Endpoint 3 X Buffer Base−address
#define IEP3_Y_BUFFER_ADDRESS   0xFA40  // Input  Endpoint 3 Y Buffer Base−address
#define OEP3_X_BUFFER_ADDRESS   0xFA80  // Output Endpoint 3 X Buffer Base−address
#define OEP3_Y_BUFFER_ADDRESS   0xFAC0  // Output Endpoint 3 Y Buffer Base−address
#define IEP4_X_BUFFER_ADDRESS   0xFB00  // Input  Endpoint 4 X Buffer Base−address
#define IEP4_Y_BUFFER_ADDRESS   0xFB40  // Input  Endpoint 4 Y Buffer Base−address
#define OEP4_X_BUFFER_ADDRESS   0xFB80  // Output Endpoint 4 X Buffer Base−address
#define OEP4_Y_BUFFER_ADDRESS   0xFBC0  // Output Endpoint 4 Y Buffer Base−address

#define IEP5_X_BUFFER_ADDRESS   0xFC00  // Input  Endpoint 5 X Buffer Base−address
#define IEP5_Y_BUFFER_ADDRESS   0xFC40  // Input  Endpoint 5 Y Buffer Base−address
#define OEP5_X_BUFFER_ADDRESS   0xFC80  // Output Endpoint 5 X Buffer Base−address
#define OEP5_Y_BUFFER_ADDRESS   0xFCC0  // Output Endpoint 5 Y Buffer Base−address
#define IEP6_X_BUFFER_ADDRESS   0xFD00  // Input  Endpoint 6 X Buffer Base−address
#define IEP6_Y_BUFFER_ADDRESS   0xFD40  // Input  Endpoint 6 Y Buffer Base−address
#define OEP6_X_BUFFER_ADDRESS   0xFD80  // Output Endpoint 6 X Buffer Base−address
#define OEP6_Y_BUFFER_ADDRESS   0xFDC0  // Output Endpoint 6 Y Buffer Base−address
#define IEP7_X_BUFFER_ADDRESS   0xFE00  // Input  Endpoint 7 X Buffer Base−address
#define IEP7_Y_BUFFER_ADDRESS   0xFE20  // Input  Endpoint 7 Y Buffer Base−address
#define OEP7_X_BUFFER_ADDRESS   0xFE40  // Output Endpoint 7 X Buffer Base−address
#define OEP7_Y_BUFFER_ADDRESS   0xFE60  // Output Endpoint 7 Y Buffer Base−address
// Miscellaneous Registers
#define ROMS_SDW        0x01
#define ROMS_R0         0x02   // Revision Number R[3:0]
#define ROMS_R1         0x04
#define ROMS_R2         0x08
#define ROMS_R3         0x10
#define ROMS_S0         0x20   // Code Size S[1:0]
#define ROMS_S1         0x40   // 00: 4K, 01:8k, 10:16k, 11:32k
#define ROMS_ROA        0x80   // Code Space 0:in ROM, 1:in RAM
#define GLOBCTL_LPWR    0x04   // Low Power Bit Mode, 0:Low Power, 1:Normal Operation
#define GLOBCTL_RSTP    0x08   // Reset Ouput Polarity 0:Active Low, 1:Active High
#define GLOBCTL_C0      0x20   // Output Clock Frequency Select, C[1:0]
#define GLOBCTL_C1      0x40   // 00:48Mhz, 01:24Mhz, 10:12Mhz, 11:6Mhz
#define GLOBCTL_24MHZ   0x80   // Clock Slect for MCU, 0:12Mhz, 1:24Mhz
// EndPoint Desciptor Block
#define EPCNF_USBIE     0x04   // USB Interrupt on Transaction Completion. Set By MCU
// 0:No Interrupt, 1:Interrupt on completion
#define EPCNF_STALL     0x08   // USB Stall Condition Indication. Set by UBM
// 0: No Stall, 1:USB Install Condition
#define EPCNF_DBUF      0x10   // Double Buffer Enable. Set by MCU
// 0: Primary Buffer Only(x−buffer only), 1:Toggle Bit
// Selects Buffer
#define EPCNF_TOGLE     0x20   // USB Toggle bit. This bit reflects the toggle sequence bit
// of DATA0 and DATA1.
#define EPCNF_ISO       0x40   // ISO=0, Non Isochronous transfer. This bit must be cleared
// by MCU since only non isochronous transfer is supported.
#define EPCNF_UBME      0x80   // UBM Enable or Disable bit. Set or Clear by MCU.
// 0:UBM can’t use this endpoint
// 1:UBM can use this endpoint
#define EPBCT_BYTECNT_MASK 0x7F // MASK for Buffer Byte Count
#define EPBCT_NAK       0x80   // NAK, 0:No Valid in buffer, 1:Valid packet in buffer
// Endpoint 0 Descriptor Registers
#define IEPBCNT_NAK     0x80   // NAK bit
// 0:buffer contains valid data
// 1:buffer is empty
// USB Registers
#define USBSTA_STPOW    0x01   // Setup Overwrite Bit. Set by hardware when setup
// packet is received
// while there is already a packet in the setup buffer.
// 0:Nothing, 1:Setup Overwrite
#define USBSTA_RWUP     0x02   // Remote wakeup pin status
// 0:Nothing, 1:Remote wakeup request
#define USBSTA_SETUP    0x04   // Setup Transaction Received Bit. As long as SETUP is ’1’,
// IN and OUT on endpoint−0 is NAKed regardless
// of their real NAK bits values.
#define USBSTA_UART1    0x08    // Uart 1 Ring Indicator
// 0: no ring coming, 1:ring coming
#define USBSTA_UART2    0x10    // Uart 2 Ring Indicator
// 0: no ring coming, 1:ring coming
#define USBSTA_RESR     0x20    // Function Resume Request Bit. 0:clear by MCU,
// 1:Function Resume is detected.
#define USBSTA_SUSR     0x40    // Function Suspended Request Bit. 0:clear by MCU,
// 1:Function Suspend is detected.
#define USBSTA_RSTR     0x80    // Function Reset Request Bit. This bit is set in
// response to a global or selective suspend condition.
// 0:clear by MCU, 1:Function reset is detected.
#define USBMSK_STPOW    0x01    // Setup Overwrite Interrupt Enable Bit
// 0: disable, 1:enable
#define USBMSK_RWUP     0x02    // Remote wakeup Interrupt Enable Bit
// 0: disable, 1:enable
#define USBMSK_SETUP    0x04    // Setup Interrupt Enable Bit
// 0: disable, 1:enable
#define USBMSK_UART1    0x08    // Uart1 RI Interrupt Enable Bit
// 0: disable, 1:enable
#define USBMSK_UART2    0x10    // Uart2 RI Interrupt Enable Bit
// 0: disable, 1:enable
#define USBMSK_RESR     0x20    // Function Resume Interrupt Enable Bit
// 0: disable, 1:enable
#define USBMSK_SUSR     0x40    // Function Suspend Interrupt Enable Bit
// 0: disable, 1:enable
#define USBMSK_RSTR     0x80    // Function Reset Interrupt Enable Bit
// 0: disable, 1:enable
#define USBCTL_DIR      0x01    // USB traffic direction 0: USB out packet, 1:in
// packet (from TUSB5052 to The host)
#define USBCTL_SIR      0x02    // Setup interrupt status bit
// 0: SETUP interrupt is not served.
// 1: SETUP interrupt in progess
#define USBCTL_SELF     0x04    // Bus/self powered bit
// 0: bus, 1:self
#define USBCTL_FRSTE    0x10    // Function Reset Condition Bit.
// This bit connects or disconnects the USB
//  Function Reset from the MCU reset
// 0:not connect, 1:connect
#define USBCTL_RWUP     0x20    // Remote wakeup request
#define USBCTL_HUBV     0x40    // USB hub version Read only
// 0:1.x hub, 1:2.x hub
#define USBCTL_CONT     0x80    // Connect or Disconnect Bit
// 0:Upstream port is disconnected. Pull−up disabled
// 1:Upstream port is connected. Pull−up enabled
#define HUBCNF1_P1E     0x01    // Hub Port 1 enable/disable control bit
// 0: disable, 1:enable
#define HUBCNF1_P1A     0x02    // Hub Port 1 connection bit
// 0: removable, 1:fixed
#define HUBCNF1_P2E     0x04    // Hub Port 2 enable/disable control bit
// 0: disable, 1:enable
#define HUBCNF1_P2A     0x08    // Hub Port 2 connection bit
// 0: removable, 1:fixed
#define HUBCNF1_P3E     0x10    // Hub Port 3 enable/disable control bit
// 0: disable, 1:enable
#define HUBCNF1_P3A             0x20    // Hub Port 3 connection bit
// 0: removable, 1:fixed
#define HUBCNF1_P4E             0x40    // Hub Port 4 enable/disable control bit
// 0: disable, 1:enable
#define HUBCNF1_P4A             0x80    // Hub Port 4 connection bit
// 0: removable, 1:fixed
#define HUBCNF2_P5E             0x01    // Hub Port 5 enable/disable control bit
// 0: disable, 1:enable
#define HUBCNF2_P5A             0x02    // Hub Port 5 connection bit
// 0: removable, 1:fixed
#define HUBCNF2_P6_NONE         0x00    // Hub Port 6: no function
#define HUBCNF2_P6_EN           0x10    // Hub Port 6: fixed attach, enable (normal
// operation)
#define HUBCNF2_P6_NOT_ATTACH   0x20    // Hub Port 6: removable, NOT attach
#define HUBCNF2_P6_ATTACH       0x30    // Hub Port 6: attached, removable
#define HUBCNF2_P6_MASK         0x30
#define HUBCNF2_PWRSW           0x80    // Power Switch controle bit
// 0: not available, 1:available
#define HUBPOTG_100MS           0x32    // power−on to power−good time is 100ms ( in 2ms increment)
#define HUBCURT_100MA           0x64    // hub requires 100mA
// DMA Control Registers
#define DMA_BASE_ADDRESS        0xFFE0  // all DMA register starts at this address
#define DMACDR_ENDPOINT_MASK    0x07    // Endpoint Select Mask
#define DMACDR_ENDPOINT1        0x01    // Select Endpoint 1
#define DMACDR_ENDPOINT2        0x02    // Select Endpoint 2
#define DMACDR_ENDPOINT3        0x03    // Select Endpoint 3
#define DMACDR_ENDPOINT4        0x04    // Select Endpoint 4
#define DMACDR_ENDPOINT5        0x05    // Select Endpoint 5
#define DMACDR_ENDPOINT6        0x06    // Select Endpoint 6
#define DMACDR_ENDPOINT7        0x07    // Select Endpoint 7
#define DMACDR_TR               0x08    // DMA Direction (not used in UMP)
// 0:out, 1:in
#define DMACDR_XY               0x10    // XY Buffer Select (valid only when CNT=0)
// 0:X buffer 1:Y buffer
#define DMACDR_CNT              0x20    // DMA Continuous Transfer Control bit
// 0:Burst Mode, 1:Continuos Mode
#define DMACDR_INE              0x40    // DMA Interrupt Enable or Disable bit.
// 0:disable, 1:enable
#define DMACDR_EN               0x80    // DMA Channel Enable
// 0:disable, 1:enable
#define DMACSR_OVRUN            0x01    // Overrun Condition Bit. Set by DMA and
// Cleared by MCU
// 0: no overrun, 1:overrun
#define DMACSR_PPKT             0x01    // Overrun Condition Bit. Set by DMA and
// Cleared by MCU
// 0: no overrun(no partial packet), 1:overrun
#define DMACSR_TXFT             0x02    // Transfer Timeout Condition. Cleared by MCU
// 0: no timeout, 1:timeout
#define DMACSR_TIMEOUT_MASK     0x7C    // Select Timeout Value
#define DMACSR_TIMEOUT_1MS      0x00    // Select Timeout 1ms Value
#define DMACSR_TIMEOUT_2MS      0x04    // Select Timeout 2ms Value
#define DMACSR_TIMEOUT_3MS      0x08    // Select Timeout 3ms Value
#define DMACSR_TIMEOUT_4MS      0x0C    // Select Timeout 4ms Value
#define DMACSR_TIMEOUT_5MS      0x10    // Select Timeout 5ms Value
#define DMACSR_TIMEOUT_6MS        0x14    // Select Timeout 6ms Value
#define DMACSR_TIMEOUT_7MS        0x18    // Select Timeout 7ms Value
#define DMACSR_TIMEOUT_8MS        0x1C    // Select Timeout 8ms Value
#define DMACSR_TIMEOUT_9MS        0x20    // Select Timeout 9ms Value
#define DMACSR_TIMEOUT_10MS       0x24    // Select Timeout 10ms Value
#define DMACSR_TIMEOUT_11MS       0x28    // Select Timeout 11ms Value
#define DMACSR_TIMEOUT_12MS       0x2C    // Select Timeout 12ms Value
#define DMACSR_TIMEOUT_13MS       0x30    // Select Timeout 13ms Value
#define DMACSR_TIMEOUT_14MS       0x34    // Select Timeout 14ms Value
#define DMACSR_TIMEOUT_15MS       0x38    // Select Timeout 15ms Value
#define DMACSR_TIMEOUT_16MS       0x3C    // Select Timeout 16ms Value
#define DMACSR_TIMEOUT_17MS       0x40    // Select Timeout 17ms Value
#define DMACSR_TIMEOUT_18MS       0x44    // Select Timeout 18ms Value
#define DMACSR_TIMEOUT_19MS       0x48    // Select Timeout 19ms Value
#define DMACSR_TIMEOUT_20MS       0x4C    // Select Timeout 20ms Value
#define DMACSR_TIMEOUT_21MS       0x50    // Select Timeout 21ms Value
#define DMACSR_TIMEOUT_22MS       0x54    // Select Timeout 22ms Value
#define DMACSR_TIMEOUT_23MS       0x58    // Select Timeout 23ms Value
#define DMACSR_TIMEOUT_24MS       0x5C    // Select Timeout 24ms Value
#define DMACSR_TIMEOUT_25MS       0x60    // Select Timeout 25ms Value
#define DMACSR_TIMEOUT_26MS       0x64    // Select Timeout 26ms Value
#define DMACSR_TIMEOUT_27MS       0x68    // Select Timeout 27ms Value
#define DMACSR_TIMEOUT_28MS       0x6C    // Select Timeout 28ms Value
#define DMACSR_TIMEOUT_29MS       0x70    // Select Timeout 29ms Value
#define DMACSR_TIMEOUT_30MS       0x74    // Select Timeout 30ms Value
#define DMACSR_TIMEOUT_31MS       0x78    // Select Timeout 31ms Value
#define DMACSR_TIMEOUT_32MS       0x7C    // Select Timeout 32ms Value
#define DMACSR_TEN                0x80    // Transaction Timeout Conouter Enable
// or Disable Bit
// 0:disable(no timeout) 1:enable
// UART
#define UART_PORT_1               0x01
#define UART_PORT_2               0x02
#define TOTAL_NUMBER_UART_PORT    0x02
#define UART_BASE_ADDRESS         0xFFA0  // UART base address
// Baud Rate
#define BaudRate50_DLL            0x0F
#define BaudRate50_DLH            0x24
#define BaudRate75_DLL            0x0A
#define BaudRate75_DLH            0x18
#define BaudRate110_DLL           0x64
#define BaudRate110_DLH           0x10
#define BaudRate135_DLL           0x68
#define BaudRate135_DLH           0x0D
#define BaudRate150_DLL           0x05
#define BaudRate150_DLH           0x0C
#define BaudRate300_DLL           0x02
#define BaudRate300_DLH           0x06
#define BaudRate600_DLL           0x01
#define BaudRate600_DLH           0x03
#define BaudRate1200_DLL          0x81
#define BaudRate1200_DLH          0x01
#define BaudRate1800_DLL          0x00
#define BaudRate1800_DLH          0x01
#define BaudRate2000_DLL          0xE7
#define BaudRate2000_DLH            0x00
#define BaudRate2400_DLL            0xC0
#define BaudRate2400_DLH            0x00
#define BaudRate3600_DLL            0x80
#define BaudRate3600_DLH            0x00
#define BaudRate4800_DLL            0x60
#define BaudRate4800_DLH            0x00
#define BaudRate7200_DLL            0x40
#define BaudRate7200_DLH            0x00
#define BaudRate9600_DLL            0x30
#define BaudRate9600_DLH            0x00
#define BaudRate19200_DLL           0x18
#define BaudRate19200_DLH           0x00
#define BaudRate38400_DLL           0x0C
#define BaudRate38400_DLH           0x00
#define BaudRate57600_DLL           0x08
#define BaudRate57600_DLH           0x00
#define BaudRate115200_DLL          0x04
#define BaudRate115200_DLH          0x00
#define BaudRate230400_DLL          0x02
#define BaudRate230400_DLH          0x00
#define BaudRate460800_DLL          0x01
#define BaudRate460800_DLH          0x00
#define LCR_WL_MASK                 0x03    // Word Length Mask
#define LCR_WL_5BIT                 0x00    // 5bit work length
#define LCR_WL_6BIT                 0x01    // 6bit work length
#define LCR_WL_7BIT                 0x02    // 7bit work length
#define LCR_WL_8BIT                 0x03    // 8bit work length
#define LCR_STP                     0x40    // Stop Bits
// 0:1 stop bit, 1:1.5 or 2 stop bits
#define LCR_PRTY                    0x08    // Parity Bit
// 0:no parity, 1:parity bit is used.
#define LCR_EPRTY                   0x10    // Odd Even Parity Bit
// 0:odd, 1:even
#define LCR_FPTY                    0x20    // Force Parity Bit Slect
// 0:not forced, 1:parity bit forced
#define LCR_BRK                     0x40    // Break Controll Bit
// 0:normal operation, 1:forces SOUT
// into break condition(logic 0)
#define LCR_FEN                     0x80    // FIFO Enable
// 0: FIFO cleared and disable, 1: enable
#define FCRL_TXOF                   0x01    // Transmitter Xon Xoff flow control
// 0:disable, 1:enable
#define FCRL_TXOA                   0x02    // Xon−on−any Xoff flow control
// 0:disable, 1:enable
#define FCRL_CTS                    0x04    // Transmitter CTS* Flow Control Enable Bit
// 0:disable, 1:enable
#define FCRL_DSR                    0x04    // Transmitter DSR* Flow Control Enable Bit
// 0:disable, 1:enable
#define FCRL_RXOF                   0x10    // Receiver Xon Xoff flow control
// 0:disable, 1:enable
#define FCRL_RTS                    0x20    // Receiver RTS* Flow Controller Enable Bit
// 0:disable, 1:enable
#define FCRL_DTR                    0x40    // Receiver DTR* Flow Controller Enable Bit
// 0:disable, 1:enable
#define FCRL_485E              0x80    // RS485 enable bit
// 0:normal, full duplex, 1:for RS485
#define MCR_URST               0x01    // UART Soft Reset
// 0:Mornal operation, 1:UART Reset
#define MCR_RCVE               0x02    // receiver enable bit
// 0:disable, 1:enable
#define MCR_LOOP               0x04    // Normal Loopback Mode Select
// 0:normal operation 1:enable loopback mode
#define MCR_IEN                0x08    // Global UART Interrupt Enable Bit
// 0:disable, 1:enable
#define MCR_DTR                0x10    // Set DTR*
// 0:set DTR* high, 1:set DTR* low
#define MCR_RTS                0x20    // Set RTS*
// 0:set RTS* high, 1:set RTS* low
#define MCR_LRI                0x40    // Used in loop−back mode only.
// 0: MSR[6]=0, 1:MSR[6]=1
#define MCR_LCD                0x40    // Used in loop−back mode only.
// 0: MSR[7]=0, 1:MSR[7]=1
#define LSR_OVR                0x01    // Overrun Condition
// 0:no overrun, 1:overrun
#define LSR_PTE                0x02    // Parity Condition
// 0:no parity error, 1:parity error
#define LSR_FRE                0x04    // Framing Condition
// 0:no frame error, 1:frame error
#define LSR_BRK                0x08    // Break Condition
// 0:no break condition, 1:break condition
#define LSR_RXF                0x10    // Receiver Data Register Condition
// 0:no data 1:has new byte coming in
#define LSR_TXE                0x20    // Transmitter Data Register Condition
// 0:not empty 1:empty
#define LSR_TMT                0x40    // Receiver Timeout Indication
// 0:not timeout 1:timeout
#define LSR_EXIT               0x80    // UART Sleep−Exit Indication
// 0:no sleep to normal transition occurred
// 1:a transition from sleep to normal mode occurred
#define MSR_dCTS               0x01    // CTS* State Changed
// 0:no changed 1:changed
#define MSR_dDSR               0x02    // DSR* State Changed
// 0:no changed 1:changed
#define MSR_TRI                0x04    // Trailing edge of the ring−indicator.
// Indicate RI* pin changed from 0 to 1
// 0:RI* is high, 1:RI* pin changed
#define MSR_dCD                0x08    // CD* State Changed. Cleared by MCU Reading MSR
// 0:no changed 1:changed
#define MSR_LCTS               0x10    // During loopback mode, this reflects MCR[1]
// 0:CTS* is low, 1:CTS* is high
#define MSR_LDSR               0x20    // During loopback mode, this reflects MCR[0]
// 0:LDSR is high, 1:LDSR is low
#define MSR_LRI                0x40    // During loopback mode, this reflects MCR[2]
// 0:RI* is high, 1:RI* is low
#define MSR_LCD                0x80    // During loopback mode, this reflects MCR[3]
// 0:CD* is high, 1:CD* is low
#define MASK_MIE               0x01    // Modem interrupt
#define MASK_SIE               0x02    // status interrupt
#define MASK_TRIE                  0x04    // TxRx interrupt
// IEEE 1284 Registers
#define PPMCR_MODE_SLECT_MASK      0x03    // Mode Select Mask
#define PPMCR_CENTRONICS_MODE      0x00    // Select Centronics Mode
#define PPMCR_SPP_MODE             0x01    // Select SPP Mode
#define PPMCR_ECP_MODE             0x02    // Select ECP Mode
#define PPMCR_EPP_MODE             0x03    // Select EEP Mode
#define PPMCR_TEST_MODE            0x04    // Select EEP Mode
#define PPMCR_DIR                  0x10    // Port Direction
// 0:output, 1:input
#define PPIMSK_ERXF                0x01    // RXF Interrupt Enable
// 0:disable, 1:enable
#define PPIMSK_ETXE                0x02    // TXE Interrupt Enable
// 0:disable, 1:enable
#define PPIMSK_FLINE               0x08    // FALT Interrupt Enable
// 0:disable, 1:enable
#define PPSTA_RXF                  0x01    // Receiver FIFO Full Indication
// 0:below threshold, 1:above threshold, ready
// for MCU read
#define PPSTA_TXE                  0x02    // Transmitter FIFO Empty Indication
// 0:above threshold, 1:below threshold, ready
// for MCU write
#define PPSTA_TRC                  0x04    // PP termination completion indication
// 0:normal 1:termination occurred.
#define PPSTA_FALT                 0x08    // Correspond to inverse of FALT* Input Signal
// 0:No Falut Condition, 1:Fault Condition
#define PPSTA_SLCT                 0x10    // Correspond to SCLT Input Signal
#define PPSTA_PER                  0x20    // Correspond to PER Input Signal
#define PPSTA_ACK                  0x40    // Correspond to inverse of ACK* Input Signal
#define PPSTA_BUSY                 0x80    // Correspond to BUSY Input Signal
#define PPCTL_STB                  0x01    // Set STB* Output Pin
// 0: set STB* high, 1:set STB* low
#define PPCTL_AFD                  0x02    // Set AFD* Output Pin
// 0: set AFD* high, 1:set AFD* low
#define PPCTL_INIT                 0x04    // Set INIT* Output Pin
// 0: set INIT* high, 1:set INIT* low
#define PPCTL_SLIN                 0x08    // Set SLIN* Output Pin
// 0: set SLIN* high, 1:set SLIN* low
#define IEEE1284_TIMEOUT           5000
#define IEEE1284_COMPATIBILITY_MODE_OUTPUT  0x80    // default output value
#define IEEE1284_REQUEST_EXTENSIBILITY_LINK 0x80
#define IEEE1284_REQUEST_EPP_MODE           0x40
#define IEEE1284_REQUEST_ECP_MODE_WITH_RLE  0x30
#define IEEE1284_REQUEST_ECP_MODE           0x10
#define IEEE1284_REQUEST_DEVICE_ID_NIBBLE   0x04
#define IEEE1284_REQUEST_DEVICE_ID_BYTE     0x05
#define IEEE1284_REQUEST_DEVICE_ID_ECP      0x14
#define IEEE1284_REQUEST_DEVICE_ID_ECP_RLE  0x34
#define IEEE1284_REQUEST_BYTE_MODE          0x01
#define IEEE1284_REQUEST_BYTE_NIBBLE  0x00
#define IEPINT_ALL_ENABLE          0xFF    // Enable All IEndpoint Interrupts
#define IEPINT_ENDPOINT0_ENABLE    0x01    // Enable IEndpoint 0 Interrupt
#define IEPINT_ENDPOINT1_ENABLE    0x02    // Enable IEndpoint 1 Interrupt
#define IEPINT_ENDPOINT2_ENABLE    0x04    // Enable IEndpoint 2 Interrupt
#define IEPINT_ENDPOINT3_ENABLE    0x08    // Enable IEndpoint 3 Interrupt
#define IEPINT_ENDPOINT4_ENABLE       0x10    // Enable IEndpoint 4 Interrupt
#define IEPINT_ENDPOINT5_ENABLE       0x20    // Enable IEndpoint 5 Interrupt
#define IEPINT_ENDPOINT6_ENABLE       0x40    // Enable IEndpoint 6 Interrupt
#define IEPINT_ENDPOINT7_ENABLE       0x80    // Enable IEndpoint 7 Interrupt
#define IEPINT_ALL_DISABLE            0x00    // Disable All IEndpoint Interrupts
#define IEPINT_ENDPOINT0_DISABLE      0xFE    // Disable IEndpoint 0 Interrupt
#define IEPINT_ENDPOINT1_DISABLE      0xFD    // Disable IEndpoint 1 Interrupt
#define IEPINT_ENDPOINT2_DISABLE      0xFB    // Disable IEndpoint 2 Interrupt
#define IEPINT_ENDPOINT3_DISABLE      0xF7    // Disable IEndpoint 3 Interrupt
#define IEPINT_ENDPOINT4_DISABLE      0xEF    // Disable IEndpoint 4 Interrupt
#define IEPINT_ENDPOINT5_DISABLE      0xDF    // Disable IEndpoint 5 Interrupt
#define IEPINT_ENDPOINT6_DISABLE      0xBF    // Disable IEndpoint 6 Interrupt
#define IEPINT_ENDPOINT7_DISABLE      0x7F    // Disable IEndpoint 7 Interrupt
#define OEPINT_ALL_ENABLE             0xFF    // Enable All OEndpoint Interrupts
#define OEPINT_ENDPOINT0_ENABLE       0x01    // Enable OEndpoint 0 Interrupt
#define OEPINT_ENDPOINT1_ENABLE       0x02    // Enable OEndpoint 1 Interrupt
#define OEPINT_ENDPOINT2_ENABLE       0x04    // Enable OEndpoint 2 Interrupt
#define OEPINT_ENDPOINT3_ENABLE       0x08    // Enable OEndpoint 3 Interrupt
#define OEPINT_ENDPOINT4_ENABLE       0x10    // Enable OEndpoint 4 Interrupt
#define OEPINT_ENDPOINT5_ENABLE       0x20    // Enable OEndpoint 5 Interrupt
#define OEPINT_ENDPOINT6_ENABLE       0x40    // Enable OEndpoint 6 Interrupt
#define OEPINT_ENDPOINT7_ENABLE       0x80    // Enable OEndpoint 7 Interrupt
#define OEPINT_ALL_DISABLE            0x00    // Disable All OEndpoint Interrupts
#define OEPINT_ENDPOINT0_DISABLE      0xFE    // Disable OEndpoint 0 Interrupt
#define OEPINT_ENDPOINT1_DISABLE      0xFD    // Disable OEndpoint 1 Interrupt
#define OEPINT_ENDPOINT2_DISABLE      0xFB    // Disable OEndpoint 2 Interrupt
#define OEPINT_ENDPOINT3_DISABLE      0xF7    // Disable OEndpoint 3 Interrupt
#define OEPINT_ENDPOINT4_DISABLE      0xEF    // Disable OEndpoint 4 Interrupt
#define OEPINT_ENDPOINT5_DISABLE      0xDF    // Disable OEndpoint 5 Interrupt
#define OEPINT_ENDPOINT6_DISABLE      0xBF    // Disable OEndpoint 6 Interrupt
#define OEPINT_ENDPOINT7_DISABLE      0x7F    // Disable OEndpoint 7 Interrupt
#define VECINT_NO_INTERRUPT           0x00
#define VECINT_OUTPUT_ENDPOINT1       0x12
#define VECINT_OUTPUT_ENDPOINT2       0x14
#define VECINT_OUTPUT_ENDPOINT3       0x16
#define VECINT_INPUT_ENDPOINT1        0x22
#define VECINT_INPUT_ENDPOINT2        0x24
#define VECINT_INPUT_ENDPOINT3        0x26
#define VECINT_STPOW_PACKET_RECEIVED  0x30    // USBSTA
#define VECINT_SETUP_PACKET_RECEIVED  0x32    // USBSTA
#define VECINT_RESR_INTERRUPT         0x38    // USBSTA
#define VECINT_SUSR_INTERRUPT         0x3A    // USBSTA
#define VECINT_RSTR_INTERRUPT         0x3C    // USBSTA
#define VECINT_RWUP_INTERRUPT         0x3E    // USBSTA
#define VECINT_I2C_RXF_INTERRUPT      0x40    // I2CSTA
#define VECINT_I2C_TXE_INTERRUPT      0x42    // I2CSTA
#define VECINT_INPUT_ENDPOINT0        0x44
#define VECINT_OUTPUT_ENDPOINT0       0x46
#define VECINT_MODEM1_INTERRUPT       0x52
#define VECINT_MODEM2_INTERRUPT       0x56
//I2C Registers
#define I2CSTA_SWR                    0x01    // Stop Write Enable
// 0:disable, 1:enable
#define I2CSTA_SRD                    0x02    // Stop Read Enable
// 0:disable, 1:enable
#define I2CSTA_TIE          0x04        // I2C Transmitter Empty Interrupt Enable
// 0:disable, 1:enable
#define I2CSTA_TXE          0x08        // I2C Transmitter Empty
// 0:full, 1:empty
#define I2CSTA_400K         0x10        // I2C Speed Select
// 0:100kHz, 1:400kHz
#define I2CSTA_ERR          0x20        // Bus Error Condition
// 0:no bus error, 1:bus error
#define I2CSTA_RIE          0x40        // I2C Receiver Ready Interrupt Enable
// 0:disable, 1:enable
#define I2CSTA_RXF          0x80        // I2C Receiver Full
// 0:empty, 1:full
#define I2CADR_READ         0x01        // Read Write Command Bit
// 0:write, 1:read
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
// register address definition
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−
// EndPoint Desciptor Block
// USB Data Buffer
#define bOEP0_BUFFER_ADDRESS    (* (char xdata *)0xFEF0)    // Output Endpoint 0
// Buffer Base−address
#define bIEP0_BUFFER_ADDRESS    (* (char xdata *)0xFEF8)    // Input  Endpoint 0
// Buffer Base−address
#define bEP0_SETUP_ADDRESS      (* (char xdata *)0xFF00)    // setup packet
#define bOEPCNF1    (* (char xdata *)0xFF08)    // Output Endpoint 1 Configuration
#define bOEPCNF2    (* (char xdata *)0xFF10)    // Output Endpoint 2 Configuration
#define bOEPCNF3    (* (char xdata *)0xFF18)    // Output Endpoint 3 Configuration
#define bOEPCNF4    (* (char xdata *)0xFF20)    // Output Endpoint 4 Configuration
#define bOEPCNF5    (* (char xdata *)0xFF28)    // Output Endpoint 5 Configuration
#define bOEPCNF6    (* (char xdata *)0xFF30)    // Output Endpoint 6 Configuration
#define bOEPCNF7    (* (char xdata *)0xFF38)    // Output Endpoint 7 Configuration
#define bOEPBBAX1   (* (char xdata *)0xFF09)    // Output Endpoint 1 X−Buffer Base−address
#define bOEPBBAX2   (* (char xdata *)0xFF11)    // Output Endpoint 2 X−Buffer Base−address
#define bOEPBBAX3   (* (char xdata *)0xFF19)    // Output Endpoint 3 X−Buffer Base−address
#define bOEPBBAX4   (* (char xdata *)0xFF21)    // Output Endpoint 4 X−Buffer Base−address
#define bOEPBBAX5   (* (char xdata *)0xFF29)    // Output Endpoint 5 X−Buffer Base−address
#define bOEPBBAX6   (* (char xdata *)0xFF31)    // Output Endpoint 6 X−Buffer Base−address
#define bOEPBBAX7   (* (char xdata *)0xFF39)    // Output Endpoint 7 X−Buffer Base−address
#define bOEPBCTX1   (* (char xdata *)0xFF0A)    // Output Endpoint 1 X Byte Count
#define bOEPBCTX2   (* (char xdata *)0xFF12)    // Output Endpoint 2 X Byte Count
#define bOEPBCTX3   (* (char xdata *)0xFF1A)    // Output Endpoint 3 X Byte Count
#define bOEPBCTX4   (* (char xdata *)0xFF22)    // Output Endpoint 4 X Byte Count
#define bOEPBCTX5   (* (char xdata *)0xFF2A)    // Output Endpoint 5 X Byte Count
#define bOEPBCTX6   (* (char xdata *)0xFF32)    // Output Endpoint 6 X Byte Count
#define bOEPBCTX7   (* (char xdata *)0xFF3A)    // Output Endpoint 7 X Byte Count
#define bOEPBBAY1   (* (char xdata *)0xFF0D)    // Output Endpoint 1 Y−Buffer Base−address
#define bOEPBBAY2   (* (char xdata *)0xFF15)    // Output Endpoint 2 Y−Buffer Base−address
#define bOEPBBAY3   (* (char xdata *)0xFF1D)    // Output Endpoint 3 Y−Buffer Base−address
#define bOEPBBAY4   (* (char xdata *)0xFF25)    // Output Endpoint 4 Y−Buffer Base−address
#define bOEPBBAY5   (* (char xdata *)0xFF2D)    // Output Endpoint 5 Y−Buffer Base−address
#define bOEPBBAY6   (* (char xdata *)0xFF35)    // Output Endpoint 6 Y−Buffer Base−address
#define bOEPBBAY7   (* (char xdata *)0xFF3D)    // Output Endpoint 7 Y−Buffer Base−address
#define bOEPBCTY1   (* (char xdata *)0xFF0E)    // Output Endpoint 1 Y Byte Count
#define bOEPBCTY2   (* (char xdata *)0xFF16)    // Output Endpoint 2 Y Byte Count
#define bOEPBCTY3   (* (char xdata *)0xFF1E)    // Output Endpoint 3 Y Byte Count
#define bOEPBCTY4   (* (char xdata *)0xFF26)    // Output Endpoint 4 Y Byte Count
#define bOEPBCTY5   (* (char xdata *)0xFF2E)    // Output Endpoint 5 Y Byte Count
#define bOEPBCTY6   (* (char xdata *)0xFF36)    // Output Endpoint 6 Y Byte Count
#define bOEPBCTY7   (* (char xdata *)0xFF3E)    // Output Endpoint 7 Y Byte Count
#define bOEPSIZXY1  (* (char xdata *)0xFF0F)    // Output Endpoint 1 XY−Buffer Size
#define bOEPSIZXY2  (* (char xdata *)0xFF17)    // Output Endpoint 2 XY−Buffer Size
#define bOEPSIZXY3  (* (char xdata *)0xFF1F)    // Output Endpoint 3 XY−Buffer Size
#define bOEPSIZXY4  (* (char xdata *)0xFF27)    // Output Endpoint 4 XY−Buffer Size
#define bOEPSIZXY5  (* (char xdata *)0xFF2F)    // Output Endpoint 5 XY−Buffer Size
#define bOEPSIZXY6  (* (char xdata *)0xFF37)    // Output Endpoint 6 XY−Buffer Size
#define bOEPSIZXY7  (* (char xdata *)0xFF3F)    // Output Endpoint 7 XY−Buffer Size
#define bIEPCNF1    (* (char xdata *)0xFF48)    // Input Endpoint 1 Configuration
#define bIEPCNF2    (* (char xdata *)0xFF50)    // Input Endpoint 2 Configuration
#define bIEPCNF3    (* (char xdata *)0xFF58)    // Input Endpoint 3 Configuration
#define bIEPCNF4    (* (char xdata *)0xFF60)    // Input Endpoint 4 Configuration
#define bIEPCNF5    (* (char xdata *)0xFF68)    // Input Endpoint 5 Configuration
#define bIEPCNF6    (* (char xdata *)0xFF70)    // Input Endpoint 6 Configuration
#define bIEPCNF7    (* (char xdata *)0xFF78)    // Input Endpoint 7 Configuration
#define bIEPBBAX1   (* (char xdata *)0xFF49)    // Input Endpoint 1 X−Buffer Base−address
#define bIEPBBAX2   (* (char xdata *)0xFF51)    // Input Endpoint 2 X−Buffer Base−address
#define bIEPBBAX3   (* (char xdata *)0xFF59)    // Input Endpoint 3 X−Buffer Base−address
#define bIEPBBAX4   (* (char xdata *)0xFF61)    // Input Endpoint 4 X−Buffer Base−address
#define bIEPBBAX5   (* (char xdata *)0xFF69)    // Input Endpoint 5 X−Buffer Base−address
#define bIEPBBAX6   (* (char xdata *)0xFF71)    // Input Endpoint 6 X−Buffer Base−address
#define bIEPBBAX7   (* (char xdata *)0xFF79)    // Input Endpoint 7 X−Buffer Base−address
#define bIEPDCTX1   (* (char xdata *)0xFF4A)    // Input Endpoint 1 X Byte Count
#define bIEPDCTX2   (* (char xdata *)0xFF52)    // Input Endpoint 2 X Byte Count
#define bIEPDCTX3   (* (char xdata *)0xFF5A)    // Input Endpoint 3 X Byte Count
#define bIEPDCTX4   (* (char xdata *)0xFF62)    // Input Endpoint 4 X Byte Count
#define bIEPDCTX5   (* (char xdata *)0xFF6A)    // Input Endpoint 5 X Byte Count
#define bIEPDCTX6   (* (char xdata *)0xFF72)    // Input Endpoint 6 X Byte Count
#define bIEPDCTX7   (* (char xdata *)0xFF7A)    // Input Endpoint 7 X Byte Count
#define bIEPBBAY1   (* (char xdata *)0xFF4D)    // Input Endpoint 1 Y−Buffer Base−address
#define bIEPBBAY2   (* (char xdata *)0xFF55)    // Input Endpoint 2 Y−Buffer Base−address
#define bIEPBBAY3   (* (char xdata *)0xFF5D)    // Input Endpoint 3 Y−Buffer Base−address
#define bIEPBBAY4   (* (char xdata *)0xFF65)    // Input Endpoint 4 Y−Buffer Base−address
#define bIEPBBAY5   (* (char xdata *)0xFF6D)    // Input Endpoint 5 Y−Buffer Base−address
#define bIEPBBAY6   (* (char xdata *)0xFF75)    // Input Endpoint 6 Y−Buffer Base−address
#define bIEPBBAY7   (* (char xdata *)0xFF7D)    // Input Endpoint 7 Y−Buffer Base−address
#define bIEPDCTY1   (* (char xdata *)0xFF4E)    // Input Endpoint 1 Y Byte Count
#define bIEPDCTY2   (* (char xdata *)0xFF56)    // Input Endpoint 2 Y Byte Count
#define bIEPDCTY3   (* (char xdata *)0xFF5E)    // Input Endpoint 3 Y Byte Count
#define bIEPDCTY4   (* (char xdata *)0xFF66)    // Input Endpoint 4 Y Byte Count
#define bIEPDCTY5   (* (char xdata *)0xFF6E)    // Input Endpoint 5 Y Byte Count
#define bIEPDCTY6   (* (char xdata *)0xFF76)    // Input Endpoint 6 Y Byte Count
#define bIEPDCTY7   (* (char xdata *)0xFF7E)    // Input Endpoint 7 Y Byte Count
#define bIEPSIZXY1  (* (char xdata *)0xFF4F)    // Input Endpoint 1 XY−Buffer Size
#define bIEPSIZXY2  (* (char xdata *)0xFF57)    // Input Endpoint 2 XY−Buffer Size
#define bIEPSIZXY3  (* (char xdata *)0xFF5F)    // Input Endpoint 3 XY−Buffer Size
#define bIEPSIZXY4  (* (char xdata *)0xFF67)    // Input Endpoint 4 XY−Buffer Size
#define bIEPSIZXY5  (* (char xdata *)0xFF6F)    // Input Endpoint 5 XY−Buffer Size
#define bIEPSIZXY6  (* (char xdata *)0xFF77)    // Input Endpoint 6 XY−Buffer Size
#define bIEPSIZXY7  (* (char xdata *)0xFF7F)    // Input Endpoint 7 XY−Buffer Size
// Endpoint 0 Descriptor Registers
#define bIEPCNFG0   (* (char xdata *)0xFF80)    // Input Endpoint Configuration Register
#define bIEPBCNT0   (* (char xdata *)0xFF81)    // Input Endpoint 0 Byte Count
#define bOEPCNFG0   (* (char xdata *)0xFF82)    // Output Endpoint Configuration Register
#define bOEPBCNT0   (* (char xdata *)0xFF83)    // Output Endpoint 0 Byte Count
// Miscellaneous Registers
#define bROMS       (* (char xdata *)0xFF90)    // ROM Shadow Configuration Register
#define bGLOBCTL    (* (char xdata *)0xFF91)    // Global Control Register
#define bVECINT     (* (char xdata *)0xFF92)    // Vector Interrupt Register
#define bIEPINT     (* (char xdata *)0xFF93)    // Input Endpoint Interrupt
// Request Register
// 0:no interrupt, 1:interrupt pending
#define bOEPINT     (* (char xdata *)0xFF94)    // Output Endpoint Interrupt
// Request Register
// 0:no interrupt, 1:interrupt pending
// IEEE 1284 Registers
#define bPPTIME     (* (char xdata *)0xFF99)    // P−Port Timing Definition Register
#define bPPMCR      (* (char xdata *)0xFF9A)    // P−Port Mode Control Register
#define bPPIMSK     (* (char xdata *)0xFF9B)    // P−Port Interrupt Mask Register
#define bPPSTA      (* (char xdata *)0xFF9C)    // P−Port Status Register
#define bPPCTL      (* (char xdata *)0xFF9D)    // P−Port Control Register
#define bPPDAT      (* (char xdata *)0xFF9E)    // P−Port Data Register
#define bPPADR      (* (char xdata *)0xFF9F)    // P−Port EPP ECP Address Register
// UART Registers
#define bRDR1       (* (char xdata *)0xFFA0)    // UART1 Receiver Data Register
#define bTDR1       (* (char xdata *)0xFFA1)    // UART1 Transmitter Data Register
#define bLCR1       (* (char xdata *)0xFFA2)    // UART1 Line Control Register
#define bFCRL1      (* (char xdata *)0xFFA3)    // UART1 Flow Control Register
#define bMCR1       (* (char xdata *)0xFFA4)    // UART1 Modem Control Register
#define bLSR1       (* (char xdata *)0xFFA5)    // UART1 Line Status Register
#define bMSR1       (* (char xdata *)0xFFA6)    // UART1 Modem Status Register
#define bDLL1       (* (char xdata *)0xFFA7)    // UART1 Divisor Register Low−byte
#define bDLH1       (* (char xdata *)0xFFA8)    // UART1 Divisor Register High−byte
#define bXON1       (* (char xdata *)0xFFA9)    // UART1 Xon Register
#define bXOFF1      (* (char xdata *)0xFFAA)    // UART1 Xoff Register
#define bMASK1      (* (char xdata *)0xFFAB)    // UART1 Interrupt Mask Register
#define bRDR2       (* (char xdata *)0xFFB0)    // UART2 Receiver Data Register
#define bTDR2       (* (char xdata *)0xFFB1)    // UART2 Transmitter Data Register
#define bLCR2       (* (char xdata *)0xFFB2)    // UART2 Line Control Register
#define bFCRL2      (* (char xdata *)0xFFB3)    // UART2 Flow Control Register
#define bMCR2       (* (char xdata *)0xFFB4)    // UART2 Modem Control Register
#define bLSR2       (* (char xdata *)0xFFB5)    // UART2 Line Status Register
#define bMSR2       (* (char xdata *)0xFFB6)    // UART2 Modem Status Register
#define bDLL2       (* (char xdata *)0xFFB7)    // UART2 Divisor Register Low−byte
#define bDLH2       (* (char xdata *)0xFFB8)    // UART2 Divisor Register High−byte
#define bXON2       (* (char xdata *)0xFFB9)    // UART2 Xon Register
#define bXOFF2      (* (char xdata *)0xFFBA)    // UART2 Xoff Register
#define bMASK2      (* (char xdata *)0xFFBB)    // UART2 Interrupt Mask Register
// DMA registers
#define bDMACDR1    (* (char xdata *)0xFFE0)    // DMA Channel 1 Definition Register
// for UART 1 Transmitter
#define bDMACSR1    (* (char xdata *)0xFFE1)    // DMA Channel 1 Control & Status Register
#define bDMACDR2    (* (char xdata *)0xFFE2)    // DMA Channel 2 Definition Register
// for UART 2 Transmitter
#define bDMACSR2    (* (char xdata *)0xFFE3)    // DMA Channel 2 Control & Status Register
#define bDMACDR3    (* (char xdata *)0xFFE4)   // DMA Channel 3 Definition Register
// for UART 1 Receiver
#define bDMACSR3    (* (char xdata *)0xFFE5)   // DMA Channel 3 Control & Status Register
#define bDMACDR4    (* (char xdata *)0xFFE6)   // DMA Channel 4 Definition Register
// for UART 2 Receiver
#define bDMACSR4    (* (char xdata *)0xFFE7)   // DMA Channel 4 Control & Status Register
#define bDMACDR5    (* (char xdata *)0xFFE8)   // DMA Channel 5 Definition
#define bDMACSR5    (* (char xdata *)0xFFE9)   // DMA Channel 5 Control & Status Register
//I2C Registers
#define bI2CSTA     (* (char xdata *)0xFFF0)   // I2C Status and Control Register
#define bI2CDAO     (* (char xdata *)0xFFF1)   // I2C Data Out Register
#define bI2CDAI     (* (char xdata *)0xFFF2)   // I2C Data In Register
#define bI2CADR     (* (char xdata *)0xFFF3)   // I2C Address Register
// USB Registers
#define bHUBCURT    (* (char xdata *)0xFFF4)   // HUB Current Descriptor Register
#define bHUBPOTG    (* (char xdata *)0xFFF5)   // HUB Power−on to Power−Good Descriptor Register
#define bHUBCNF2    (* (char xdata *)0xFFF6)   // HUB Configuration−2 Register
#define bHUBCNF1    (* (char xdata *)0xFFF7)   // HUB Configuration−1 Register
#define bHUBPIDL    (* (char xdata *)0xFFF8)   // HUB PID Low−byte Register
#define bHUBPIDH    (* (char xdata *)0xFFF9)   // HUB PID High−byte Register
#define bHUBVIDL    (* (char xdata *)0xFFFA)   // HUB VID Low−byte Register
#define bHUBVIDH    (* (char xdata *)0xFFFB)   // HUB VID High−byte Register
#define bUSBCTL     (* (char xdata *)0xFFFC)   // USB Control Register
#define bUSBMSK     (* (char xdata *)0xFFFD)   // USB Interrupt Mask Register
#define bUSBSTA     (* (char xdata *)0xFFFE)   // USB Status Register
#define bFUNADR     (* (char xdata *)0xFFFF)   // This register contains the
// device function address.
#ifdef __cplusplus
}
#endif
#endif /* _TUSB5052_H_ */
//−−−−−−−−−−−−−−−−−−−−−−−−−−−−− Cut along the line −−−−−−−−−−−−−−−−−−−−−−−−−−−−
