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
all: USB_Scope_BOOT_M0APP.axf

# Tool invocations
USB_Scope_BOOT_M0APP.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_chip_43xx_m0\Debug" -L"C:\Dev\project\Android_Oscilloscope\device_firm\USB_scope\lpc_board_nxp_lpclink2_4370_m0\Debug" -Xlinker -Map="USB_Scope_BOOT_M0APP.map" -Xlinker --gc-sections -mcpu=cortex-m0 -mthumb -T "USB_Scope_BOOT_M0APP_Release.ld" -o "USB_Scope_BOOT_M0APP.axf" $(OBJS) $(USER_OBJS) $(LIBS) && arm-none-eabi-objcopy --target elf32-littlearm --verbose --strip-all --remove-section=.ARM.attributes --redefine-sym __vectors_start__=__vectors_start___core_m0app --keep-symbol __vectors_start___core_m0app --redefine-sym _data=__start_data_core_m0app --keep-symbol __start_data_core_m0app --redefine-sym _edata=__end_data_core_m0app --keep-symbol __end_data_core_m0app --remove-section=".bss*" --remove-section=".noinit*" --rename-section .ARM.exidx=".core_m0app.ARM.exidx" --rename-section .ARM.extab=".core_m0app.ARM.extab" --rename-section .text=".core_m0app" --rename-section .data=".core_m0app.data" "USB_Scope_BOOT_M0APP.axf" "USB_Scope_BOOT_M0APP.axf.o"
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) USB_Scope_BOOT_M0APP.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "USB_Scope_BOOT_M0APP.axf"; # arm-none-eabi-objcopy -v -O binary "USB_Scope_BOOT_M0APP.axf" "USB_Scope_BOOT_M0APP.bin" ; # checksum -p LPC4370_M0 -d "USB_Scope_BOOT_M0APP.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
