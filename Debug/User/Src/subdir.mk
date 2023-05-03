################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Src/24cxx.c \
../User/Src/Audio_driver.c \
../User/Src/KEY_Control.c \
../User/Src/Si5351AI2C.c \
../User/Src/Softdds.c \
../User/Src/adc.c \
../User/Src/cat_kenwood.c \
../User/Src/dac.c \
../User/Src/dma.c \
../User/Src/dsp_filter.c \
../User/Src/dwt.c \
../User/Src/encode.c \
../User/Src/exti.c \
../User/Src/filter.c \
../User/Src/gpio_set.c \
../User/Src/key.c \
../User/Src/lcd.c \
../User/Src/led.c \
../User/Src/main.c \
../User/Src/mpu.c \
../User/Src/myiic.c \
../User/Src/rtc.c \
../User/Src/sai.c \
../User/Src/sys.c \
../User/Src/timer.c \
../User/Src/tpad.c \
../User/Src/ui_Driver.c \
../User/Src/usart.c \
../User/Src/wdg.c \
../User/Src/wm8978.c 

OBJS += \
./User/Src/24cxx.o \
./User/Src/Audio_driver.o \
./User/Src/KEY_Control.o \
./User/Src/Si5351AI2C.o \
./User/Src/Softdds.o \
./User/Src/adc.o \
./User/Src/cat_kenwood.o \
./User/Src/dac.o \
./User/Src/dma.o \
./User/Src/dsp_filter.o \
./User/Src/dwt.o \
./User/Src/encode.o \
./User/Src/exti.o \
./User/Src/filter.o \
./User/Src/gpio_set.o \
./User/Src/key.o \
./User/Src/lcd.o \
./User/Src/led.o \
./User/Src/main.o \
./User/Src/mpu.o \
./User/Src/myiic.o \
./User/Src/rtc.o \
./User/Src/sai.o \
./User/Src/sys.o \
./User/Src/timer.o \
./User/Src/tpad.o \
./User/Src/ui_Driver.o \
./User/Src/usart.o \
./User/Src/wdg.o \
./User/Src/wm8978.o 

C_DEPS += \
./User/Src/24cxx.d \
./User/Src/Audio_driver.d \
./User/Src/KEY_Control.d \
./User/Src/Si5351AI2C.d \
./User/Src/Softdds.d \
./User/Src/adc.d \
./User/Src/cat_kenwood.d \
./User/Src/dac.d \
./User/Src/dma.d \
./User/Src/dsp_filter.d \
./User/Src/dwt.d \
./User/Src/encode.d \
./User/Src/exti.d \
./User/Src/filter.d \
./User/Src/gpio_set.d \
./User/Src/key.d \
./User/Src/lcd.d \
./User/Src/led.d \
./User/Src/main.d \
./User/Src/mpu.d \
./User/Src/myiic.d \
./User/Src/rtc.d \
./User/Src/sai.d \
./User/Src/sys.d \
./User/Src/timer.d \
./User/Src/tpad.d \
./User/Src/ui_Driver.d \
./User/Src/usart.d \
./User/Src/wdg.d \
./User/Src/wm8978.d 


# Each subdirectory must supply rules for building sources it contributes
User/Src/%.o User/Src/%.su User/Src/%.cyclo: ../User/Src/%.c User/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DSTM32H743xx -c -I"/home/daniel/Documents/workspace/FX-4CR/User/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Core/Inc" -I"/home/daniel/Documents/workspace/FX-4CR/Dsp/Inc" -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User-2f-Src

clean-User-2f-Src:
	-$(RM) ./User/Src/24cxx.cyclo ./User/Src/24cxx.d ./User/Src/24cxx.o ./User/Src/24cxx.su ./User/Src/Audio_driver.cyclo ./User/Src/Audio_driver.d ./User/Src/Audio_driver.o ./User/Src/Audio_driver.su ./User/Src/KEY_Control.cyclo ./User/Src/KEY_Control.d ./User/Src/KEY_Control.o ./User/Src/KEY_Control.su ./User/Src/Si5351AI2C.cyclo ./User/Src/Si5351AI2C.d ./User/Src/Si5351AI2C.o ./User/Src/Si5351AI2C.su ./User/Src/Softdds.cyclo ./User/Src/Softdds.d ./User/Src/Softdds.o ./User/Src/Softdds.su ./User/Src/adc.cyclo ./User/Src/adc.d ./User/Src/adc.o ./User/Src/adc.su ./User/Src/cat_kenwood.cyclo ./User/Src/cat_kenwood.d ./User/Src/cat_kenwood.o ./User/Src/cat_kenwood.su ./User/Src/dac.cyclo ./User/Src/dac.d ./User/Src/dac.o ./User/Src/dac.su ./User/Src/dma.cyclo ./User/Src/dma.d ./User/Src/dma.o ./User/Src/dma.su ./User/Src/dsp_filter.cyclo ./User/Src/dsp_filter.d ./User/Src/dsp_filter.o ./User/Src/dsp_filter.su ./User/Src/dwt.cyclo ./User/Src/dwt.d ./User/Src/dwt.o ./User/Src/dwt.su ./User/Src/encode.cyclo ./User/Src/encode.d ./User/Src/encode.o ./User/Src/encode.su ./User/Src/exti.cyclo ./User/Src/exti.d ./User/Src/exti.o ./User/Src/exti.su ./User/Src/filter.cyclo ./User/Src/filter.d ./User/Src/filter.o ./User/Src/filter.su ./User/Src/gpio_set.cyclo ./User/Src/gpio_set.d ./User/Src/gpio_set.o ./User/Src/gpio_set.su ./User/Src/key.cyclo ./User/Src/key.d ./User/Src/key.o ./User/Src/key.su ./User/Src/lcd.cyclo ./User/Src/lcd.d ./User/Src/lcd.o ./User/Src/lcd.su ./User/Src/led.cyclo ./User/Src/led.d ./User/Src/led.o ./User/Src/led.su ./User/Src/main.cyclo ./User/Src/main.d ./User/Src/main.o ./User/Src/main.su ./User/Src/mpu.cyclo ./User/Src/mpu.d ./User/Src/mpu.o ./User/Src/mpu.su ./User/Src/myiic.cyclo ./User/Src/myiic.d ./User/Src/myiic.o ./User/Src/myiic.su ./User/Src/rtc.cyclo ./User/Src/rtc.d ./User/Src/rtc.o ./User/Src/rtc.su ./User/Src/sai.cyclo ./User/Src/sai.d ./User/Src/sai.o ./User/Src/sai.su ./User/Src/sys.cyclo ./User/Src/sys.d ./User/Src/sys.o ./User/Src/sys.su ./User/Src/timer.cyclo ./User/Src/timer.d ./User/Src/timer.o ./User/Src/timer.su ./User/Src/tpad.cyclo ./User/Src/tpad.d ./User/Src/tpad.o ./User/Src/tpad.su ./User/Src/ui_Driver.cyclo ./User/Src/ui_Driver.d ./User/Src/ui_Driver.o ./User/Src/ui_Driver.su ./User/Src/usart.cyclo ./User/Src/usart.d ./User/Src/usart.o ./User/Src/usart.su ./User/Src/wdg.cyclo ./User/Src/wdg.d ./User/Src/wdg.o ./User/Src/wdg.su ./User/Src/wm8978.cyclo ./User/Src/wm8978.d ./User/Src/wm8978.o ./User/Src/wm8978.su

.PHONY: clean-User-2f-Src

