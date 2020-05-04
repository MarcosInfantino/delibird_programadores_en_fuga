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
#include "messages_lib.h"

//\n

int main(int argc, char* argv[]) {

	t_config * config = config_create("gameBoy1.config");
	void* paqueteAEnviar;
	void* stream;
	paquete* paquete;
	uint32_t sizeStream, procesoDestinatario;
	uint32_t colaMensaje=obtenerColaMensaje(argv[2]);

	if(strcmp(argv[1], "SUSCRIPTOR")==0){
		procesoDestinatario = BROKER;
		mensajeSuscripcionTiempo* mensajeEnviar = malloc(sizeof(uint32_t)*2);
		mensajeEnviar->cola = colaMensaje;
		mensajeEnviar->tiempo=atoi(argv[3]);
		stream = serializarSuscripcionTiempo(mensajeEnviar);
		sizeStream=sizeof(uint32_t)*2;
		paquete = llenarPaquete(sizeStream,stream, SUSCRIPCION_TIEMPO);
		paqueteAEnviar = serializarPaquete(paquete);
	}else{
		procesoDestinatario = obtenerNombreProceso(argv[1]);
		switch(colaMensaje){
				case APPEARED_POKEMON:
					if(procesoDestinatario == BROKER){
						mensajeAppearedBroker* mensajeEnviar = malloc(strlen(argv[3])+1+sizeof(uint32_t)*6);
						mensajeEnviar->sizePokemon = strlen(argv[3]) + 1;
						mensajeEnviar->pokemon = argv[3];
						mensajeEnviar->posX = atoi(argv[4]);
						mensajeEnviar->posY = atoi(argv[5]);
						mensajeEnviar->id=atoi(argv[6]);
						stream = serializarAppearedBroker(mensajeEnviar);

					}else if (procesoDestinatario==TEAM){
						mensajeAppearedTeam* mensajeEnviar = malloc(strlen(argv[3]) + 1 + sizeof(uint32_t)*5);
						mensajeEnviar->sizePokemon=strlen(argv[3])+1;
						mensajeEnviar->pokemon=argv[3];
						mensajeEnviar->posX=atoi(argv[4]);
						mensajeEnviar->posY=atoi(argv[5]);
						stream = serializarAppearedTeam(mensajeEnviar);
					}
					break;

				case NEW_POKEMON:
					if(procesoDestinatario==BROKER){
						mensajeNewBroker* mensajeEnviar = malloc(strlen(argv[3])+1+sizeof(uint32_t)*6);
						mensajeEnviar->sizePokemon=strlen(argv[3])+1;
						mensajeEnviar->pokemon = argv[3];
						mensajeEnviar->posX=atoi(argv[4]);
						mensajeEnviar->posY=atoi(argv[5]);
						mensajeEnviar->cantidad=atoi(argv[6]);
						stream = serializarNewBroker(mensajeEnviar);

					}else if (procesoDestinatario==GAMECARD){
						mensajeNewGamecard* mensajeEnviar = malloc(strlen(argv[3])+1+sizeof(uint32_t)*7);
						mensajeEnviar->sizePokemon=strlen(argv[3])+1;
						mensajeEnviar->pokemon=argv[3];
						mensajeEnviar->posX=atoi(argv[4]);
						mensajeEnviar->posY = atoi(argv[5]);
						mensajeEnviar->cantidad = atoi(argv[6]);
						mensajeEnviar->id=atoi(argv[7]);
						stream = serializarNewGamecard(mensajeEnviar);
					}
					break;

				case CATCH_POKEMON:
					if(procesoDestinatario==BROKER){
						mensajeCatchBroker* mensajeEnviar = malloc(strlen(argv[3])+1+sizeof(uint32_t)*5);
						mensajeEnviar->sizePokemon=strlen(argv[3])+1;
						mensajeEnviar->pokemon=argv[3];
						mensajeEnviar->posX=atoi(argv[4]);
						mensajeEnviar->posY=atoi(argv[5]);
						stream = serializarCatchBroker(mensajeEnviar);

					}else if(procesoDestinatario==GAMECARD){
						mensajeCatchGamecard* mensajeEnviarCatch = malloc(strlen(argv[3])+1+sizeof(uint32_t)*6);
						mensajeEnviarCatch->sizePokemon=strlen(argv[3])+1;
						mensajeEnviarCatch->pokemon=argv[3];
						mensajeEnviarCatch->posX=atoi(argv[4]);
						mensajeEnviarCatch->posY=atoi(argv[5]);
						mensajeEnviarCatch->id=atoi(argv[6]);
						stream = serializarCatchGamecard(mensajeEnviarCatch);
					}
					break;

				case CAUGHT_POKEMON: ;
					mensajeCaught* mensajeEnviarCaught = malloc(sizeof(uint32_t)*4);
					mensajeEnviarCaught->id=atoi(argv[3]);
					mensajeEnviarCaught->resultadoCaught=atoi(argv[4]);
					stream = serializarCaught(mensajeEnviarCaught);
					break;

				case GET_POKEMON: ;
					mensajeGet* mensajeEnviarGet = malloc(strlen(argv[3])+1+sizeof(uint32_t)*3);
					mensajeEnviarGet->sizePokemon=strlen(argv[3])+1;
					mensajeEnviarGet->pokemon = argv[3];
					stream = serializarGet(mensajeEnviarGet);
					break;

				default:
					printf("caso ingresado invalido");
					break;
	}
	sizeStream = sizeArgumentos (colaMensaje, argv, procesoDestinatario);
	paquete=llenarPaquete(sizeStream, stream, colaMensaje);
	paqueteAEnviar = serializarPaquete(paquete);
}
	char* ipProcesoDestinatario = obtenerIpProceso (procesoDestinatario, config);
	uint32_t puertoProcesoDestinatario = obtenerPuertoProceso (procesoDestinatario, config);
	uint32_t socket_cliente = socketCliente (ipProcesoDestinatario, puertoProcesoDestinatario);
	send(socket_cliente, paqueteAEnviar, sizeof(uint32_t)*5+sizeStream,0);
	return EXIT_SUCCESS;
}

paquete* llenarPaquete(uint32_t sizeStream, void* stream, uint32_t tipoMensaje){
	paquete* paqueteASerializar = malloc(sizeof(uint32_t)*5+sizeStream);
	paqueteASerializar->modulo=GAMEBOY;
	paqueteASerializar->tipoMensaje=tipoMensaje;
	paqueteASerializar->id=-1;
	paqueteASerializar->idCorrelativo=-1;
	paqueteASerializar->sizeStream=sizeStream;
	paqueteASerializar->stream=stream;

	return paqueteASerializar;
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

uint32_t obtenerColaMensaje (char* tipo){
	uint32_t cola;
	if(strcmp(tipo, "APPEARED_POKEMON") ==0){
		cola = APPEARED_POKEMON;
	}else if(strcmp(tipo, "NEW_POKEMON")==0){
		cola=NEW_POKEMON;
	}else if(strcmp(tipo,"CAUGHT_POKEMON")==0){
		cola=CAUGHT_POKEMON;
	}else if(strcmp (tipo, "CATCH_POKEMON")==0){
		cola=CATCH_POKEMON;
	}else if(strcmp (tipo, "GET_POKEMON")==0){
		cola=GET_POKEMON;
	}
	return cola;
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

