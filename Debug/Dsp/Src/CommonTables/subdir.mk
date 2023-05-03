################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dsp/Src/CommonTables/arm_common_tables.c \
../Dsp/Src/CommonTables/arm_const_structs.c 

OBJS += \
./Dsp/Src/CommonTables/arm_common_tables.o \
./Dsp/Src/CommonTables/arm_const_structs.o 

C_DEPS += \
./Dsp/Src/CommonTables/arm_common_tables.d \
./Dsp/Src/CommonTables/arm_const_structs.d 


# Each subdirectory must supply rules for building sources it contributes
Dsp/Src/CommonTables/%.o Dsp/Src/CommonTables/%.su Dsp/Src/CommonTables/%.cyclo: ../Dsp/Src/CommonTables/%.c Dsp/Src/CommonTables/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H743xx -c -I"/home/daniel/Documents/workspace/FX-4CR/User/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Core/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Dsp/Inc" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Dsp-2f-Src-2f-CommonTables

clean-Dsp-2f-Src-2f-CommonTables:
	-$(RM) ./Dsp/Src/CommonTables/arm_common_tables.cyclo ./Dsp/Src/CommonTables/arm_common_tables.d ./Dsp/Src/CommonTables/arm_common_tables.o ./Dsp/Src/CommonTables/arm_common_tables.su ./Dsp/Src/CommonTables/arm_const_structs.cyclo ./Dsp/Src/CommonTables/arm_const_structs.d ./Dsp/Src/CommonTables/arm_const_structs.o ./Dsp/Src/CommonTables/arm_const_structs.su

.PHONY: clean-Dsp-2f-Src-2f-CommonTables

