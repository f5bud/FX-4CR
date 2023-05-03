################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dsp/Src/FastMathFunctions/arm_cos_f32.c \
../Dsp/Src/FastMathFunctions/arm_cos_q15.c \
../Dsp/Src/FastMathFunctions/arm_cos_q31.c \
../Dsp/Src/FastMathFunctions/arm_sin_f32.c \
../Dsp/Src/FastMathFunctions/arm_sin_q15.c \
../Dsp/Src/FastMathFunctions/arm_sin_q31.c \
../Dsp/Src/FastMathFunctions/arm_sqrt_q15.c \
../Dsp/Src/FastMathFunctions/arm_sqrt_q31.c 

OBJS += \
./Dsp/Src/FastMathFunctions/arm_cos_f32.o \
./Dsp/Src/FastMathFunctions/arm_cos_q15.o \
./Dsp/Src/FastMathFunctions/arm_cos_q31.o \
./Dsp/Src/FastMathFunctions/arm_sin_f32.o \
./Dsp/Src/FastMathFunctions/arm_sin_q15.o \
./Dsp/Src/FastMathFunctions/arm_sin_q31.o \
./Dsp/Src/FastMathFunctions/arm_sqrt_q15.o \
./Dsp/Src/FastMathFunctions/arm_sqrt_q31.o 

C_DEPS += \
./Dsp/Src/FastMathFunctions/arm_cos_f32.d \
./Dsp/Src/FastMathFunctions/arm_cos_q15.d \
./Dsp/Src/FastMathFunctions/arm_cos_q31.d \
./Dsp/Src/FastMathFunctions/arm_sin_f32.d \
./Dsp/Src/FastMathFunctions/arm_sin_q15.d \
./Dsp/Src/FastMathFunctions/arm_sin_q31.d \
./Dsp/Src/FastMathFunctions/arm_sqrt_q15.d \
./Dsp/Src/FastMathFunctions/arm_sqrt_q31.d 


# Each subdirectory must supply rules for building sources it contributes
Dsp/Src/FastMathFunctions/%.o Dsp/Src/FastMathFunctions/%.su Dsp/Src/FastMathFunctions/%.cyclo: ../Dsp/Src/FastMathFunctions/%.c Dsp/Src/FastMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H743xx -c -I"/home/daniel/Documents/workspace/FX-4CR/User/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Core/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Dsp/Inc" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Dsp-2f-Src-2f-FastMathFunctions

clean-Dsp-2f-Src-2f-FastMathFunctions:
	-$(RM) ./Dsp/Src/FastMathFunctions/arm_cos_f32.cyclo ./Dsp/Src/FastMathFunctions/arm_cos_f32.d ./Dsp/Src/FastMathFunctions/arm_cos_f32.o ./Dsp/Src/FastMathFunctions/arm_cos_f32.su ./Dsp/Src/FastMathFunctions/arm_cos_q15.cyclo ./Dsp/Src/FastMathFunctions/arm_cos_q15.d ./Dsp/Src/FastMathFunctions/arm_cos_q15.o ./Dsp/Src/FastMathFunctions/arm_cos_q15.su ./Dsp/Src/FastMathFunctions/arm_cos_q31.cyclo ./Dsp/Src/FastMathFunctions/arm_cos_q31.d ./Dsp/Src/FastMathFunctions/arm_cos_q31.o ./Dsp/Src/FastMathFunctions/arm_cos_q31.su ./Dsp/Src/FastMathFunctions/arm_sin_f32.cyclo ./Dsp/Src/FastMathFunctions/arm_sin_f32.d ./Dsp/Src/FastMathFunctions/arm_sin_f32.o ./Dsp/Src/FastMathFunctions/arm_sin_f32.su ./Dsp/Src/FastMathFunctions/arm_sin_q15.cyclo ./Dsp/Src/FastMathFunctions/arm_sin_q15.d ./Dsp/Src/FastMathFunctions/arm_sin_q15.o ./Dsp/Src/FastMathFunctions/arm_sin_q15.su ./Dsp/Src/FastMathFunctions/arm_sin_q31.cyclo ./Dsp/Src/FastMathFunctions/arm_sin_q31.d ./Dsp/Src/FastMathFunctions/arm_sin_q31.o ./Dsp/Src/FastMathFunctions/arm_sin_q31.su ./Dsp/Src/FastMathFunctions/arm_sqrt_q15.cyclo ./Dsp/Src/FastMathFunctions/arm_sqrt_q15.d ./Dsp/Src/FastMathFunctions/arm_sqrt_q15.o ./Dsp/Src/FastMathFunctions/arm_sqrt_q15.su ./Dsp/Src/FastMathFunctions/arm_sqrt_q31.cyclo ./Dsp/Src/FastMathFunctions/arm_sqrt_q31.d ./Dsp/Src/FastMathFunctions/arm_sqrt_q31.o ./Dsp/Src/FastMathFunctions/arm_sqrt_q31.su

.PHONY: clean-Dsp-2f-Src-2f-FastMathFunctions

