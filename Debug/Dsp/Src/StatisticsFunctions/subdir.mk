################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Dsp/Src/StatisticsFunctions/arm_max_f32.c \
../Dsp/Src/StatisticsFunctions/arm_max_q15.c \
../Dsp/Src/StatisticsFunctions/arm_max_q31.c \
../Dsp/Src/StatisticsFunctions/arm_max_q7.c \
../Dsp/Src/StatisticsFunctions/arm_mean_f32.c \
../Dsp/Src/StatisticsFunctions/arm_mean_q15.c \
../Dsp/Src/StatisticsFunctions/arm_mean_q31.c \
../Dsp/Src/StatisticsFunctions/arm_mean_q7.c \
../Dsp/Src/StatisticsFunctions/arm_min_f32.c \
../Dsp/Src/StatisticsFunctions/arm_min_q15.c \
../Dsp/Src/StatisticsFunctions/arm_min_q31.c \
../Dsp/Src/StatisticsFunctions/arm_min_q7.c \
../Dsp/Src/StatisticsFunctions/arm_power_f32.c \
../Dsp/Src/StatisticsFunctions/arm_power_q15.c \
../Dsp/Src/StatisticsFunctions/arm_power_q31.c \
../Dsp/Src/StatisticsFunctions/arm_power_q7.c \
../Dsp/Src/StatisticsFunctions/arm_rms_f32.c \
../Dsp/Src/StatisticsFunctions/arm_rms_q15.c \
../Dsp/Src/StatisticsFunctions/arm_rms_q31.c \
../Dsp/Src/StatisticsFunctions/arm_std_f32.c \
../Dsp/Src/StatisticsFunctions/arm_std_q15.c \
../Dsp/Src/StatisticsFunctions/arm_std_q31.c \
../Dsp/Src/StatisticsFunctions/arm_var_f32.c \
../Dsp/Src/StatisticsFunctions/arm_var_q15.c \
../Dsp/Src/StatisticsFunctions/arm_var_q31.c 

OBJS += \
./Dsp/Src/StatisticsFunctions/arm_max_f32.o \
./Dsp/Src/StatisticsFunctions/arm_max_q15.o \
./Dsp/Src/StatisticsFunctions/arm_max_q31.o \
./Dsp/Src/StatisticsFunctions/arm_max_q7.o \
./Dsp/Src/StatisticsFunctions/arm_mean_f32.o \
./Dsp/Src/StatisticsFunctions/arm_mean_q15.o \
./Dsp/Src/StatisticsFunctions/arm_mean_q31.o \
./Dsp/Src/StatisticsFunctions/arm_mean_q7.o \
./Dsp/Src/StatisticsFunctions/arm_min_f32.o \
./Dsp/Src/StatisticsFunctions/arm_min_q15.o \
./Dsp/Src/StatisticsFunctions/arm_min_q31.o \
./Dsp/Src/StatisticsFunctions/arm_min_q7.o \
./Dsp/Src/StatisticsFunctions/arm_power_f32.o \
./Dsp/Src/StatisticsFunctions/arm_power_q15.o \
./Dsp/Src/StatisticsFunctions/arm_power_q31.o \
./Dsp/Src/StatisticsFunctions/arm_power_q7.o \
./Dsp/Src/StatisticsFunctions/arm_rms_f32.o \
./Dsp/Src/StatisticsFunctions/arm_rms_q15.o \
./Dsp/Src/StatisticsFunctions/arm_rms_q31.o \
./Dsp/Src/StatisticsFunctions/arm_std_f32.o \
./Dsp/Src/StatisticsFunctions/arm_std_q15.o \
./Dsp/Src/StatisticsFunctions/arm_std_q31.o \
./Dsp/Src/StatisticsFunctions/arm_var_f32.o \
./Dsp/Src/StatisticsFunctions/arm_var_q15.o \
./Dsp/Src/StatisticsFunctions/arm_var_q31.o 

