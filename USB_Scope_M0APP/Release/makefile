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
all: USB_Scope_M0APP.axf

# Tool invocations
USB_Scope_M0APP.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\Debug" -L"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_board_nxp_lpclink2_4370_m0\Debug" -Xlinker -Map="USB_Scope_M0APP.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "USB_Scope_M0APP_Release.ld" -o "USB_Scope_M0APP.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) USB_Scope_M0APP.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "USB_Scope_M0APP.axf" ; arm-none-eabi-objcopy -v -O binary "USB_Scope_M0APP.axf" "USB_Scope_M0APP.bin" ; mv "USB_Scope_M0APP.bin" "m0app.bin" ; cp "m0app.bin" "C:\Dev\project\Android_Oscilloscope\host_app\Android\USBOscilloscopeHost\USBOscilloscopeHost\res\raw" ; # checksum -p LPC4370_M0 -d "USB_Scope_M0APP.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
