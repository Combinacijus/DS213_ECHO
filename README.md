# DS213_ECHO


# !!!!!FLASH IT AT YOUR OWN RISK!!!!!
FLASHING THIS EXPERIMENTAL FIRMWARE DOESN'T ALWAYS WORK. (Probably you will be able to downgrade but there might be a small chance of something happening)
## THIS FFT PROJECT IS ABANDONED BY ME
If someone would want to continue the first good step would be to compile only neccessary newly added FFT and math libraries to make compiled firmware smaller. I think inconsistencies in flashing might be due to big firmware binary

This is a customized DS213 APP firmware with Roll Mode support (and other features). Based on the official app firmware v1.27, and fixed some bugs.

## What is [Roll Mode](https://www.sweetwater.com/insync/roll-mode/)?

On an oscilloscope, roll mode is a method of displaying acquired waveform data without waiting for the complete waveform record. For example, if a sweep was 10 divisions long with a rate of one second per division, and roll mode wasn’t available, it would take 10 seconds to fill the waveform record. But using roll mode, the oscilloscope will immediately
begin displaying results rather than waiting the full 10 seconds.

## Roll Mode Video Demo

- [Youtube](https://youtu.be/iGE-QNMqRk0)
- [Bilibili](https://www.bilibili.com/video/BV1QA411x7YX)

## DS213 FFT Spectrum Analyzer Firmware (Work in Progress)
FFT demo video: https://www.youtube.com/watch?v=ZVqgT-4zOE0&ab_channel=Combinacijus

For testing you can try updating to: [./Release/DS213_ECHO_v20.11.28_FFT.hex](./Release/DS213_ECHO_v20.11.28_FFT.hex)

**Note**: Config backup (.RPM and .BAK files) has to be removed because menu element was added and old config is not valid. Same with downgrading from this firmware. You can save .RPM and .BAK on your computer for later in case of downgrade or those files can be deleted so all settings will go to default values.

FFT Features:
- Enable CH(D) to display FFT spectrum.
- Scroll right side FV element to zoom vertically.
- T1 and T2 slices spectrum to show only selected range. To show full spectrum slide T1 fully to the left and T2 to the right.
- Spectrum parameters: F1 - start frequency, Fdiv - frequency/div, F2 - end frequency

## Toolchain

The firmware is built on GNU Arm Embedded Toolchain. Both Windows and Linux are tested.

### On Windows

```
arm-none-eabi-gcc --version
arm-none-eabi-gcc.exe (GNU Tools for Arm Embedded Processors 9-2019-q4-major) 9.
2.1 20191025 (release) [ARM/arm-9-branch revision 277599]
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## Windows setup
### One time dependency setup
1. Install `arm-none-eabi-gcc --version` [download from](https://developer.arm.com/downloads/-/gnu-rm). Probably latest version will work but tested on `arm-none-eabi-gcc --version` arm-none-eabi-gcc.exe (GNU Arm Embedded Toolchain 10.3-2021.10) 10.3.1 20210824 (release)`
2. Install `make`
  2.1. Open Powershell as admin (must be admin)
  2.2. Run `choco install make`. After that **make** can be used without admin
1. Fully close and reopen vscode and terminal that you gonna use for building

### Build project on Windows

1. [Optional] You can increment app version in file `Program\APP\Main.c` around line 86 `u8  APP_VERSION[12] = { "v20.11.27" };`
2. Open Powershell terminal.
3. Navigate to the project folder e.g. with command `cd C:\GitHub\DS213_ECHO` inside which `Makefile` is located
4. Run command `make`. This will create new `build` folder with build files
5. Only file you need is `build/DS213_ECHO.hex` 
6. Then follow firmware update instructions as usual but use your new .hex file


### On Linux (Ubuntu 18.04):

```
arm-none-eabi-gcc --version
arm-none-eabi-gcc (15:6.3.1+svn253039-1build1) 6.3.1 20170620
Copyright (C) 2016 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## FLASH Layout

The MCU of DS213 is STM32F103VET6, with 512kB(0x80000) FLASH. The FLASH layout is as follows:

| Name | Start      | Length  |
|:----:|:----------:|:--------|
| DFU  | 0x08000000 | 0x08000 |
| APP  | 0x08008000 | 0x68000 |
| FPGA | 0x08070000 | 0x0D800 |
| LOGO | 0x0807D800 | 8310    |

## (Legacy) Firmware Update Procedure

Full update procedure in case a speedy version below doesn't work

1. Enter DS213 DFU mode (turn power on while holding K1 Play/Pause button)
2. Copy FPGA_CFG.ADR to DFU disk, Then the DFU disk will restart.
3. Copy STMU_213_018.bin to DFU disk, Then the DFU disk will restart.
4. Copy DS213_ECHO_v20.12.25.hex to DFU disk.
5. Restart DS213.

There are no address information in the .bin file, so we put a .ADR file to DFU disk to setup the firmware start addressThat's what step 2 did. However, the .hex file contains address information, we don't need a .ADR file. If your DS213 is using the official latest firmware, the FPGA is latest 018 version, you can omit step 2 and 3.

## Speedy Firmware Update Procedure
1. Prep: Have file explorer open and `DS213_ECHO.hex` in view and USB connected
2. Build: `make`
3. Enter DFU: Hold K1 Play/Pause button and turn off and on
4. Drag `DS213_ECHO.hex` to newly opened window
5. Off & On at the moment copy window disappears (no need to wait any longer)

## Hardware and Trivia

- STM32F103VET6 32 bit ARM Cortex-M3 72MHz
- No FPU
- 512KB FLASH, 64KB RAM
- 8 MB memory for waveform data and images
- Internal 1000 mAh Lithium battery
- 2.8" Full Color TFT LCD
- 400x240 resolution
- 360x200 area for waveform (other area taken by menus)
- 12x8 grid | dx: 30px/div | dy: 25px/div
