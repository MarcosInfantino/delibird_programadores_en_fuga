/*
 * log.c
 *
 *  Created on: 27 may. 2020
 *      Author: utnso
 */

//Conexión a cualquier proceso.
//Suscripción a una cola de mensajes.
//Llegada de un nuevo mensaje a una cola de mensajes.

#include "messages_lib.h"

t_log* iniciar_logger(char* file, char* program_name)
{
	t_log* logger;
	if((logger=log_create(file, program_name, 1, LOG_LEVEL_INFO))==NULL){
		printf("No pude crear el logger\n");
		exit(1);
	}
	return logger;
}

void loggearMensaje (paquete* paqueteRespuesta, t_log* logger){
	switch(paqueteRespuesta->tipoMensaje){
		case APPEARED_POKEMON: ;
			mensajeAppearedTeam* msgAppeared=deserializarAppearedTeam(paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje appeared pokemon: \n");
			log_info(logger, "El pokemon es: %s\n", msgAppeared->pokemon);
			log_info(logger, "La posX es: %i\n", msgAppeared->posX);
			log_info(logger, "La posY es: %i\n", msgAppeared->posY);
			//log_info(logger, "El id correlativo es: %i\n", msgAppeared->idCorrelativo);
			destruirAppearedTeam(msgAppeared);
			break;

		case NEW_POKEMON: ;
			mensajeNewBroker* msgNew = deserializarNewBroker(paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje new pokemon: \n");
			log_info(logger, "El pokemon es: %s\n", msgNew->pokemon);
			log_info(logger, "La posX es: %i\n", msgNew->posX);
			log_info(logger, "La posY es: %i\n", msgNew->posY);
			log_info(logger, "La cantidad es: %i\n", msgNew->cantidad);
			destruirNewBroker(msgNew);
			break;

		case CATCH_POKEMON: ;
			mensajeCatchBroker* msgCatch = deserializarCatchBroker (paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje catch pokemon: \n");
			log_info(logger, "El pokemon es: %s\n", msgCatch->pokemon);
			log_info(logger, "La posX es: %i\n", msgCatch->posX);
			log_info(logger, "La posY es: %s\n", msgCatch->posY);
			destruirCatchBroker(msgCatch);
			break;

		case CAUGHT_POKEMON: ;
			mensajeCaught* msgCaught = deserializarCaught (paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje caught pokemon: \n");
			log_info(logger, "El id correlativo es: %i\n", msgCaught->idCorrelativo);
			log_info(logger, "El resultado es: %i\n", msgCaught->resultadoCaught);
			destruirCaught(msgCaught);
			break;

		case GET_POKEMON: ;
			mensajeGetBroker* msgGet = deserializarGetBroker (paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje get pokemon: \n");
			log_info(logger, "El pokemon es: %s\n", msgGet->pokemon);
			destruirGetBroker(msgGet);
			break;

		case LOCALIZED_POKEMON: ;
			mensajeLocalized* msgLocalized = deserializarLocalized(paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje localized: \n");
			log_info(logger, "El pokemon es: %s\n", msgLocalized->pokemon);
			log_info (logger, "La cantidad es: %i\n", msgLocalized->cantidad);
//			for(int j = 0; j<msgLocalized->cantidad; j++){
//				log_info(logger, "las posiciones son: %i\n", (msgLocalized->arrayPosiciones)[j]);
//			}
			//destruirLocalized(msgLocalized);
			break;
		}
}

char* nombreDeProceso(uint32_t modulo){

	switch(modulo){
	case BROKER:
		return "Broker";
		break;
	case TEAM:
		return "Team";
		break;
	case GAMECARD:
		return "GameCard";
		break;
	case GAMEBOY:
		return "GameBoy";
		break;
	case -1:
		return "ERROR";
		break;
	}
	return "0";

}

char* nombreDeCola(uint32_t cola){

	switch(cola){
	case APPEARED_POKEMON:
		return "APPEARED_POKEMON";
		break;
	case NEW_POKEMON:
		return "NEW_POKEMON";
		break;
	case CAUGHT_POKEMON:
		return "CAUGHT_POKEMON";
		break;
	case CATCH_POKEMON:
		return "CATCH_POKEMON";
		break;
	case GET_POKEMON:
		return "GET_POKEMON";
		break;
	case LOCALIZED_POKEMON:
		return "LOCALIZED_POKEMON";
		break;
	case SUSCRIPCION:
		return "ES UNA SUSCRIPCION";
		break;
		case SUSCRIPCION_TIEMPO:
		return "ES UNA SUSCRIPCION POR TIEMPO DEL GAMEBOY";
		break;
	case -1:
		return "ERROR";
		break;
	}

	return "0";
}

char* armarStringSuscripLog(uint32_t modulo, uint32_t cola){
	char* suscripcionDeUnProceso = "Se suscribrió el proceso ";
	char* suscripcionAcola       = " a la cola ";
	strcat(suscripcionDeUnProceso, nombreDeProceso(modulo));
	strcat(suscripcionDeUnProceso,suscripcionAcola );
	strcat(suscripcionDeUnProceso ,nombreDeCola(cola));

	return suscripcionDeUnProceso;
}


char* armarStringMsgNuevoLog(uint32_t cola){

	char * mensajeNuevoDeProceso = "Llegó un nuevo mensaje a la cola ";
	strcat(mensajeNuevoDeProceso, nombreDeCola(cola));
	return mensajeNuevoDeProceso;

}

char* armarConexionNuevoProcesoLog(uint32_t modulo){
	char* str="Se conectó un proceso ";
	char * conexionDeProceso = malloc(strlen(str) + strlen(nombreDeProceso(modulo)) +1);
	strcpy(conexionDeProceso,str);
    strcat(conexionDeProceso, nombreDeProceso(modulo));

    return conexionDeProceso;
}

void terminar_programa(t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
}
