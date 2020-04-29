/*
 ============================================================================
 Name        : gameBoy.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "gameBoy.h"
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <commons/config.h>
#include <stdbool.h>
#include <sys/types.h>

//\n
//argv[0] es ./gameboy, argv[1] es el proceso
//./gameboy SUSCRIPTOR [COLA_DE_MENSAJES] [TIEMPO]

uint32_t procesoDestinatario, colaMensaje, tipoDeMensaje, bytesArgumentos, tiempoSuscripcion;
void* mensajeAEnviar;

int main(int argc, char* argv[]) {

	t_config * config = config_create("gameBoy1.config");

	if(strcmp(argv[1], "SUSCRIPTOR")==0){
		colaMensaje = obtenerColaMensaje(argv[2]);
		tiempoSuscripcion = atoi(argv[3]);
		procesoDestinatario = BROKER;
		tipoDeMensaje = MENSAJE_SUSCRIPCION;
		mensajeAEnviar = armarMensajeSuscripcion(colaMensaje, tiempoSuscripcion);
	}else{
		procesoDestinatario = obtenerNombreProceso(argv[1]);
		colaMensaje = obtenerColaMensaje(argv[2]);
		bytesArgumentos = sizeArgumentos (colaMensaje, argv, procesoDestinatario);
		tipoDeMensaje=MENSAJE_NORMAL;
		void* argumentos = llenarArgumentos (colaMensaje, bytesArgumentos, argv, procesoDestinatario);
		mensajeAEnviar = armarMensajeNormal(argumentos, procesoDestinatario, colaMensaje, bytesArgumentos);
	}
	char* ipProcesoDestinatario = obtenerIpProceso (procesoDestinatario, config);
	uint32_t puertoProcesoDestinatario = obtenerPuertoProceso (procesoDestinatario, config);
	uint32_t socket_Cliente = socketCliente (ipProcesoDestinatario, puertoProcesoDestinatario);

	enviarMensaje(mensajeAEnviar, socket_Cliente, bytesArgumentos);

	return EXIT_SUCCESS;
}

uint32_t obtenerNombreProceso (char* proceso){
	uint32_t nombreProceso;
	if(strcmp(proceso, "BROKER")==0){
		nombreProceso = BROKER;
	}else if(strcmp(proceso, "TEAM") == 0){
		nombreProceso = TEAM;
	}else if(strcmp(proceso, "GAMECARD")==0){
		nombreProceso = GAMECARD;
	}
	return nombreProceso;
}

uint32_t obtenerColaMensaje (char* cola){
	uint32_t colaMensaje;
	if(strcmp(cola, "APPEARED_POKEMON") ==0){
		colaMensaje = APPEARED_POKEMON;
	}else if(strcmp(cola, "NEW_POKEMON")==0){
		colaMensaje=NEW_POKEMON;
	}else if(strcmp(cola,"CAUGHT_POKEMON")==0){
		colaMensaje=CAUGHT_POKEMON;
	}else if(strcmp (cola, "CATCH_POKEMON")==0){
		colaMensaje=CATCH_POKEMON;
	}else if(strcmp (cola, "GET_POKEMON")==0){
		colaMensaje=GET_POKEMON;
	}
	return colaMensaje;
}

char* obtenerIpProceso (uint32_t proceso, t_config* config){
	char* ip;
	switch(proceso){
	case BROKER:
		ip = config_get_string_value(config, "IP_BROKER");
		break;

	case TEAM:
		ip = config_get_string_value(config, "IP_TEAM");
		break;

	case GAMECARD:
		ip = config_get_string_value(config, "IP_GAMECARD");
		break;

	default:
		printf("Error: ip no encontrado \n");
		break;
	}
	return ip;
}

uint32_t obtenerPuertoProceso (uint32_t proceso, t_config* config){
	uint32_t puerto;
	switch(proceso){
	case BROKER:
		puerto = config_get_int_value(config, "PUERTO_BROKER");
		break;
	case TEAM:
		puerto = config_get_int_value(config, "PUERTO_TEAM");
		break;
	case GAMECARD:
		puerto = config_get_int_value(config, "PUERTO_GAMECARD");
		break;
	default:
		printf("Error: puerto no encontrado \n");
		break;
	}
	return puerto;
}

uint32_t socketCliente (char* ip, uint32_t puerto){
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family=AF_INET;
	direccionServidor.sin_addr.s_addr=inet_addr(ip);
	direccionServidor.sin_port=htons(puerto);

	uint32_t cliente=socket(AF_INET,SOCK_STREAM,0);
	if(connect(cliente,(void*) &direccionServidor,sizeof(direccionServidor))!=0){
		perror("No se pudo conectar");
		return 1;
	}
	return cliente;
}

uint32_t sizeArgumentos (uint32_t colaMensaje, char* argv[], uint32_t proceso){
	uint32_t size;
	switch(colaMensaje){
	case NEW_POKEMON:
		if(proceso == BROKER){ //size pokemon, pokemon,posx, posy, cantidad
			size = strlen(argv[3]) + 1 + sizeof(uint32_t)*4;
		} else if (proceso==GAMECARD){ //size pokemon, pokemon, posX, posY, cantidad, ID
			size = strlen(argv[3]) + 1 + sizeof(uint32_t)*5;
		}
		break;

	case APPEARED_POKEMON:
		if(proceso==BROKER){ // sizePokemon, pokemon, posX, posY, ID
			size = strlen(argv[3]) +1 + sizeof(uint32_t)*4;
		}else if(proceso==TEAM){ //sizePokemon, pokemon, posX, posY
			size=strlen(argv[3]) + 1 +sizeof(uint32_t)*3;
		}
		break;

	case CATCH_POKEMON:
		if(proceso==BROKER){ //sizePokemon, pokemon, posX, posY
			size = strlen(argv[3]) + 1 + sizeof(uint32_t)*3;
		}else if (proceso==GAMECARD){ //sizePokemon, pokemon, posX, posY, ID
			size = strlen(argv[3]) + 1 + sizeof(uint32_t)*4;
		}
		break;

	case CAUGHT_POKEMON://ID, ok/fail
		size = sizeof(uint32_t)*2;
		break;

	case GET_POKEMON://sizePokemon, pokemon
		size = strlen(argv[3]) + 1 + sizeof(uint32_t);
		break;

	default:
		printf("Error: el caso ingresado no esta contemplado \n");
		break;
	}
	return size;
}

void* llenarArgumentos (uint32_t colaMensaje, uint32_t bytesArgumentos, char* argv[],uint32_t proceso){
	void* argumentos = malloc(bytesArgumentos);
	uint32_t offset = 0;
	switch(colaMensaje){
		case APPEARED_POKEMON:
			if(proceso==BROKER){
				uint32_t sizePokemon = sizeof(uint32_t);
				memcpy(argumentos+offset, &sizePokemon,sizeof(uint32_t));
				offset+=sizeof(uint32_t);
				memcpy(argumentos+offset, argv[3], strlen(argv[3])+1);
				offset+=(strlen(argv[3])+1);
				uint32_t posX = atoi(argv[4]);
				memcpy(argumentos+offset, &posX, sizeof(uint32_t));
				offset+=sizeof(uint32_t);
				uint32_t posY = atoi(argv[5]);
				memcpy(argumentos+offset, &posY, sizeof(uint32_t));
				offset+=sizeof(uint32_t);
				uint32_t cantidad = atoi(argv[6]);
				memcpy(argumentos+offset, &cantidad, sizeof(uint32_t));
				offset+=sizeof(uint32_t);
			}else if(proceso==TEAM){
				uint32_t sizePokemon = sizeof(uint32_t);
				memcpy(argumentos+offset, &sizePokemon,sizeof(uint32_t));
				offset+=sizeof(uint32_t);
				memcpy(argumentos+offset, argv[3], strlen(argv[3])+1);
				offset+=(strlen(argv[3])+1);
				uint32_t posX = atoi(argv[4]);
				memcpy(argumentos+offset, &posX, sizeof(uint32_t));
				offset+=sizeof(uint32_t);
				uint32_t posY = atoi(argv[5]);
				memcpy(argumentos+offset, &posY, sizeof(uint32_t));
				offset+=sizeof(uint32_t);
			}
		}
	return argumentos;
}

void* armarMensajeNormal (void* argumentos, uint32_t proceso,uint32_t colaMensaje, uint32_t bytesArgumentos){

	/*mensajeNormal mensajeAEnviar = malloc(sizeof(uint32_t)*4 + bytesArgumentos); //opcion 1, pero deberia meterlo en un paquete con un codigo, en este caso tipoMensaje
	mensajeAEnviar->proceso = proceso;
	mensajeAEnviar->tipoMensaje=MENSAJE_NORMAL;
	mensajeAEnviar->cola=colaMensaje;
	mensajeAEnviar->bytes=bytesArgumentos;
	mensajeAEnviar->argumentos=argumentos;*/

	void* mensajeAEnviar = malloc(sizeof(uint32_t)*4 + bytesArgumentos);
	uint32_t offset = 0;
	uint32_t tipoMensaje = MENSAJE_NORMAL;

	memcpy(mensajeAEnviar+offset, &proceso,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensajeAEnviar + offset, &tipoMensaje, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensajeAEnviar+offset, &colaMensaje, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensajeAEnviar+offset, &bytesArgumentos, bytesArgumentos);
	offset+=sizeof(uint32_t);

	memcpy(mensajeAEnviar+offset, argumentos, bytesArgumentos);

	return mensajeAEnviar;
}

void* armarMensajeSuscripcion(uint32_t colaSuscripcion, uint32_t tiempoSuscripcion){

	uint32_t tipoMensaje = MENSAJE_SUSCRIPCION;
	uint32_t proceso = GAMEBOY;
	uint32_t colaSuscribir = colaSuscripcion;

	void* mensajeAEnviar = malloc(sizeof(uint32_t)*5);
	uint32_t offset = 0;

	memcpy(mensajeAEnviar+offset, &tipoMensaje, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensajeAEnviar+offset, &proceso, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensajeAEnviar+offset, &colaSuscribir, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensajeAEnviar+offset, &socket, sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	memcpy(mensajeAEnviar+offset, &tiempoSuscripcion, sizeof(uint32_t));

	return mensajeAEnviar;
}

void enviarMensaje(void* mensajeAEnviar, uint32_t socketCliente, uint32_t bytesArgumentos){

	switch(tipoDeMensaje){
	case MENSAJE_NORMAL:
		send(socketCliente, mensajeAEnviar, sizeof(uint32_t)*4+bytesArgumentos, 0);
		break;
	case MENSAJE_SUSCRIPCION:
		send(socketCliente, mensajeAEnviar, sizeof(uint32_t)*5, 0);
		break;
	}
}