C_DEPS += \
./Dsp/Src/StatisticsFunctions/arm_max_f32.d \
./Dsp/Src/StatisticsFunctions/arm_max_q15.d \
./Dsp/Src/StatisticsFunctions/arm_max_q31.d \
./Dsp/Src/StatisticsFunctions/arm_max_q7.d \
./Dsp/Src/StatisticsFunctions/arm_mean_f32.d \
./Dsp/Src/StatisticsFunctions/arm_mean_q15.d \
./Dsp/Src/StatisticsFunctions/arm_mean_q31.d \
./Dsp/Src/StatisticsFunctions/arm_mean_q7.d \
./Dsp/Src/StatisticsFunctions/arm_min_f32.d \
./Dsp/Src/StatisticsFunctions/arm_min_q15.d \
./Dsp/Src/StatisticsFunctions/arm_min_q31.d \
./Dsp/Src/StatisticsFunctions/arm_min_q7.d \
./Dsp/Src/StatisticsFunctions/arm_power_f32.d \
./Dsp/Src/StatisticsFunctions/arm_power_q15.d \
./Dsp/Src/StatisticsFunctions/arm_power_q31.d \
./Dsp/Src/StatisticsFunctions/arm_power_q7.d \
./Dsp/Src/StatisticsFunctions/arm_rms_f32.d \
./Dsp/Src/StatisticsFunctions/arm_rms_q15.d \
./Dsp/Src/StatisticsFunctions/arm_rms_q31.d \
./Dsp/Src/StatisticsFunctions/arm_std_f32.d \
./Dsp/Src/StatisticsFunctions/arm_std_q15.d \
./Dsp/Src/StatisticsFunctions/arm_std_q31.d \
./Dsp/Src/StatisticsFunctions/arm_var_f32.d \
./Dsp/Src/StatisticsFunctions/arm_var_q15.d \
./Dsp/Src/StatisticsFunctions/arm_var_q31.d 


# Each subdirectory must supply rules for building sources it contributes
Dsp/Src/StatisticsFunctions/%.o Dsp/Src/StatisticsFunctions/%.su Dsp/Src/StatisticsFunctions/%.cyclo: ../Dsp/Src/StatisticsFunctions/%.c Dsp/Src/StatisticsFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H743xx -c -I"/home/daniel/Documents/workspace/FX-4CR/User/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Core/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Dsp/Inc" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Dsp-2f-Src-2f-StatisticsFunctions

