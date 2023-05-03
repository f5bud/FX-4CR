################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_f32.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q15.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q31.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_f32.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q15.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q31.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_f32.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q15.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q31.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_f32.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q15.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q31.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_f32.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q15.c \
../Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q31.c 

OBJS += \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_f32.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q15.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q31.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_f32.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q15.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q31.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_f32.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q15.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q31.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_f32.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q15.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q31.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_f32.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q15.o \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q31.o 

C_DEPS += \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_f32.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q15.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q31.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_f32.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q15.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q31.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_f32.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q15.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q31.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_f32.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q15.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q31.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_f32.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q15.d \
./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q31.d 


# Each subdirectory must supply rules for building sources it contributes
Dsp/Src/ComplexMathFunctions/%.o Dsp/Src/ComplexMathFunctions/%.su Dsp/Src/ComplexMathFunctions/%.cyclo: ../Dsp/Src/ComplexMathFunctions/%.c Dsp/Src/ComplexMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H743xx -c -I"/home/daniel/Documents/workspace/FX-4CR/User/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Core/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Dsp/Inc" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Dsp-2f-Src-2f-ComplexMathFunctions

clean-Dsp-2f-Src-2f-ComplexMathFunctions:
	-$(RM) ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_f32.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_f32.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_f32.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_f32.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q15.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q15.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q15.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q15.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q31.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q31.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q31.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_conj_q31.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_f32.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_f32.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_f32.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_f32.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q15.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q15.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q15.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q15.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q31.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q31.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q31.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_dot_prod_q31.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_f32.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_f32.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_f32.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_f32.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q15.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q15.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q15.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q15.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q31.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q31.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q31.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_q31.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_f32.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_f32.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_f32.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_f32.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q15.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q15.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q15.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q15.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q31.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q31.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q31.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mag_squared_q31.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_f32.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q15.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_cmplx_q31.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_f32.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_f32.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_f32.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_f32.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q15.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q15.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q15.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q15.su ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q31.cyclo ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q31.d ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q31.o ./Dsp/Src/ComplexMathFunctions/arm_cmplx_mult_real_q31.su

.PHONY: clean-Dsp-2f-Src-2f-ComplexMathFunctions

