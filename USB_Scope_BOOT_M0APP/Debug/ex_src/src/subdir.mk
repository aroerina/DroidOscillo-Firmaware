################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Dev/project/Android_Oscilloscope/device_firm/USB_scope/USB_Scope_M0APP/src/src/m0app_init.c \
C:/Dev/project/Android_Oscilloscope/device_firm/USB_scope/USB_Scope_M0APP/src/src/main_usb_handler.c \
C:/Dev/project/Android_Oscilloscope/device_firm/USB_scope/USB_Scope_M0APP/src/src/vector_table.c 

OBJS += \
./ex_src/src/m0app_init.o \
./ex_src/src/main_usb_handler.o \
./ex_src/src/vector_table.o 

C_DEPS += \
./ex_src/src/m0app_init.d \
./ex_src/src/main_usb_handler.d \
./ex_src/src/vector_table.d 


# Each subdirectory must supply rules for building sources it contributes
ex_src/src/m0app_init.o: C:/Dev/project/Android_Oscilloscope/device_firm/USB_scope/USB_Scope_M0APP/src/src/m0app_init.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__CODE_RED -D__USE_LPCOPEN -DCORE_M0 -D__MULTICORE_M0APP -D__REDLIB__ -DDEBUG -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc\config_m0app" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc\usbd" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_board_nxp_lpclink2_4370_m0\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_BOOT_M4\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_BOOT_M0APP\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_M0APP\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_M4\src\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

ex_src/src/main_usb_handler.o: C:/Dev/project/Android_Oscilloscope/device_firm/USB_scope/USB_Scope_M0APP/src/src/main_usb_handler.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__CODE_RED -D__USE_LPCOPEN -DCORE_M0 -D__MULTICORE_M0APP -D__REDLIB__ -DDEBUG -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc\config_m0app" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc\usbd" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_board_nxp_lpclink2_4370_m0\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_BOOT_M4\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_BOOT_M0APP\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_M0APP\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_M4\src\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

ex_src/src/vector_table.o: C:/Dev/project/Android_Oscilloscope/device_firm/USB_scope/USB_Scope_M0APP/src/src/vector_table.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__CODE_RED -D__USE_LPCOPEN -DCORE_M0 -D__MULTICORE_M0APP -D__REDLIB__ -DDEBUG -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc\config_m0app" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc\usbd" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_board_nxp_lpclink2_4370_m0\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_BOOT_M4\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_BOOT_M0APP\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_M0APP\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_M4\src\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


