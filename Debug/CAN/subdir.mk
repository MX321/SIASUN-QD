################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CAN/can.c 

OBJS += \
./CAN/can.o 

C_DEPS += \
./CAN/can.d 


# Each subdirectory must supply rules for building sources it contributes
CAN/%.o: ../CAN/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-poky-linux-gnueabi-gcc  -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/poky/1.7/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi -I"/home/siasunhebo/workspace/IMUtest/CAN" -I"/home/siasunhebo/workspace/IMUtest/MPI204A" -I"/home/siasunhebo/workspace/IMUtest" -include"/home/siasunhebo/workspace/IMUtest/CAN/can.h" -include"/home/siasunhebo/workspace/IMUtest/MPI204A/MPI204A.h" -include"/home/siasunhebo/workspace/IMUtest/main.h" -O1 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