clean-Dsp-2f-Src-2f-StatisticsFunctions:
	-$(RM) ./Dsp/Src/StatisticsFunctions/arm_max_f32.cyclo ./Dsp/Src/StatisticsFunctions/arm_max_f32.d ./Dsp/Src/StatisticsFunctions/arm_max_f32.o ./Dsp/Src/StatisticsFunctions/arm_max_f32.su ./Dsp/Src/StatisticsFunctions/arm_max_q15.cyclo ./Dsp/Src/StatisticsFunctions/arm_max_q15.d ./Dsp/Src/StatisticsFunctions/arm_max_q15.o ./Dsp/Src/StatisticsFunctions/arm_max_q15.su ./Dsp/Src/StatisticsFunctions/arm_max_q31.cyclo ./Dsp/Src/StatisticsFunctions/arm_max_q31.d ./Dsp/Src/StatisticsFunctions/arm_max_q31.o ./Dsp/Src/StatisticsFunctions/arm_max_q31.su ./Dsp/Src/StatisticsFunctions/arm_max_q7.cyclo ./Dsp/Src/StatisticsFunctions/arm_max_q7.d ./Dsp/Src/StatisticsFunctions/arm_max_q7.o ./Dsp/Src/StatisticsFunctions/arm_max_q7.su ./Dsp/Src/StatisticsFunctions/arm_mean_f32.cyclo ./Dsp/Src/StatisticsFunctions/arm_mean_f32.d ./Dsp/Src/StatisticsFunctions/arm_mean_f32.o ./Dsp/Src/StatisticsFunctions/arm_mean_f32.su ./Dsp/Src/StatisticsFunctions/arm_mean_q15.cyclo ./Dsp/Src/StatisticsFunctions/arm_mean_q15.d ./Dsp/Src/StatisticsFunctions/arm_mean_q15.o ./Dsp/Src/StatisticsFunctions/arm_mean_q15.su ./Dsp/Src/StatisticsFunctions/arm_mean_q31.cyclo ./Dsp/Src/StatisticsFunctions/arm_mean_q31.d ./Dsp/Src/StatisticsFunctions/arm_mean_q31.o ./Dsp/Src/StatisticsFunctions/arm_mean_q31.su ./Dsp/Src/StatisticsFunctions/arm_mean_q7.cyclo ./Dsp/Src/StatisticsFunctions/arm_mean_q7.d ./Dsp/Src/StatisticsFunctions/arm_mean_q7.o ./Dsp/Src/StatisticsFunctions/arm_mean_q7.su ./Dsp/Src/StatisticsFunctions/arm_min_f32.cyclo ./Dsp/Src/StatisticsFunctions/arm_min_f32.d ./Dsp/Src/StatisticsFunctions/arm_min_f32.o ./Dsp/Src/StatisticsFunctions/arm_min_f32.su ./Dsp/Src/StatisticsFunctions/arm_min_q15.cyclo ./Dsp/Src/StatisticsFunctions/arm_min_q15.d ./Dsp/Src/StatisticsFunctions/arm_min_q15.o ./Dsp/Src/StatisticsFunctions/arm_min_q15.su ./Dsp/Src/StatisticsFunctions/arm_min_q31.cyclo ./Dsp/Src/StatisticsFunctions/arm_min_q31.d ./Dsp/Src/StatisticsFunctions/arm_min_q31.o ./Dsp/Src/StatisticsFunctions/arm_min_q31.su ./Dsp/Src/StatisticsFunctions/arm_min_q7.cyclo ./Dsp/Src/StatisticsFunctions/arm_min_q7.d ./Dsp/Src/StatisticsFunctions/arm_min_q7.o ./Dsp/Src/StatisticsFunctions/arm_min_q7.su ./Dsp/Src/StatisticsFunctions/arm_power_f32.cyclo ./Dsp/Src/StatisticsFunctions/arm_power_f32.d ./Dsp/Src/StatisticsFunctions/arm_power_f32.o ./Dsp/Src/StatisticsFunctions/arm_power_f32.su ./Dsp/Src/StatisticsFunctions/arm_power_q15.cyclo ./Dsp/Src/StatisticsFunctions/arm_power_q15.d ./Dsp/Src/StatisticsFunctions/arm_power_q15.o ./Dsp/Src/StatisticsFunctions/arm_power_q15.su ./Dsp/Src/StatisticsFunctions/arm_power_q31.cyclo ./Dsp/Src/StatisticsFunctions/arm_power_q31.d ./Dsp/Src/StatisticsFunctions/arm_power_q31.o ./Dsp/Src/StatisticsFunctions/arm_power_q31.su ./Dsp/Src/StatisticsFunctions/arm_power_q7.cyclo ./Dsp/Src/StatisticsFunctions/arm_power_q7.d ./Dsp/Src/StatisticsFunctions/arm_power_q7.o ./Dsp/Src/StatisticsFunctions/arm_power_q7.su ./Dsp/Src/StatisticsFunctions/arm_rms_f32.cyclo ./Dsp/Src/StatisticsFunctions/arm_rms_f32.d ./Dsp/Src/StatisticsFunctions/arm_rms_f32.o ./Dsp/Src/StatisticsFunctions/arm_rms_f32.su ./Dsp/Src/StatisticsFunctions/arm_rms_q15.cyclo ./Dsp/Src/StatisticsFunctions/arm_rms_q15.d ./Dsp/Src/StatisticsFunctions/arm_rms_q15.o ./Dsp/Src/StatisticsFunctions/arm_rms_q15.su ./Dsp/Src/StatisticsFunctions/arm_rms_q31.cyclo ./Dsp/Src/StatisticsFunctions/arm_rms_q31.d ./Dsp/Src/StatisticsFunctions/arm_rms_q31.o ./Dsp/Src/StatisticsFunctions/arm_rms_q31.su ./Dsp/Src/StatisticsFunctions/arm_std_f32.cyclo ./Dsp/Src/StatisticsFunctions/arm_std_f32.d ./Dsp/Src/StatisticsFunctions/arm_std_f32.o ./Dsp/Src/StatisticsFunctions/arm_std_f32.su ./Dsp/Src/StatisticsFunctions/arm_std_q15.cyclo ./Dsp/Src/StatisticsFunctions/arm_std_q15.d ./Dsp/Src/StatisticsFunctions/arm_std_q15.o ./Dsp/Src/StatisticsFunctions/arm_std_q15.su ./Dsp/Src/StatisticsFunctions/arm_std_q31.cyclo ./Dsp/Src/StatisticsFunctions/arm_std_q31.d ./Dsp/Src/StatisticsFunctions/arm_std_q31.o ./Dsp/Src/StatisticsFunctions/arm_std_q31.su ./Dsp/Src/StatisticsFunctions/arm_var_f32.cyclo ./Dsp/Src/StatisticsFunctions/arm_var_f32.d ./Dsp/Src/StatisticsFunctions/arm_var_f32.o ./Dsp/Src/StatisticsFunctions/arm_var_f32.su ./Dsp/Src/StatisticsFunctions/arm_var_q15.cyclo ./Dsp/Src/StatisticsFunctions/arm_var_q15.d ./Dsp/Src/StatisticsFunctions/arm_var_q15.o ./Dsp/Src/StatisticsFunctions/arm_var_q15.su ./Dsp/Src/StatisticsFunctions/arm_var_q31.cyclo ./Dsp/Src/StatisticsFunctions/arm_var_q31.d ./Dsp/Src/StatisticsFunctions/arm_var_q31.o ./Dsp/Src/StatisticsFunctions/arm_var_q31.su

.PHONY: clean-Dsp-2f-Src-2f-StatisticsFunctions

