/*
 * log.c
 *
 *  Created on: 27 may. 2020
 *      Author: utnso
 */

//Conexión a cualquier proceso.
//Suscripción a una cola de mensajes.
//Llegada de un nuevo mensaje a una cola de mensajes.


#include "log.h"
#include "gameBoy.h"

t_log* iniciar_logger()
{
	t_log* logger;
	if((logger=log_create("gameboy.log", "GAMEBOY", 1, LOG_LEVEL_INFO))==NULL){
		printf("No pude crear el logger\n");
		exit(1);
	}
	return logger;
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
			destruirAppearedBroker(msgAppeared);
			break;

		case NEW_POKEMON: ;
			mensajeNewBroker* msgNew = deserializarNewBroker(paqueteRespuesta->stream);
			log_info(gameboyLogger, "Recibi mensaje new pokemon: \n");
			log_info(gameboyLogger, "El pokemon es: %s\n", msgNew->pokemon);
			log_info(gameboyLogger, "La posX es: %i\n", msgNew->posX);
			log_info(gameboyLogger, "La posY es: %i\n", msgNew->posY);
			log_info(gameboyLogger, "La cantidad es: %i\n", msgNew->cantidad);
			destruirNewBroker(msgNew);
			break;

		case CATCH_POKEMON: ;
			mensajeCatchBroker* msgCatch = deserializarCatchBroker (paqueteRespuesta->stream);
			log_info(gameboyLogger, "Recibi mensaje catch pokemon: \n");
			log_info(gameboyLogger, "El pokemon es: %s\n", msgCatch->pokemon);
			log_info(gameboyLogger, "La posX es: %i\n", msgCatch->posX);
			log_info(gameboyLogger, "La posY es: %s\n", msgCatch->posY);
			destruirCatchBroker(msgCatch);
			break;

		case CAUGHT_POKEMON: ;
			mensajeCaught* msgCaught = deserializarCaught (paqueteRespuesta->stream);
			log_info(gameboyLogger, "Recibi mensaje caught pokemon: \n");
			log_info(gameboyLogger, "El id correlativo es: %i\n", msgCaught->idCorrelativo);
			log_info(gameboyLogger, "El resultado es: %i\n", msgCaught->resultadoCaught);
			destruirCaught(msgCaught);
			break;

		case GET_POKEMON: ;
			mensajeGetBroker* msgGet = deserializarGetBroker (paqueteRespuesta->stream);
			log_info(gameboyLogger, "Recibi mensaje get pokemon: \n");
			log_info(gameboyLogger, "El pokemon es: %s\n", msgGet->pokemon);
			destruirGetBroker(msgGet);
			break;

		case LOCALIZED_POKEMON: ;
			mensajeLocalized* msgLocalized = deserializarLocalized(paqueteRespuesta->stream);
			log_info(gameboyLogger, "Recibi mensaje localized: \n");
			log_info(gameboyLogger, "El pokemon es: %s\n", msgLocalized->pokemon);
			log_info (gameboyLogger, "La cantidad es: %i\n", msgLocalized->cantidad);
//			for(int j = 0; j<msgLocalized->cantidad; j++){
//				log_info(gameboyLogger, "las posiciones son: %i\n", (msgLocalized->arrayPosiciones)[j]);
//			}
			//destruirLocalized(msgLocalized);
			break;
		}
}

void terminar_programa(t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
}
