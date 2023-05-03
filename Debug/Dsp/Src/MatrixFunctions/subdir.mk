################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dsp/Src/MatrixFunctions/arm_mat_add_f32.c \
../Dsp/Src/MatrixFunctions/arm_mat_add_q15.c \
../Dsp/Src/MatrixFunctions/arm_mat_add_q31.c \
../Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_f32.c \
../Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q15.c \
../Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q31.c \
../Dsp/Src/MatrixFunctions/arm_mat_init_f32.c \
../Dsp/Src/MatrixFunctions/arm_mat_init_q15.c \
../Dsp/Src/MatrixFunctions/arm_mat_init_q31.c \
../Dsp/Src/MatrixFunctions/arm_mat_inverse_f32.c \
../Dsp/Src/MatrixFunctions/arm_mat_inverse_f64.c \
../Dsp/Src/MatrixFunctions/arm_mat_mult_f32.c \
../Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q15.c \
../Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q31.c \
../Dsp/Src/MatrixFunctions/arm_mat_mult_q15.c \
../Dsp/Src/MatrixFunctions/arm_mat_mult_q31.c \
../Dsp/Src/MatrixFunctions/arm_mat_scale_f32.c \
../Dsp/Src/MatrixFunctions/arm_mat_scale_q15.c \
../Dsp/Src/MatrixFunctions/arm_mat_scale_q31.c \
../Dsp/Src/MatrixFunctions/arm_mat_sub_f32.c \
../Dsp/Src/MatrixFunctions/arm_mat_sub_q15.c \
../Dsp/Src/MatrixFunctions/arm_mat_sub_q31.c \
../Dsp/Src/MatrixFunctions/arm_mat_trans_f32.c \
../Dsp/Src/MatrixFunctions/arm_mat_trans_q15.c \
../Dsp/Src/MatrixFunctions/arm_mat_trans_q31.c 

OBJS += \
./Dsp/Src/MatrixFunctions/arm_mat_add_f32.o \
./Dsp/Src/MatrixFunctions/arm_mat_add_q15.o \
./Dsp/Src/MatrixFunctions/arm_mat_add_q31.o \
./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_f32.o \
./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q15.o \
./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q31.o \
./Dsp/Src/MatrixFunctions/arm_mat_init_f32.o \
./Dsp/Src/MatrixFunctions/arm_mat_init_q15.o \
./Dsp/Src/MatrixFunctions/arm_mat_init_q31.o \
./Dsp/Src/MatrixFunctions/arm_mat_inverse_f32.o \
./Dsp/Src/MatrixFunctions/arm_mat_inverse_f64.o \
./Dsp/Src/MatrixFunctions/arm_mat_mult_f32.o \
./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q15.o \
./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q31.o \
./Dsp/Src/MatrixFunctions/arm_mat_mult_q15.o \
./Dsp/Src/MatrixFunctions/arm_mat_mult_q31.o \
./Dsp/Src/MatrixFunctions/arm_mat_scale_f32.o \
./Dsp/Src/MatrixFunctions/arm_mat_scale_q15.o \
./Dsp/Src/MatrixFunctions/arm_mat_scale_q31.o \
./Dsp/Src/MatrixFunctions/arm_mat_sub_f32.o \
./Dsp/Src/MatrixFunctions/arm_mat_sub_q15.o \
./Dsp/Src/MatrixFunctions/arm_mat_sub_q31.o \
./Dsp/Src/MatrixFunctions/arm_mat_trans_f32.o \
./Dsp/Src/MatrixFunctions/arm_mat_trans_q15.o \
./Dsp/Src/MatrixFunctions/arm_mat_trans_q31.o 

