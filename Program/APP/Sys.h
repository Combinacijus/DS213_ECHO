/********************* (C) COPYRIGHT 2018 e-Design Co.,Ltd. ********************
  DS213_APP Sys.h                                                Author : bure
*******************************************************************************/
#ifndef __SYS_H
#define __SYS_H

  #ifdef STM32F30X
    #include "stm32f30x.h"
  #elif STM32F10X_HD
    #include "stm32f10x.h"
  #endif

  #include <string.h>
  #include <stdio.h>
  #include "DS213HwDriver.h"
  #include "USB_istr.h"
  #include "USB_scsi.h"
  #include "usb_regs.h"
  #include "USB_conf.h"
  #include "usb_lib.h"
  #include "USB_bot.h"
  #include "usb_mem.h"
  #include "USB_pwr.h"
  #include "Interrupt.h"
  #include "Process.h"
  #include "FAT12.h"
  #include "Fpga.h"
  #include "Func.h"
  #include "AppDisk.h"
  #include "File.h"
  #include "Menu.h"
  #include "Draw.h"
  #include "Calibrat.h"

  typedef uint64_t  u64;
  typedef int64_t   s64;

  #define DC               1
  #define AC               0
  #define MAX_SEC_SIZE     4096

  #define CALIBRAT(k, n)  (((k)*(n)+512)/1024)
  #define CALIBRATE(n, k) (((n)*(k)+512)/1024)

  #define ENC1a           0x1000
  #define ENC1b           0x2000
  #define ENC2a           0x4000
  #define ENC2b           0x8000

  #define K1              0x0001
  #define K2              0x0002
  #define K3              0x0004
  #define K4              0x0008
  #define KEYSBIT         0x000F

  #define K1_ACTn         0x0010
  #define K2_ACTn         0x0020
  #define K3_ACTn         0x0040
  #define K4_ACTn         0x0080
  #define K1_ACTp         0x0100
  #define K2_ACTp         0x0200
  #define K3_ACTp         0x0400
  #define K4_ACTp         0x0800
  #define Kn_ACTn         0x00F0
  #define Kn_ACTp         0x0F00
  #define ENCD_1p         0x1000
  #define ENCD_1n         0x2000
  #define ENCD_2p         0x4000
  #define ENCD_2n         0x8000
  #define ENCDBIT         0xF000

  #define CHAR            0x00   // Standard Display Character display
  #define INVR            0x01   // Inverse Display  Reverse display
  #define SYMB            0x02   // Symbol Display   Symbol display
  #define VOID            0x04   // Dotted Display   Ghost display

  #define LED_PWM_MAX     1000
  #define LED_PWM_MIN     0

  extern HwDrvDef Hw;
  extern vu16 PD_Cnt;

  extern vu16 BeepCnt, Dly_mS, LED_Pwm;//, TestCnt
  extern u8   Flashing, PwrDownEn, DiskBuf[];
  extern u16  Fps, nKeySt, KeyAct, Vbat; //SleepEn,
  extern uc16 Wait[];
  extern u32  SectorSize;
  extern u8 sigIntProbe;
  #define SIG_INT_FREQ    3

  #define TRIG_TYPE_ANY   8

  void DiskReset();

  void Beep_mS(u16 mS);
  void Delay_mS(vu32 mS);
  void USB_DevInit(void);
  void USB_DiskLink(u8 NewState);

  u16  RdPxl(void);
  u16  Font_8x14(u8 Code, u16 Row);
  u32  GetDev_SN(void);

  void WrPxl(u16 Color);
  void PxlPosi(u16 x, u16 y);
  void LCD_WrBlock(u16 x1, u16 y1, u16 x2, u16 y2);
  void LCD_RdBlock(u16 x1, u16 y1, u16 x2, u16 y2);

  void SmplStart(void);
  void TrackClr(void);
  u16  FPGA_SmplRd(u8 Dev);

  u8   FPGA_ByteRd(u8 Dev, u8 Cmd);
  u16  FPGA_DataRd(u8 Dev, u8 Cmd);
  u32  FPGA_WordRd(u8 Dev, u8 Cmd);
  u8   FPGA_CtrlRW(u8 Dev, u8 Cmd);
  u8   FPGA_ByteWr(u8 Dev, u8 Cmd, u8  Byte);
  u8   FPGA_DataWr(u8 Dev, u8 Cmd, u16 Data);

  void SetRangeA(u8 Range);
  void SetRangeB(u8 Range);
  void SetBase(u8 Base);
  void SetOffsetA(u8 Range, u8 Offset);
  void SetOffsetB(u8 Range, u8 Offset);

  void DispStr(u16 x, u16 y, u16 Color, u8 Mode, u8 *Str);
  void SetTriggTyp(u8 Type, u8 Src);
  void SetPreSmplDpth(u16 PsDpth);
  void SetDgtlFrq(u16 Nfrq);
  void SetAnlgFrq(u16 Nfrq);

#endif
