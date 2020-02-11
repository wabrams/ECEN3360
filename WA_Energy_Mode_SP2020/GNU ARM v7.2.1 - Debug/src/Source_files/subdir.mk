################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Source_files/app.c \
../src/Source_files/cmu.c \
../src/Source_files/gpio.c \
../src/Source_files/letimer.c \
../src/Source_files/scheduler.c \
../src/Source_files/sleep_routines.c 

OBJS += \
./src/Source_files/app.o \
./src/Source_files/cmu.o \
./src/Source_files/gpio.o \
./src/Source_files/letimer.o \
./src/Source_files/scheduler.o \
./src/Source_files/sleep_routines.o 

C_DEPS += \
./src/Source_files/app.d \
./src/Source_files/cmu.d \
./src/Source_files/gpio.d \
./src/Source_files/letimer.d \
./src/Source_files/scheduler.d \
./src/Source_files/sleep_routines.d 


# Each subdirectory must supply rules for building sources it contributes
src/Source_files/app.o: ../src/Source_files/app.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Header_files" -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Source_files" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/app.d" -MT"src/Source_files/app.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/cmu.o: ../src/Source_files/cmu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Header_files" -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Source_files" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/cmu.d" -MT"src/Source_files/cmu.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/gpio.o: ../src/Source_files/gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Header_files" -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Source_files" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/gpio.d" -MT"src/Source_files/gpio.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/letimer.o: ../src/Source_files/letimer.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Header_files" -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Source_files" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/letimer.d" -MT"src/Source_files/letimer.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/scheduler.o: ../src/Source_files/scheduler.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Header_files" -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Source_files" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/scheduler.d" -MT"src/Source_files/scheduler.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/Source_files/sleep_routines.o: ../src/Source_files/sleep_routines.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g3 -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DRETARGET_VCOM=1' '-DDEBUG_EFM=1' '-DEFM32PG12B500F1024GL125=1' -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Header_files" -I"C:\Users\Will\git\ECEN3360\WA_Energy_Mode_SP2020\src\Source_files" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/emlib/inc" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/CMSIS/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/SLSTK3402A_EFM32PG12/config" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/bsp" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//platform/Device/SiliconLabs/EFM32PG12B/Include" -I"C:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.6//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"src/Source_files/sleep_routines.d" -MT"src/Source_files/sleep_routines.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