C_DEPS += \
./Dsp/Src/MatrixFunctions/arm_mat_add_f32.d \
./Dsp/Src/MatrixFunctions/arm_mat_add_q15.d \
./Dsp/Src/MatrixFunctions/arm_mat_add_q31.d \
./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_f32.d \
./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q15.d \
./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q31.d \
./Dsp/Src/MatrixFunctions/arm_mat_init_f32.d \
./Dsp/Src/MatrixFunctions/arm_mat_init_q15.d \
./Dsp/Src/MatrixFunctions/arm_mat_init_q31.d \
./Dsp/Src/MatrixFunctions/arm_mat_inverse_f32.d \
./Dsp/Src/MatrixFunctions/arm_mat_inverse_f64.d \
./Dsp/Src/MatrixFunctions/arm_mat_mult_f32.d \
./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q15.d \
./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q31.d \
./Dsp/Src/MatrixFunctions/arm_mat_mult_q15.d \
./Dsp/Src/MatrixFunctions/arm_mat_mult_q31.d \
./Dsp/Src/MatrixFunctions/arm_mat_scale_f32.d \
./Dsp/Src/MatrixFunctions/arm_mat_scale_q15.d \
./Dsp/Src/MatrixFunctions/arm_mat_scale_q31.d \
./Dsp/Src/MatrixFunctions/arm_mat_sub_f32.d \
./Dsp/Src/MatrixFunctions/arm_mat_sub_q15.d \
./Dsp/Src/MatrixFunctions/arm_mat_sub_q31.d \
./Dsp/Src/MatrixFunctions/arm_mat_trans_f32.d \
./Dsp/Src/MatrixFunctions/arm_mat_trans_q15.d \
./Dsp/Src/MatrixFunctions/arm_mat_trans_q31.d 


# Each subdirectory must supply rules for building sources it contributes
Dsp/Src/MatrixFunctions/%.o Dsp/Src/MatrixFunctions/%.su Dsp/Src/MatrixFunctions/%.cyclo: ../Dsp/Src/MatrixFunctions/%.c Dsp/Src/MatrixFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H743xx -c -I"/home/daniel/Documents/workspace/FX-4CR/User/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Core/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Dsp/Inc" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Dsp-2f-Src-2f-MatrixFunctions

