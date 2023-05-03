################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dsp/Src/SupportFunctions/arm_copy_f32.c \
../Dsp/Src/SupportFunctions/arm_copy_q15.c \
../Dsp/Src/SupportFunctions/arm_copy_q31.c \
../Dsp/Src/SupportFunctions/arm_copy_q7.c \
../Dsp/Src/SupportFunctions/arm_fill_f32.c \
../Dsp/Src/SupportFunctions/arm_fill_q15.c \
../Dsp/Src/SupportFunctions/arm_fill_q31.c \
../Dsp/Src/SupportFunctions/arm_fill_q7.c \
../Dsp/Src/SupportFunctions/arm_float_to_q15.c \
../Dsp/Src/SupportFunctions/arm_float_to_q31.c \
../Dsp/Src/SupportFunctions/arm_float_to_q7.c \
../Dsp/Src/SupportFunctions/arm_q15_to_float.c \
../Dsp/Src/SupportFunctions/arm_q15_to_q31.c \
../Dsp/Src/SupportFunctions/arm_q15_to_q7.c \
../Dsp/Src/SupportFunctions/arm_q31_to_float.c \
../Dsp/Src/SupportFunctions/arm_q31_to_q15.c \
../Dsp/Src/SupportFunctions/arm_q31_to_q7.c \
../Dsp/Src/SupportFunctions/arm_q7_to_float.c \
../Dsp/Src/SupportFunctions/arm_q7_to_q15.c \
../Dsp/Src/SupportFunctions/arm_q7_to_q31.c 

OBJS += \
./Dsp/Src/SupportFunctions/arm_copy_f32.o \
./Dsp/Src/SupportFunctions/arm_copy_q15.o \
./Dsp/Src/SupportFunctions/arm_copy_q31.o \
./Dsp/Src/SupportFunctions/arm_copy_q7.o \
./Dsp/Src/SupportFunctions/arm_fill_f32.o \
./Dsp/Src/SupportFunctions/arm_fill_q15.o \
./Dsp/Src/SupportFunctions/arm_fill_q31.o \
./Dsp/Src/SupportFunctions/arm_fill_q7.o \
./Dsp/Src/SupportFunctions/arm_float_to_q15.o \
./Dsp/Src/SupportFunctions/arm_float_to_q31.o \
./Dsp/Src/SupportFunctions/arm_float_to_q7.o \
./Dsp/Src/SupportFunctions/arm_q15_to_float.o \
./Dsp/Src/SupportFunctions/arm_q15_to_q31.o \
./Dsp/Src/SupportFunctions/arm_q15_to_q7.o \
./Dsp/Src/SupportFunctions/arm_q31_to_float.o \
./Dsp/Src/SupportFunctions/arm_q31_to_q15.o \
./Dsp/Src/SupportFunctions/arm_q31_to_q7.o \
./Dsp/Src/SupportFunctions/arm_q7_to_float.o \
./Dsp/Src/SupportFunctions/arm_q7_to_q15.o \
./Dsp/Src/SupportFunctions/arm_q7_to_q31.o 

C_DEPS += \
./Dsp/Src/SupportFunctions/arm_copy_f32.d \
./Dsp/Src/SupportFunctions/arm_copy_q15.d \
./Dsp/Src/SupportFunctions/arm_copy_q31.d \
./Dsp/Src/SupportFunctions/arm_copy_q7.d \
./Dsp/Src/SupportFunctions/arm_fill_f32.d \
./Dsp/Src/SupportFunctions/arm_fill_q15.d \
./Dsp/Src/SupportFunctions/arm_fill_q31.d \
./Dsp/Src/SupportFunctions/arm_fill_q7.d \
./Dsp/Src/SupportFunctions/arm_float_to_q15.d \
./Dsp/Src/SupportFunctions/arm_float_to_q31.d \
./Dsp/Src/SupportFunctions/arm_float_to_q7.d \
./Dsp/Src/SupportFunctions/arm_q15_to_float.d \
./Dsp/Src/SupportFunctions/arm_q15_to_q31.d \
./Dsp/Src/SupportFunctions/arm_q15_to_q7.d \
./Dsp/Src/SupportFunctions/arm_q31_to_float.d \
./Dsp/Src/SupportFunctions/arm_q31_to_q15.d \
./Dsp/Src/SupportFunctions/arm_q31_to_q7.d \
./Dsp/Src/SupportFunctions/arm_q7_to_float.d \
./Dsp/Src/SupportFunctions/arm_q7_to_q15.d \
./Dsp/Src/SupportFunctions/arm_q7_to_q31.d 


# Each subdirectory must supply rules for building sources it contributes
Dsp/Src/SupportFunctions/%.o Dsp/Src/SupportFunctions/%.su Dsp/Src/SupportFunctions/%.cyclo: ../Dsp/Src/SupportFunctions/%.c Dsp/Src/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H743xx -c -I"/home/daniel/Documents/workspace/FX-4CR/User/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Core/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Dsp/Inc" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Dsp-2f-Src-2f-SupportFunctions

