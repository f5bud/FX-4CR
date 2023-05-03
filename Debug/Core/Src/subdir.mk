################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Src/startup_stm32h743xx.s 

C_SRCS += \
../Core/Src/system_stm32h7xx.c 

OBJS += \
./Core/Src/startup_stm32h743xx.o \
./Core/Src/system_stm32h7xx.o 

S_DEPS += \
./Core/Src/startup_stm32h743xx.d 

C_DEPS += \
./Core/Src/system_stm32h7xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.s Core/Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -c -I"/home/daniel/Documents/workspace/FX-4CR/User/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Core/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Dsp/Inc" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H743xx -c -I"/home/daniel/Documents/workspace/FX-4CR/User/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Core/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Dsp/Inc" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/startup_stm32h743xx.d ./Core/Src/startup_stm32h743xx.o ./Core/Src/system_stm32h7xx.cyclo ./Core/Src/system_stm32h7xx.d ./Core/Src/system_stm32h7xx.o ./Core/Src/system_stm32h7xx.su

.PHONY: clean-Core-2f-Src

