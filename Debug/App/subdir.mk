################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../App/app.class.cpp \
../App/app.cpp \
../App/circular_buffer.class.cpp \
../App/config.class.cpp \
../App/gps_parser.cpp \
../App/gps_record.cpp \
../App/gps_record_synchronizer.class.cpp \
../App/logger.cpp \
../App/ui.class.cpp \
../App/ui.info.screen.class.cpp \
../App/ui.menu.screen.class.cpp \
../App/ui.record_gps.screen.class.cpp \
../App/ui.sync_gps.screen.class.cpp \
../App/ui.sync_gps_finish.screen.class.cpp \
../App/wifi.cpp 

OBJS += \
./App/app.class.o \
./App/app.o \
./App/circular_buffer.class.o \
./App/config.class.o \
./App/gps_parser.o \
./App/gps_record.o \
./App/gps_record_synchronizer.class.o \
./App/logger.o \
./App/ui.class.o \
./App/ui.info.screen.class.o \
./App/ui.menu.screen.class.o \
./App/ui.record_gps.screen.class.o \
./App/ui.sync_gps.screen.class.o \
./App/ui.sync_gps_finish.screen.class.o \
./App/wifi.o 

CPP_DEPS += \
./App/app.class.d \
./App/app.d \
./App/circular_buffer.class.d \
./App/config.class.d \
./App/gps_parser.d \
./App/gps_record.d \
./App/gps_record_synchronizer.class.d \
./App/logger.d \
./App/ui.class.d \
./App/ui.info.screen.class.d \
./App/ui.menu.screen.class.d \
./App/ui.record_gps.screen.class.d \
./App/ui.sync_gps.screen.class.d \
./App/ui.sync_gps_finish.screen.class.d \
./App/wifi.d 


# Each subdirectory must supply rules for building sources it contributes
App/%.o App/%.su App/%.cyclo: ../App/%.cpp App/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I.././ -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-App

clean-App:
	-$(RM) ./App/app.class.cyclo ./App/app.class.d ./App/app.class.o ./App/app.class.su ./App/app.cyclo ./App/app.d ./App/app.o ./App/app.su ./App/circular_buffer.class.cyclo ./App/circular_buffer.class.d ./App/circular_buffer.class.o ./App/circular_buffer.class.su ./App/config.class.cyclo ./App/config.class.d ./App/config.class.o ./App/config.class.su ./App/gps_parser.cyclo ./App/gps_parser.d ./App/gps_parser.o ./App/gps_parser.su ./App/gps_record.cyclo ./App/gps_record.d ./App/gps_record.o ./App/gps_record.su ./App/gps_record_synchronizer.class.cyclo ./App/gps_record_synchronizer.class.d ./App/gps_record_synchronizer.class.o ./App/gps_record_synchronizer.class.su ./App/logger.cyclo ./App/logger.d ./App/logger.o ./App/logger.su ./App/ui.class.cyclo ./App/ui.class.d ./App/ui.class.o ./App/ui.class.su ./App/ui.info.screen.class.cyclo ./App/ui.info.screen.class.d ./App/ui.info.screen.class.o ./App/ui.info.screen.class.su ./App/ui.menu.screen.class.cyclo ./App/ui.menu.screen.class.d ./App/ui.menu.screen.class.o ./App/ui.menu.screen.class.su ./App/ui.record_gps.screen.class.cyclo ./App/ui.record_gps.screen.class.d ./App/ui.record_gps.screen.class.o ./App/ui.record_gps.screen.class.su ./App/ui.sync_gps.screen.class.cyclo ./App/ui.sync_gps.screen.class.d ./App/ui.sync_gps.screen.class.o ./App/ui.sync_gps.screen.class.su ./App/ui.sync_gps_finish.screen.class.cyclo ./App/ui.sync_gps_finish.screen.class.d ./App/ui.sync_gps_finish.screen.class.o ./App/ui.sync_gps_finish.screen.class.su ./App/wifi.cyclo ./App/wifi.d ./App/wifi.o ./App/wifi.su

.PHONY: clean-App