clean-Dsp-2f-Src-2f-MatrixFunctions:
	-$(RM) ./Dsp/Src/MatrixFunctions/arm_mat_add_f32.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_add_f32.d ./Dsp/Src/MatrixFunctions/arm_mat_add_f32.o ./Dsp/Src/MatrixFunctions/arm_mat_add_f32.su ./Dsp/Src/MatrixFunctions/arm_mat_add_q15.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_add_q15.d ./Dsp/Src/MatrixFunctions/arm_mat_add_q15.o ./Dsp/Src/MatrixFunctions/arm_mat_add_q15.su ./Dsp/Src/MatrixFunctions/arm_mat_add_q31.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_add_q31.d ./Dsp/Src/MatrixFunctions/arm_mat_add_q31.o ./Dsp/Src/MatrixFunctions/arm_mat_add_q31.su ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_f32.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_f32.d ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_f32.o ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_f32.su ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q15.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q15.d ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q15.o ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q15.su ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q31.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q31.d ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q31.o ./Dsp/Src/MatrixFunctions/arm_mat_cmplx_mult_q31.su ./Dsp/Src/MatrixFunctions/arm_mat_init_f32.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_init_f32.d ./Dsp/Src/MatrixFunctions/arm_mat_init_f32.o ./Dsp/Src/MatrixFunctions/arm_mat_init_f32.su ./Dsp/Src/MatrixFunctions/arm_mat_init_q15.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_init_q15.d ./Dsp/Src/MatrixFunctions/arm_mat_init_q15.o ./Dsp/Src/MatrixFunctions/arm_mat_init_q15.su ./Dsp/Src/MatrixFunctions/arm_mat_init_q31.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_init_q31.d ./Dsp/Src/MatrixFunctions/arm_mat_init_q31.o ./Dsp/Src/MatrixFunctions/arm_mat_init_q31.su ./Dsp/Src/MatrixFunctions/arm_mat_inverse_f32.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_inverse_f32.d ./Dsp/Src/MatrixFunctions/arm_mat_inverse_f32.o ./Dsp/Src/MatrixFunctions/arm_mat_inverse_f32.su ./Dsp/Src/MatrixFunctions/arm_mat_inverse_f64.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_inverse_f64.d ./Dsp/Src/MatrixFunctions/arm_mat_inverse_f64.o ./Dsp/Src/MatrixFunctions/arm_mat_inverse_f64.su ./Dsp/Src/MatrixFunctions/arm_mat_mult_f32.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_mult_f32.d ./Dsp/Src/MatrixFunctions/arm_mat_mult_f32.o ./Dsp/Src/MatrixFunctions/arm_mat_mult_f32.su ./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q15.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q15.d ./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q15.o ./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q15.su ./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q31.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q31.d ./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q31.o ./Dsp/Src/MatrixFunctions/arm_mat_mult_fast_q31.su ./Dsp/Src/MatrixFunctions/arm_mat_mult_q15.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_mult_q15.d ./Dsp/Src/MatrixFunctions/arm_mat_mult_q15.o ./Dsp/Src/MatrixFunctions/arm_mat_mult_q15.su ./Dsp/Src/MatrixFunctions/arm_mat_mult_q31.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_mult_q31.d ./Dsp/Src/MatrixFunctions/arm_mat_mult_q31.o ./Dsp/Src/MatrixFunctions/arm_mat_mult_q31.su ./Dsp/Src/MatrixFunctions/arm_mat_scale_f32.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_scale_f32.d ./Dsp/Src/MatrixFunctions/arm_mat_scale_f32.o ./Dsp/Src/MatrixFunctions/arm_mat_scale_f32.su ./Dsp/Src/MatrixFunctions/arm_mat_scale_q15.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_scale_q15.d ./Dsp/Src/MatrixFunctions/arm_mat_scale_q15.o ./Dsp/Src/MatrixFunctions/arm_mat_scale_q15.su ./Dsp/Src/MatrixFunctions/arm_mat_scale_q31.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_scale_q31.d ./Dsp/Src/MatrixFunctions/arm_mat_scale_q31.o ./Dsp/Src/MatrixFunctions/arm_mat_scale_q31.su ./Dsp/Src/MatrixFunctions/arm_mat_sub_f32.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_sub_f32.d ./Dsp/Src/MatrixFunctions/arm_mat_sub_f32.o ./Dsp/Src/MatrixFunctions/arm_mat_sub_f32.su ./Dsp/Src/MatrixFunctions/arm_mat_sub_q15.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_sub_q15.d ./Dsp/Src/MatrixFunctions/arm_mat_sub_q15.o ./Dsp/Src/MatrixFunctions/arm_mat_sub_q15.su ./Dsp/Src/MatrixFunctions/arm_mat_sub_q31.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_sub_q31.d ./Dsp/Src/MatrixFunctions/arm_mat_sub_q31.o ./Dsp/Src/MatrixFunctions/arm_mat_sub_q31.su ./Dsp/Src/MatrixFunctions/arm_mat_trans_f32.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_trans_f32.d ./Dsp/Src/MatrixFunctions/arm_mat_trans_f32.o ./Dsp/Src/MatrixFunctions/arm_mat_trans_f32.su ./Dsp/Src/MatrixFunctions/arm_mat_trans_q15.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_trans_q15.d ./Dsp/Src/MatrixFunctions/arm_mat_trans_q15.o ./Dsp/Src/MatrixFunctions/arm_mat_trans_q15.su ./Dsp/Src/MatrixFunctions/arm_mat_trans_q31.cyclo ./Dsp/Src/MatrixFunctions/arm_mat_trans_q31.d ./Dsp/Src/MatrixFunctions/arm_mat_trans_q31.o ./Dsp/Src/MatrixFunctions/arm_mat_trans_q31.su

.PHONY: clean-Dsp-2f-Src-2f-MatrixFunctions

