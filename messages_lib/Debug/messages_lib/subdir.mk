################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../messages_lib/appearedPokemonBroker.c \
../messages_lib/appearedPokemonTeam.c \
../messages_lib/catchPokemonBroker.c \
../messages_lib/catchPokemonGamecard.c \
../messages_lib/caughtPokemon.c \
../messages_lib/colaMutex.c \
../messages_lib/getPokemonBroker.c \
../messages_lib/getPokemonGamecard.c \
../messages_lib/listaMutex.c \
../messages_lib/localizedPokemon.c \
../messages_lib/log.c \
../messages_lib/messages_lib.c \
../messages_lib/newPokemonBroker.c \
../messages_lib/newPokemonGamecard.c \
../messages_lib/paquete.c \
../messages_lib/suscripcion.c \
../messages_lib/suscripcionTiempo.c 

OBJS += \
./messages_lib/appearedPokemonBroker.o \
./messages_lib/appearedPokemonTeam.o \
./messages_lib/catchPokemonBroker.o \
./messages_lib/catchPokemonGamecard.o \
./messages_lib/caughtPokemon.o \
./messages_lib/colaMutex.o \
./messages_lib/getPokemonBroker.o \
./messages_lib/getPokemonGamecard.o \
./messages_lib/listaMutex.o \
./messages_lib/localizedPokemon.o \
./messages_lib/log.o \
./messages_lib/messages_lib.o \
./messages_lib/newPokemonBroker.o \
./messages_lib/newPokemonGamecard.o \
./messages_lib/paquete.o \
./messages_lib/suscripcion.o \
./messages_lib/suscripcionTiempo.o 

C_DEPS += \
./messages_lib/appearedPokemonBroker.d \
./messages_lib/appearedPokemonTeam.d \
./messages_lib/catchPokemonBroker.d \
./messages_lib/catchPokemonGamecard.d \
./messages_lib/caughtPokemon.d \
./messages_lib/colaMutex.d \
./messages_lib/getPokemonBroker.d \
./messages_lib/getPokemonGamecard.d \
./messages_lib/listaMutex.d \
./messages_lib/localizedPokemon.d \
./messages_lib/log.d \
./messages_lib/messages_lib.d \
./messages_lib/newPokemonBroker.d \
./messages_lib/newPokemonGamecard.d \
./messages_lib/paquete.d \
./messages_lib/suscripcion.d \
./messages_lib/suscripcionTiempo.d 


# Each subdirectory must supply rules for building sources it contributes
messages_lib/%.o: ../messages_lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


