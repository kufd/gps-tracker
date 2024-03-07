################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../App/app.cpp \
../App/gps_parser.cpp \
../App/logger.cpp \
../App/storage.cpp \
../App/ui.cpp \
../App/wifi.cpp 

OBJS += \
./App/app.o \
./App/gps_parser.o \
./App/logger.o \
./App/storage.o \
./App/ui.o \
./App/wifi.o 

CPP_DEPS += \
./App/app.d \
./App/gps_parser.d \
./App/logger.d \
./App/storage.d \
./App/ui.d \
./App/wifi.d 


# Each subdirectory must supply rules for building sources it contributes
App/%.o App/%.su App/%.cyclo: ../App/%.cpp App/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I.././ -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App

clean-App:
	-$(RM) ./App/app.cyclo ./App/app.d ./App/app.o ./App/app.su ./App/gps_parser.cyclo ./App/gps_parser.d ./App/gps_parser.o ./App/gps_parser.su ./App/logger.cyclo ./App/logger.d ./App/logger.o ./App/logger.su ./App/storage.cyclo ./App/storage.d ./App/storage.o ./App/storage.su ./App/ui.cyclo ./App/ui.d ./App/ui.o ./App/ui.su ./App/wifi.cyclo ./App/wifi.d ./App/wifi.o ./App/wifi.su

.PHONY: clean-App

