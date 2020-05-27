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
#include "log.h"

//\n

t_log* gameboyLogger;

int main(int argc, char* argv[]) {

	gameboyLogger = iniciar_logger();

	printf("Estoy andando\n");
	log_info(gameboyLogger, "Arranco\n");
	t_config * config = config_create("gameBoy1.config");
	void* stream;
	paquete* paquete;
	uint32_t sizeStream, procesoDestinatario;
	uint32_t colaMensaje = obtenerColaMensaje(argv[2]);

	if(strcmp(argv[1], "SUSCRIPTOR") == 0){
		procesoDestinatario 					= BROKER;
		mensajeSuscripcionTiempo* mensajeEnviar = malloc(sizeof(mensajeSuscripcionTiempo));
		mensajeEnviar->cola 					= colaMensaje;
		mensajeEnviar->tiempo					= atoi(argv[3]);
		stream 									= serializarSuscripcionTiempo(mensajeEnviar);
		sizeStream								= sizeof(uint32_t)*2;
		paquete 								= llenarPaquete(GAMEBOY, SUSCRIPCION_TIEMPO, sizeStream, stream);
	}else{
		procesoDestinatario = obtenerNombreProceso(argv[1]);
		stream 				= generarStreamArgumentos(colaMensaje, argv);
		sizeStream 			= sizeArgumentos (colaMensaje, argv[3], procesoDestinatario);
		paquete				= llenarPaquete(GAMEBOY, colaMensaje, sizeStream, stream);
		}

	char* ipProcesoDestinatario        = obtenerIpProceso (procesoDestinatario, config);
	uint32_t puertoProcesoDestinatario = obtenerPuertoProceso (procesoDestinatario, config);
	paqueteYSocket* paqueteySocket 	   = malloc(sizeof(paqueteYSocket));
	paqueteySocket->paqueteAEnviar 	   = serializarPaquete(paquete);
	paqueteySocket->socketCliente      = crearSocketCliente (ipProcesoDestinatario, puertoProcesoDestinatario);
	iniciarHiloEnvio(paqueteySocket);

	return EXIT_SUCCESS;
}

