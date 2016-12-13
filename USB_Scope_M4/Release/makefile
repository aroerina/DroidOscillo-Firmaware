################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: USB_Scope_M4.axf

# Tool invocations
USB_Scope_M4.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\MY_CMSIS_LPC43xx_DriverLib\Debug" -Xlinker -Map="USB_Scope_M4.map" -Xlinker --gc-sections -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -mthumb -T "USB_Scope_M4_Release.ld" -o "USB_Scope_M4.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) USB_Scope_M4.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "USB_Scope_M4.axf" ; arm-none-eabi-objcopy -O binary "USB_Scope_M4.axf" "USB_Scope_M4.bin" ; mv "USB_Scope_M4.bin" "m4.bin" ; cp "m4.bin" "C:\Dev\project\Android_Oscilloscope\host_app\Android\USBOscilloscopeHost\USBOscilloscopeHost\res\raw" ; #checksum -p LPC4370 -d "USB_Scope_M4.bin" ; arm-none-eabi-objdump -S -d scope_init.o
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
