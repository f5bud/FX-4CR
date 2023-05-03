################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/Src/subdir.mk
-include Dsp/Src/TransformFunctions/subdir.mk
-include Dsp/Src/SupportFunctions/subdir.mk
-include Dsp/Src/StatisticsFunctions/subdir.mk
-include Dsp/Src/MatrixFunctions/subdir.mk
-include Dsp/Src/FilteringFunctions/subdir.mk
-include Dsp/Src/FastMathFunctions/subdir.mk
-include Dsp/Src/ControllerFunctions/subdir.mk
-include Dsp/Src/ComplexMathFunctions/subdir.mk
-include Dsp/Src/CommonTables/subdir.mk
-include Dsp/Src/BasicMathFunctions/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := FX-4CR
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
FX-4CR.elf \

MAP_FILES += \
FX-4CR.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
FX-4CR.list \

OBJCOPY_HEX += \
FX-4CR.hex \

OBJCOPY_BIN += \
FX-4CR.bin \


# All Target
all: main-build

# Main-build Target
main-build: FX-4CR.elf secondary-outputs

# Tool invocations
FX-4CR.elf FX-4CR.map: $(OBJS) $(USER_OBJS) /home/daniel/Documents/workspace/FX-4CR/Core/STM32H743VITX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "FX-4CR.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"/home/daniel/Documents/workspace/FX-4CR/Core/STM32H743VITX_FLASH.ld" --specs=nosys.specs -Wl,-Map="FX-4CR.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

FX-4CR.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "FX-4CR.list"
	@echo 'Finished building: $@'
	@echo ' '

FX-4CR.hex: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "FX-4CR.hex"
	@echo 'Finished building: $@'
	@echo ' '

FX-4CR.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "FX-4CR.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) FX-4CR.bin FX-4CR.elf FX-4CR.hex FX-4CR.list FX-4CR.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
