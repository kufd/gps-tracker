################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Application/app.cpp \
../Application/gps_parser.cpp \
../Application/ui.cpp 

OBJS += \
./Application/app.o \
./Application/gps_parser.o \
./Application/ui.o 

CPP_DEPS += \
./Application/app.d \
./Application/gps_parser.d \
./Application/ui.d 


# Each subdirectory must supply rules for building sources it contributes
Application/%.o Application/%.su Application/%.cyclo: ../Application/%.cpp Application/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I.././ -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application

clean-Application:
	-$(RM) ./Application/app.cyclo ./Application/app.d ./Application/app.o ./Application/app.su ./Application/gps_parser.cyclo ./Application/gps_parser.d ./Application/gps_parser.o ./Application/gps_parser.su ./Application/ui.cyclo ./Application/ui.d ./Application/ui.o ./Application/ui.su

.PHONY: clean-Application

