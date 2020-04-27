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

int main(int argc, char* argv[]) {
//
//	argv[1] = "BROKER";
//	argv[2]="APPEARED_POKEMON";

	t_config * config = config_create("gameBoy1.config");

	uint32_t procesoDestinatario = obtenerNombreProceso(argv[1]);
	uint32_t tipoMensaje = obtenerTipoMensaje(argv[2]);
	char* ipProcesoDestinatario = obtenerIpProceso (procesoDestinatario, config);
	uint32_t puertoProcesoDestinatario = obtenerPuertoProceso (procesoDestinatario, config);
	uint32_t socket_Cliente = socketCliente (ipProcesoDestinatario, puertoProcesoDestinatario);
	uint32_t bytesStream = sizeStream (tipoMensaje, argv, procesoDestinatario);
	//enviarMensaje(socket_Cliente, bytesStream);

	printf("El proceso es: %i\n", procesoDestinatario);
	printf("El tipo de mensaje es: %i\n", tipoMensaje);
	printf("El IP es: %s \n", ipProcesoDestinatario);
	printf("El puerto es: %i\n", puertoProcesoDestinatario);
	printf("El size es: %i", bytesStream);

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

uint32_t obtenerTipoMensaje (char* tipo){
	uint32_t tipoMensaje;
	if(strcmp(tipo, "APPEARED_POKEMON") ==0){
		tipoMensaje = APPEARED;
	}else if(strcmp(tipo, "NEW_POKEMON")==0){
		tipoMensaje=NEW;
	}else if(strcmp(tipo,"CAUGHT_POKEMON")==0){
		tipoMensaje=CAUGHT;
	}else if(strcmp (tipo, "CATCH_POKEMON")==0){
		tipoMensaje=CATCH;
	}else if(strcmp (tipo, "GET_POKEMON")==0){
		tipoMensaje=GET;
	}
	return tipoMensaje;
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

uint32_t sizeStream (uint32_t tipoMensaje, char* argv[], uint32_t proceso){
	uint32_t size;
	switch(tipoMensaje){
	case NEW:
		if(proceso == BROKER){
			size = sizeof(argv[3]) + sizeof(uint32_t)*3;
		} else if (proceso==GAMECARD){
			size = sizeof(argv[3]) + sizeof(uint32_t)*4;
		}
		break;

	case APPEARED:
		if(proceso==BROKER){
			size = sizeof(argv[3]) + sizeof(uint32_t)*3;
		}else if(proceso==TEAM){
			size=sizeof(argv[3])+sizeof(uint32_t)*2;
		}
		break;

	case CATCH:
		if(proceso==BROKER){
			size = sizeof(argv[3]) + sizeof(uint32_t)*2;
		}else if (proceso==GAMECARD){
			size = sizeof(argv[3]) + sizeof(uint32_t)*3;
		}
		break;

	case CAUGHT:
		size = sizeof(uint32_t)*2;
		break;

	case GET:
		size = sizeof(argv[3]);
		break;

	default:
		printf("Error: el caso ingresado no esta contemplado \n");
		break;
	}
	return size;
}

//void enviarMensaje (uint32_t socketCliente, uint32_t bytesStream){
//	void* mensajeEnviar = malloc(sizeof(uint32_t)*2 + bytesStream);
//	uint32_t offset = 0;
//}































