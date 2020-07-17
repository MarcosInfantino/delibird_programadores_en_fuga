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
#include <commons/config.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/types.h>
#include <pthread.h>
#include "gameBoy.h"

//\n

int main(int argc, char* argv[]) {
//	char* logGameboy = "gameboy.log";
//	char* nombreProg = "GAMEBOY";
//	gameboyLogger = iniciar_logger(nombreLog, nombreProg);
//	t_config * config = config_create("gameBoy1.config");

	t_config * config = config_create("gameboy.config");
	idProcesoGameboy=config_get_int_value(config, "ID_PROCESO");
	char* pathLogger = config_get_string_value(config, "LOG_FILE");
	char* nombreProg = "GAMEBOY";
	gameboyLogger = iniciar_logger(pathLogger, nombreProg);
	log_info(gameboyLogger, "Arranco\n");

	void* stream;
	paquete* paquete;
	uint32_t sizeStream, procesoDestinatario;
	uint32_t colaMensaje = obtenerColaMensaje(argv[2]);


	if(strcmp(argv[1], "SUSCRIPTOR") == 0){
		procesoDestinatario 					= BROKER;
		mensajeSuscripcionTiempo* mensajeEnviar = llenarSuscripcionTiempo(colaMensaje, atoi(argv[3]), idProcesoGameboy);
		stream 									= serializarSuscripcionTiempo(mensajeEnviar);
		destruirSuscripcionTiempo(mensajeEnviar);
		//sizeStream								= sizeof(uint32_t)*2;
		paquete 								= llenarPaquete(GAMEBOY, SUSCRIPCION_TIEMPO, sizeArgumentos(SUSCRIPCION_TIEMPO, "", 0), stream);
	}else{
		procesoDestinatario = obtenerNombreProceso(argv[1]);

		stream 				= generarStreamArgumentos(colaMensaje, argv);
		sizeStream 			= sizeArgumentos (colaMensaje, argv[3],1);
		paquete				= llenarPaquete(GAMEBOY, colaMensaje, sizeStream, stream);
		asignarIds(argv,procesoDestinatario,paquete);

		}

	char* ipProcesoDestinatario        = obtenerIpProceso (procesoDestinatario, config);
	ipGlobal=malloc(strlen(ipProcesoDestinatario)+1);
	strcpy(ipGlobal, ipProcesoDestinatario);
	uint32_t puertoProcesoDestinatario = obtenerPuertoProceso (procesoDestinatario, config);
	puertoGlobal=puertoProcesoDestinatario;
	paqueteYSocket* paqueteySocket 	   = malloc(sizeof(paqueteYSocket));
	paqueteySocket->paqueteAEnviar 	   = serializarPaquete(paquete);
	paqueteySocket->socketCliente      = crearSocketCliente (ipProcesoDestinatario, puertoProcesoDestinatario);
	iniciarHiloEnvio(paqueteySocket);
	terminar_programa(gameboyLogger,config);
	return EXIT_SUCCESS;
}

