################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../LcdDriver/DEV_Config.cpp \
../LcdDriver/GUI_Paint.cpp \
../LcdDriver/LCD_2inch.cpp \
../LcdDriver/LCD_2inch4.cpp \
../LcdDriver/font12.cpp \
../LcdDriver/font16.cpp \
../LcdDriver/font20.cpp \
../LcdDriver/font24.cpp \
../LcdDriver/font8.cpp 

OBJS += \
./LcdDriver/DEV_Config.o \
./LcdDriver/GUI_Paint.o \
./LcdDriver/LCD_2inch.o \
./LcdDriver/LCD_2inch4.o \
./LcdDriver/font12.o \
./LcdDriver/font16.o \
./LcdDriver/font20.o \
./LcdDriver/font24.o \
./LcdDriver/font8.o 

CPP_DEPS += \
./LcdDriver/DEV_Config.d \
./LcdDriver/GUI_Paint.d \
./LcdDriver/LCD_2inch.d \
./LcdDriver/LCD_2inch4.d \
./LcdDriver/font12.d \
./LcdDriver/font16.d \
./LcdDriver/font20.d \
./LcdDriver/font24.d \
./LcdDriver/font8.d 


# Each subdirectory must supply rules for building sources it contributes
LcdDriver/%.o LcdDriver/%.su LcdDriver/%.cyclo: ../LcdDriver/%.cpp LcdDriver/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I.././ -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-LcdDriver

clean-LcdDriver:
	-$(RM) ./LcdDriver/DEV_Config.cyclo ./LcdDriver/DEV_Config.d ./LcdDriver/DEV_Config.o ./LcdDriver/DEV_Config.su ./LcdDriver/GUI_Paint.cyclo ./LcdDriver/GUI_Paint.d ./LcdDriver/GUI_Paint.o ./LcdDriver/GUI_Paint.su ./LcdDriver/LCD_2inch.cyclo ./LcdDriver/LCD_2inch.d ./LcdDriver/LCD_2inch.o ./LcdDriver/LCD_2inch.su ./LcdDriver/LCD_2inch4.cyclo ./LcdDriver/LCD_2inch4.d ./LcdDriver/LCD_2inch4.o ./LcdDriver/LCD_2inch4.su ./LcdDriver/font12.cyclo ./LcdDriver/font12.d ./LcdDriver/font12.o ./LcdDriver/font12.su ./LcdDriver/font16.cyclo ./LcdDriver/font16.d ./LcdDriver/font16.o ./LcdDriver/font16.su ./LcdDriver/font20.cyclo ./LcdDriver/font20.d ./LcdDriver/font20.o ./LcdDriver/font20.su ./LcdDriver/font24.cyclo ./LcdDriver/font24.d ./LcdDriver/font24.o ./LcdDriver/font24.su ./LcdDriver/font8.cyclo ./LcdDriver/font8.d ./LcdDriver/font8.o ./LcdDriver/font8.su

.PHONY: clean-LcdDriver

