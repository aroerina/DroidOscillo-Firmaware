## DroidOscillo firmware

## Description
DroidOscillo firmware.
C language source code and LPCXpresso project files.


## Building tools
LPCXpresso 8.2

## Install
Please download .zip or clone and import into your workspace.

## Folder Description

USB_Scope_BOOT_M0APP  
:Boot loader for LPC4370's m0app core.This is written to SPI flash memory.
 This program is mainly used for controlling USB communication
  
USB_Scope_BOOT_M4  
:Boot loader for LPC4370's m4 core.This is written to SPI flash memory.  
  
  
USB_Scope_M0APP  
:Firmware transmitted from the host application for LPC4370's m0app core. 
 This program is mainly used for controlling USB communication  


USB_Scope_M4  
:Firmware transmitted from the host application for LPC4370's m4 core. 
 This program is mainly responsible for controlling the highspeed ADC. 
 This program uses the CMSIS library.  


## Related repositorys

DroidOscillo Android host: https://github.com/aroerina/DroidOscillo-Android_host  
DroidOscillo Board: https://github.com/aroerina/DoroidOscillo-Board  

## Wiki
https://github.com/aroerina/DroidOscillo-Android_host/wiki
 
 
## Licence

[MIT Licence](https://github.com/tcnksm/tool/blob/master/LICENCE)