clean-Dsp-2f-Src-2f-SupportFunctions:
	-$(RM) ./Dsp/Src/SupportFunctions/arm_copy_f32.cyclo ./Dsp/Src/SupportFunctions/arm_copy_f32.d ./Dsp/Src/SupportFunctions/arm_copy_f32.o ./Dsp/Src/SupportFunctions/arm_copy_f32.su ./Dsp/Src/SupportFunctions/arm_copy_q15.cyclo ./Dsp/Src/SupportFunctions/arm_copy_q15.d ./Dsp/Src/SupportFunctions/arm_copy_q15.o ./Dsp/Src/SupportFunctions/arm_copy_q15.su ./Dsp/Src/SupportFunctions/arm_copy_q31.cyclo ./Dsp/Src/SupportFunctions/arm_copy_q31.d ./Dsp/Src/SupportFunctions/arm_copy_q31.o ./Dsp/Src/SupportFunctions/arm_copy_q31.su ./Dsp/Src/SupportFunctions/arm_copy_q7.cyclo ./Dsp/Src/SupportFunctions/arm_copy_q7.d ./Dsp/Src/SupportFunctions/arm_copy_q7.o ./Dsp/Src/SupportFunctions/arm_copy_q7.su ./Dsp/Src/SupportFunctions/arm_fill_f32.cyclo ./Dsp/Src/SupportFunctions/arm_fill_f32.d ./Dsp/Src/SupportFunctions/arm_fill_f32.o ./Dsp/Src/SupportFunctions/arm_fill_f32.su ./Dsp/Src/SupportFunctions/arm_fill_q15.cyclo ./Dsp/Src/SupportFunctions/arm_fill_q15.d ./Dsp/Src/SupportFunctions/arm_fill_q15.o ./Dsp/Src/SupportFunctions/arm_fill_q15.su ./Dsp/Src/SupportFunctions/arm_fill_q31.cyclo ./Dsp/Src/SupportFunctions/arm_fill_q31.d ./Dsp/Src/SupportFunctions/arm_fill_q31.o ./Dsp/Src/SupportFunctions/arm_fill_q31.su ./Dsp/Src/SupportFunctions/arm_fill_q7.cyclo ./Dsp/Src/SupportFunctions/arm_fill_q7.d ./Dsp/Src/SupportFunctions/arm_fill_q7.o ./Dsp/Src/SupportFunctions/arm_fill_q7.su ./Dsp/Src/SupportFunctions/arm_float_to_q15.cyclo ./Dsp/Src/SupportFunctions/arm_float_to_q15.d ./Dsp/Src/SupportFunctions/arm_float_to_q15.o ./Dsp/Src/SupportFunctions/arm_float_to_q15.su ./Dsp/Src/SupportFunctions/arm_float_to_q31.cyclo ./Dsp/Src/SupportFunctions/arm_float_to_q31.d ./Dsp/Src/SupportFunctions/arm_float_to_q31.o ./Dsp/Src/SupportFunctions/arm_float_to_q31.su ./Dsp/Src/SupportFunctions/arm_float_to_q7.cyclo ./Dsp/Src/SupportFunctions/arm_float_to_q7.d ./Dsp/Src/SupportFunctions/arm_float_to_q7.o ./Dsp/Src/SupportFunctions/arm_float_to_q7.su ./Dsp/Src/SupportFunctions/arm_q15_to_float.cyclo ./Dsp/Src/SupportFunctions/arm_q15_to_float.d ./Dsp/Src/SupportFunctions/arm_q15_to_float.o ./Dsp/Src/SupportFunctions/arm_q15_to_float.su ./Dsp/Src/SupportFunctions/arm_q15_to_q31.cyclo ./Dsp/Src/SupportFunctions/arm_q15_to_q31.d ./Dsp/Src/SupportFunctions/arm_q15_to_q31.o ./Dsp/Src/SupportFunctions/arm_q15_to_q31.su ./Dsp/Src/SupportFunctions/arm_q15_to_q7.cyclo ./Dsp/Src/SupportFunctions/arm_q15_to_q7.d ./Dsp/Src/SupportFunctions/arm_q15_to_q7.o ./Dsp/Src/SupportFunctions/arm_q15_to_q7.su ./Dsp/Src/SupportFunctions/arm_q31_to_float.cyclo ./Dsp/Src/SupportFunctions/arm_q31_to_float.d ./Dsp/Src/SupportFunctions/arm_q31_to_float.o ./Dsp/Src/SupportFunctions/arm_q31_to_float.su ./Dsp/Src/SupportFunctions/arm_q31_to_q15.cyclo ./Dsp/Src/SupportFunctions/arm_q31_to_q15.d ./Dsp/Src/SupportFunctions/arm_q31_to_q15.o ./Dsp/Src/SupportFunctions/arm_q31_to_q15.su ./Dsp/Src/SupportFunctions/arm_q31_to_q7.cyclo ./Dsp/Src/SupportFunctions/arm_q31_to_q7.d ./Dsp/Src/SupportFunctions/arm_q31_to_q7.o ./Dsp/Src/SupportFunctions/arm_q31_to_q7.su ./Dsp/Src/SupportFunctions/arm_q7_to_float.cyclo ./Dsp/Src/SupportFunctions/arm_q7_to_float.d ./Dsp/Src/SupportFunctions/arm_q7_to_float.o ./Dsp/Src/SupportFunctions/arm_q7_to_float.su ./Dsp/Src/SupportFunctions/arm_q7_to_q15.cyclo ./Dsp/Src/SupportFunctions/arm_q7_to_q15.d ./Dsp/Src/SupportFunctions/arm_q7_to_q15.o ./Dsp/Src/SupportFunctions/arm_q7_to_q15.su ./Dsp/Src/SupportFunctions/arm_q7_to_q31.cyclo ./Dsp/Src/SupportFunctions/arm_q7_to_q31.d ./Dsp/Src/SupportFunctions/arm_q7_to_q31.o ./Dsp/Src/SupportFunctions/arm_q7_to_q31.su

.PHONY: clean-Dsp-2f-Src-2f-SupportFunctions

