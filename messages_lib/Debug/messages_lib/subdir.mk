################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../messages_lib/messages_lib.c 

OBJS += \
./messages_lib/messages_lib.o 

C_DEPS += \
./messages_lib/messages_lib.d 


# Each subdirectory must supply rules for building sources it contributes
messages_lib/%.o: ../messages_lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


