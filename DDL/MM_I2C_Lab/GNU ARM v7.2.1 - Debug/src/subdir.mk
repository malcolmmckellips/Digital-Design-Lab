################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c 

OBJS += \
./src/main.o 

C_DEPS += \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/main.o: ../src/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_I2C_Lab\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_I2C_Lab\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/main.d" -MT"src/main.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


