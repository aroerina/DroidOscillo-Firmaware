################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/bluetooth_scope.c \
../src/crp.c \
../src/lpc43xx_libcfg_default.c \
../src/new_cr_startup_lpc43xx.c \
../src/new_lpc43xx_cgu.c \
../src/new_lpc43xx_gpdma.c 

OBJS += \
./src/bluetooth_scope.o \
./src/crp.o \
./src/lpc43xx_libcfg_default.o \
./src/new_cr_startup_lpc43xx.o \
./src/new_lpc43xx_cgu.o \
./src/new_lpc43xx_gpdma.o 

C_DEPS += \
./src/bluetooth_scope.d \
./src/crp.d \
./src/lpc43xx_libcfg_default.d \
./src/new_cr_startup_lpc43xx.d \
./src/new_lpc43xx_cgu.d \
./src/new_lpc43xx_gpdma.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_CMSIS=CMSIS_LPC43xx_DriverLib -D__LPC43XX__ -D__MULTICORE_NONE -I"C:\Dev\source\ARM\NXP\LPC4370\USB_scope\CMSIS_LPC43xx_DriverLib\inc" -I"C:\Dev\source\ARM\NXP\LPC4370\USB_scope\USB_Scope\inc" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/new_cr_startup_lpc43xx.o: ../src/new_cr_startup_lpc43xx.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -DCORE_M4 -D__USE_CMSIS=CMSIS_LPC43xx_DriverLib -D__LPC43XX__ -D__MULTICORE_NONE -I"C:\Dev\source\ARM\NXP\LPC4370\USB_scope\CMSIS_LPC43xx_DriverLib\inc" -I"C:\Dev\source\ARM\NXP\LPC4370\USB_scope\USB_Scope\inc" -Os -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fsingle-precision-constant -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/new_cr_startup_lpc43xx.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


