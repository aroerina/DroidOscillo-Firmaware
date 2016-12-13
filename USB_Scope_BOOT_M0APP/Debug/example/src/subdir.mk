################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../example/src/cr_startup_lpc43xx_m0app.c \
../example/src/hid_desc.c \
../example/src/hid_generic.c \
../example/src/hid_main.c \
../example/src/sysinit.c 

OBJS += \
./example/src/cr_startup_lpc43xx_m0app.o \
./example/src/hid_desc.o \
./example/src/hid_generic.o \
./example/src/hid_main.o \
./example/src/sysinit.o 

C_DEPS += \
./example/src/cr_startup_lpc43xx_m0app.d \
./example/src/hid_desc.d \
./example/src/hid_generic.d \
./example/src/hid_main.d \
./example/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
example/src/%.o: ../example/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -D__USE_LPCOPEN -DCORE_M0 -D__MULTICORE_M0APP -D__REDLIB__ -I"C:\Dev\source\ARM\NXP\LPC4370\USB_scope\lpc_chip_43xx_m0\inc\config_m0app" -I"C:\Dev\source\ARM\NXP\LPC4370\USB_scope\USB_Scope_M4\src\inc" -I"C:\Dev\source\ARM\NXP\LPC4370\USB_scope\lpc_chip_43xx_m0\inc\usbd" -I"C:\Dev\source\ARM\NXP\LPC4370\USB_scope\lpc_board_nxp_lpclink2_4370_m0\inc" -I"C:\Dev\source\ARM\NXP\LPC4370\USB_scope\lpc_chip_43xx_m0\inc" -I"C:\Dev\source\ARM\NXP\LPC4370\USB_scope\USB_scope_M0APP\example\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m0 -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


