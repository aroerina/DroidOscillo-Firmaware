################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/src/cr_startup_lpc43xx_m0app.c \
../src/src/hid_desc.c \
../src/src/m0_boot_main.c \
../src/src/m0_boot_usbfunc.c \
../src/src/sysinit.c 

OBJS += \
./src/src/cr_startup_lpc43xx_m0app.o \
./src/src/hid_desc.o \
./src/src/m0_boot_main.o \
./src/src/m0_boot_usbfunc.o \
./src/src/sysinit.o 

C_DEPS += \
./src/src/cr_startup_lpc43xx_m0app.d \
./src/src/hid_desc.d \
./src/src/m0_boot_main.d \
./src/src/m0_boot_usbfunc.d \
./src/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
src/src/%.o: ../src/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__CODE_RED -D__USE_LPCOPEN -DCORE_M0 -D__MULTICORE_M0APP -D__REDLIB__ -DDEBUG -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc\config_m0app" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc\usbd" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_board_nxp_lpclink2_4370_m0\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_BOOT_M4\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_BOOT_M0APP\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_M0APP\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_M4\src\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