void asignarIds(char * argv[], uint32_t proceso, paquete* paq){

	switch(paq->tipoMensaje){
		case APPEARED_POKEMON:
			if(proceso==BROKER){
				insertarIdCorrelativoPaquete(paq,atoi(argv[6]));
			}
			break;
		case CAUGHT_POKEMON:
			if(proceso==BROKER){
				insertarIdCorrelativoPaquete(paq,atoi(argv[3]));
			}
			break;
		case CATCH_POKEMON:
			if(proceso==GAMECARD){
				insertarIdPaquete(paq, atoi(argv[6]));
			}
			break;
		case NEW_POKEMON:
			if(proceso==GAMECARD){
				insertarIdPaquete(paq, atoi(argv[7]));
			}
			break;
		case GET_POKEMON:
			if(proceso==GAMECARD){
				insertarIdPaquete(paq, atoi(argv[4]));
			}
			break;
		default:break;

	}

}
void* generarStreamArgumentos (uint32_t colaMensaje, char* argv[]){
	void* streamArgumentos;

	switch(colaMensaje){
		case APPEARED_POKEMON:;

				mensajeAppeared* mensajeEnviarAppeared = llenarAppeared(argv[3], atoi(argv[4]), atoi(argv[5]));
				streamArgumentos = serializarAppeared(mensajeEnviarAppeared);
				destruirAppeared(mensajeEnviarAppeared);

			break;

		case NEW_POKEMON: ;

				mensajeNew* mensajeEnviarNew = llenarNew(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
				streamArgumentos = serializarNew(mensajeEnviarNew);
				destruirNew(mensajeEnviarNew);


			break;

		case CATCH_POKEMON: ;
				mensajeCatch* mensajeEnviarCatch = llenarCatch(argv[3], atoi(argv[4]), atoi(argv[5]));
				streamArgumentos = serializarCatch(mensajeEnviarCatch);
				destruirCatch(mensajeEnviarCatch);


			break;

		case CAUGHT_POKEMON: ;
			uint32_t respuestaCaught;
			if(strcmp("OK", argv[4])==0){
				respuestaCaught=OK;

			}else{
				respuestaCaught=FAIL;

			}
			mensajeCaught* mensajeEnviarCaught = llenarCaught(respuestaCaught);
			streamArgumentos = serializarCaught(mensajeEnviarCaught);
			destruirCaught(mensajeEnviarCaught);
			break;

		case GET_POKEMON:;

				mensajeGet* mensajeEnviarGet = llenarGet(argv[3]);
				streamArgumentos 					= serializarGet(mensajeEnviarGet);
				destruirGet(mensajeEnviarGet);


			break;

		default:
			printf("caso ingresado invalido");
			break;
	}
	return streamArgumentos;
}

uint32_t obtenerNombreProceso (char* proceso){
	uint32_t nombreProceso;
	if(strcmp(proceso, "BROKER") == 0){
		nombreProceso = BROKER;
	}else if(strcmp(proceso, "TEAM") == 0){
		nombreProceso = TEAM;
	}else if(strcmp(proceso, "GAMECARD") == 0){
		nombreProceso = GAMECARD;
	}
	return nombreProceso;
}

uint32_t obtenerColaMensaje (char* tipo){
	uint32_t cola;
	if(strcmp(tipo, "APPEARED_POKEMON") == 0){
		cola = APPEARED_POKEMON;
	}else if(strcmp(tipo, "NEW_POKEMON") == 0){
		cola = NEW_POKEMON;
	}else if(strcmp(tipo,"CAUGHT_POKEMON") == 0){
		cola = CAUGHT_POKEMON;
	}else if(strcmp (tipo, "CATCH_POKEMON") == 0){
		cola = CATCH_POKEMON;
	}else if(strcmp (tipo, "GET_POKEMON")== 0){
		cola = GET_POKEMON;
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

void* enviarMensaje(void* paqueteySocket){
	paqueteYSocket* paqueteConSocket = (paqueteYSocket*) paqueteySocket;

	send(paqueteConSocket->socketCliente, paqueteConSocket->paqueteAEnviar, sizePaquete(paqueteConSocket->paqueteAEnviar), 0);
	log_info(gameboyLogger, "Estoy esperando respuesta al mensaje enviado\n");
	uint32_t respuesta=0;
	recv(paqueteConSocket->socketCliente, &respuesta,sizeof(uint32_t),0);
	if(respuesta>0){
		log_info(gameboyLogger, "Mensaje fue recibido correctamente, respuesta: %i\n", respuesta);
	}else{
		log_info(gameboyLogger, "Mensaje recibido incorrectamente, respuesta: %i\n", respuesta);
	}
	close(paqueteConSocket->socketCliente);
	free(paqueteConSocket->paqueteAEnviar);
	free(paqueteConSocket);
	return NULL;
}

void* enviarMensajeSuscripcion(void* paqueteySocket){
	paqueteYSocket* paqueteConSocket = (paqueteYSocket*) paqueteySocket;
	send(paqueteConSocket->socketCliente, paqueteConSocket->paqueteAEnviar, sizePaquete(paqueteConSocket->paqueteAEnviar), 0);
	log_info(gameboyLogger, "Espero respuesta a solicitud de suscripción\n");
	uint32_t respuesta=0;
	recv(paqueteConSocket->socketCliente, &respuesta,sizeof(uint32_t),0);
	if(respuesta>0){
		log_info(gameboyLogger, "Recibido correctamente: %i\n", respuesta);
	}else{
		log_info(gameboyLogger, "Recibido incorrectamente: %i\n", respuesta);
	}
	paquete* paqueteRespuesta=recibirPaquete(paqueteConSocket->socketCliente);
	while(paqueteRespuesta->tipoMensaje!=SUSCRIPCION_FINALIZADA){
		enviarACK(puertoGlobal,ipGlobal, GAMEBOY, paqueteRespuesta->id, idProcesoGameboy);
		loggearMensaje(paqueteRespuesta, gameboyLogger);
		destruirPaquete(paqueteRespuesta);
		paqueteRespuesta=recibirPaquete(paqueteConSocket->socketCliente);
	}
	return NULL;
}

void iniciarHiloEnvio(paqueteYSocket* paqueteySocket){
	pthread_t hilo;
	uint32_t nro = 0;
	paquete* paq = (paquete*)(paqueteySocket->paqueteAEnviar);
	if(paq->tipoMensaje == SUSCRIPCION_TIEMPO){
		nro = pthread_create(&hilo, NULL, enviarMensajeSuscripcion, (void*)paqueteySocket);
	}else{
		nro = pthread_create(&hilo, NULL, enviarMensaje, (void*)paqueteySocket);
	}
	if(nro!=0){
		printf("Hubo un problema en la creación del hilo para conectarse\n");
	}
	pthread_join(hilo,NULL);
}