void* generarStreamArgumentos (uint32_t colaMensaje, char* argv[]){
	void* streamArgumentos;
	uint32_t procesoDestinatario = obtenerNombreProceso(argv[1]);
	switch(colaMensaje){
		case APPEARED_POKEMON:
			if(procesoDestinatario == BROKER){
				mensajeAppearedBroker* mensajeEnviar = llenarMensajeAppearedBroker(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
				streamArgumentos = serializarAppearedBroker(mensajeEnviar);

			}else if (procesoDestinatario == TEAM){
				mensajeAppearedTeam* mensajeEnviar = llenarMensajeAppearedTeam(argv[3], atoi(argv[4]), atoi(argv[5]));
				streamArgumentos = serializarAppearedTeam(mensajeEnviar);
			}
			break;

		case NEW_POKEMON:
			if(procesoDestinatario == BROKER){
				mensajeNewBroker* mensajeEnviar = llenarMensajeNewBroker(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
				streamArgumentos = serializarNewBroker(mensajeEnviar);

			}else if (procesoDestinatario == GAMECARD){
				mensajeNewGamecard* mensajeEnviar= llenarMensajeNewGameCard(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[6]));
				streamArgumentos = serializarNewGamecard(mensajeEnviar);
			}
			break;

		case CATCH_POKEMON:
			if(procesoDestinatario == BROKER){
				mensajeCatchBroker* mensajeEnviar = llenarMensajeCatchBroker(argv[3], atoi(argv[4]), atoi(argv[5]));
				streamArgumentos = serializarCatchBroker(mensajeEnviar);

			}else if(procesoDestinatario == GAMECARD){
				mensajeCatchGamecard* mensajeEnviarCatch = llenarMensajeCatchGamecard(argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
				streamArgumentos = serializarCatchGamecard(mensajeEnviarCatch);
			}
			break;

		case CAUGHT_POKEMON: ;
			mensajeCaught* mensajeEnviarCaught = llenarMensajeCaught(atoi(argv[3]), atoi(argv[4]));
			streamArgumentos = serializarCaught(mensajeEnviarCaught);
			break;

		case GET_POKEMON:
			if(procesoDestinatario == BROKER){
				mensajeGetBroker* mensajeEnviarGetBroker = llenarMensajeGetBroker(argv[3]);
				streamArgumentos 					= serializarGetBroker(mensajeEnviarGetBroker);

			}else if(procesoDestinatario == GAMECARD){
				mensajeGetGamecard* mensajeEnviarGetGamecard = llenarMensajeGetGamecard(argv[3], atoi(argv[4]));
				streamArgumentos = serializarGetGamecard (mensajeEnviarGetGamecard);
			}
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
	return NULL;
}

void loggearMensajeRecibido (paquete* paqueteRespuesta){
	switch(paqueteRespuesta->tipoMensaje){
		case APPEARED_POKEMON:;
			mensajeAppearedBroker* msgAppeared=deserializarAppearedBroker(paqueteRespuesta->stream);
			log_info(gameboyLogger, "Recibi mensaje appeared pokemon: \n");
			log_info(gameboyLogger, "El pokemon es: %s\n", msgAppeared->pokemon);
			log_info(gameboyLogger, "La posX es: %i\n", msgAppeared->posX);
			log_info(gameboyLogger, "La posY es: %i\n", msgAppeared->posY);
			log_info(gameboyLogger, "El id correlativo es: %i\n", msgAppeared->idCorrelativo);
			break;

		case NEW_POKEMON: ;
			mensajeNewBroker* msgNew = deserializarNewBroker(paqueteRespuesta->stream);
			log_info(gameboyLogger, "Recibi mensaje new pokemon: \n");
			log_info(gameboyLogger, "El pokemon es: %s\n", msgNew->pokemon);
			log_info(gameboyLogger, "La posX es: %i\n", msgNew->posX);
			log_info(gameboyLogger, "La posY es: %i\n", msgNew->posY);
			log_info(gameboyLogger, "La cantidad es: %i\n", msgNew->cantidad);
			break;

		case CATCH_POKEMON: ;
			mensajeCatchBroker* msgCatch = deserializarCatchBroker (paqueteRespuesta->stream);
			log_info(gameboyLogger, "Recibi mensaje catch pokemon: \n");
			log_info(gameboyLogger, "El pokemon es: %s\n", msgCatch->pokemon);
			log_info(gameboyLogger, "La posX es: %i\n", msgCatch->posX);
			log_info(gameboyLogger, "La posY es: %s\n", msgCatch->posY);
			break;

		case CAUGHT_POKEMON: ;
			mensajeCaught* msgCaught = deserializarCaught (paqueteRespuesta->stream);
			log_info(gameboyLogger, "Recibi mensaje caught pokemon: \n");
			log_info(gameboyLogger, "El id correlativo es: %i\n", msgCaught->idCorrelativo);
			log_info(gameboyLogger, "El resultado es: %i\n", msgCaught->resultadoCaught);
			break;

		case GET_POKEMON: ;
			mensajeGetBroker* msgGet = deserializarGetBroker (paqueteRespuesta->stream);
			log_info(gameboyLogger, "Recibi mensaje get pokemon: \n");
			log_info(gameboyLogger, "El pokemon es: %s\n", msgGet->pokemon);
			break;

	//	case LOCALIZED_POKEMON: ;
	//		mensajeLocalized* msgLocalized = deserializarLocalized(paqueteRespuesta->stream);
	//		log_info(gameboyLogger, "Recibi mensaje localized pokemon\n");
	//		break;
		}
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
	while(1){
		paquete* paqueteRespuesta=recibirPaquete(paqueteConSocket->socketCliente);
		loggearMensajeRecibido (paqueteRespuesta);
	return NULL;
	}
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
