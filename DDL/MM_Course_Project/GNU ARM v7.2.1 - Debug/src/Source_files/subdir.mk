################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Source_files/app.c \
../src/Source_files/ble.c \
../src/Source_files/cmu.c \
../src/Source_files/gpio.c \
../src/Source_files/i2c.c \
../src/Source_files/letimer.c \
../src/Source_files/leuart.c \
../src/Source_files/scheduler.c \
../src/Source_files/si7021.c \
../src/Source_files/sleep_routines.c 

OBJS += \
./src/Source_files/app.o \
./src/Source_files/ble.o \
./src/Source_files/cmu.o \
./src/Source_files/gpio.o \
./src/Source_files/i2c.o \
./src/Source_files/letimer.o \
./src/Source_files/leuart.o \
./src/Source_files/scheduler.o \
./src/Source_files/si7021.o \
./src/Source_files/sleep_routines.o 

C_DEPS += \
./src/Source_files/app.d \
./src/Source_files/ble.d \
./src/Source_files/cmu.d \
./src/Source_files/gpio.d \
./src/Source_files/i2c.d \
./src/Source_files/letimer.d \
./src/Source_files/leuart.d \
./src/Source_files/scheduler.d \
./src/Source_files/si7021.d \
./src/Source_files/sleep_routines.d 


# Each subdirectory must supply rules for building sources it contributes
src/Source_files/app.o: ../src/Source_files/app.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/app.d" -MT"src/Source_files/app.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/ble.o: ../src/Source_files/ble.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/ble.d" -MT"src/Source_files/ble.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/cmu.o: ../src/Source_files/cmu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/cmu.d" -MT"src/Source_files/cmu.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/gpio.o: ../src/Source_files/gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/gpio.d" -MT"src/Source_files/gpio.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/i2c.o: ../src/Source_files/i2c.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/i2c.d" -MT"src/Source_files/i2c.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/letimer.o: ../src/Source_files/letimer.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/letimer.d" -MT"src/Source_files/letimer.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/leuart.o: ../src/Source_files/leuart.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/leuart.d" -MT"src/Source_files/leuart.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/scheduler.o: ../src/Source_files/scheduler.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/scheduler.d" -MT"src/Source_files/scheduler.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/si7021.o: ../src/Source_files/si7021.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/si7021.d" -MT"src/Source_files/si7021.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/sleep_routines.o: ../src/Source_files/sleep_routines.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Header_files" -I"C:\Users\Freck\SimplicityStudio\v4_workspace\MM_Course_Project\src\Source_files" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"D:/Simplicity_Studio/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/sleep_routines.d" -MT"src/Source_files/sleep_routines.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


