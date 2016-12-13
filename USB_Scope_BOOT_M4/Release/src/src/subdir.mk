################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/src/boot_m4_main.c \
../src/src/cr_startup_lpc43xx.c \
../src/src/sysinit.c 

OBJS += \
./src/src/boot_m4_main.o \
./src/src/cr_startup_lpc43xx.o \
./src/src/sysinit.o 

C_DEPS += \
./src/src/boot_m4_main.d \
./src/src/cr_startup_lpc43xx.d \
./src/src/sysinit.d 


# Each subdirectory must supply rules for building sources it contributes
src/src/%.o: ../src/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DRELEASE -D__USE_LPCOPEN -D__CODE_RED -DCORE_M4 -D__MULTICORE_MASTER -D__MULTICORE_MASTER_SLAVE_M0APP -D__REDLIB__ -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_board_nxp_lpclink2_4370\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_M4\src\inc" -I"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\USB_Scope_BOOT_M4\src\inc" -O0 -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


