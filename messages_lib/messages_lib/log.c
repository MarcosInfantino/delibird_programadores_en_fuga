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
	if((logger=log_create(file, program_name, false, LOG_LEVEL_INFO))==NULL){
		printf("No pude crear el logger\n");
		exit(1);
	}
	return logger;
}

void loggearMensaje (paquete* paqueteRespuesta, t_log* logger){
	switch(paqueteRespuesta->tipoMensaje){
		case APPEARED_POKEMON: ;
			mensajeAppeared* msgAppeared=deserializarAppeared(paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje appeared pokemon. El pokemon es: %s. La posicion es: (%i,%i). \n",
					msgAppeared->pokemon, msgAppeared->posX,msgAppeared->posY);

			destruirAppeared(msgAppeared);
			break;

		case NEW_POKEMON: ;
			mensajeNew* msgNew = deserializarNew(paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje new pokemon. El pokemon es: %s. La posicion es: (%i,%i). La cantidad es: %i.\n",
					msgNew->pokemon,msgNew->posX, msgNew->posY,msgNew->cantidad);

			destruirNew(msgNew);
			break;

		case CATCH_POKEMON: ;
			mensajeCatch* msgCatch = deserializarCatch (paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje catch pokemon. El pokemon es: %s. La posicion es: (%i,%i).\n",
					msgCatch->pokemon,msgCatch->posX,msgCatch->posY);

			destruirCatch(msgCatch);
			break;

		case CAUGHT_POKEMON: ;
			mensajeCaught* msgCaught = deserializarCaught (paqueteRespuesta->stream);

			if(msgCaught->resultadoCaught==CORRECTO){
				log_info(logger, "Recibi mensaje caught pokemon. El pokemon fue atrapado. \n");
			}else{
				log_info(logger, "Recibi mensaje caught pokemon. El pokemon no fue atrapado. \n");
			}
			destruirCaught(msgCaught);
			break;

		case GET_POKEMON: ;
			mensajeGet* msgGet = deserializarGet (paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje get pokemon. El pokemon es: %s. \n", msgGet->pokemon);
			destruirGet(msgGet);
			break;

		case LOCALIZED_POKEMON: ;
			mensajeLocalized* msgLocalized = deserializarLocalized(paqueteRespuesta->stream);
			log_info(logger, "Recibi mensaje localized. El pokemon es: %s. La cantidad es: %i.\n",
					msgLocalized->pokemon, msgLocalized->cantidad);
			log_info(logger,"Las posiciones son: ");
			posicion* posActual=malloc(sizeof(posicion));
			for(int j = 0; j<msgLocalized->cantidad; j++){
				*posActual=*((msgLocalized->arrayPosiciones) + j);
				log_info(logger, "(%i,%i)", posActual->x, posActual->y);
			}
			free(posActual);
			destruirLocalized(msgLocalized);
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

/*char* armarStringSuscripLog(uint32_t modulo, uint32_t cola){
	char* suscripcionDeUnProceso = "Se suscribrió el proceso ";
	char* suscripcionAcola       = " a la cola ";
	char* cadena = malloc(sizeof(suscripcionDeUnProceso) + sizeof(suscripcionAcola) + sizeof(nombreDeProceso(modulo)) + sizeof(nombreDeCola(cola)) + 1);
	strcpy(cadena,suscripcionDeUnProceso);
	strcat(cadena, nombreDeProceso(modulo));
	strcat(cadena, suscripcionAcola );
	strcat(cadena, nombreDeCola(cola));

	return cadena;
}*/


char* armarStringMsgNuevoLog(uint32_t cola){
	char * mensajeNuevoDeProceso = "Llegó un nuevo mensaje a la cola ";
	char * cadena = malloc(sizeof(mensajeNuevoDeProceso) + sizeof(nombreDeCola(cola)) + 1);
	strcpy(cadena,mensajeNuevoDeProceso);
	strcat(cadena, nombreDeCola(cola));
	return cadena;

}

char* armarConexionNuevoProcesoLog(uint32_t modulo){
	char* str = "Se conectó un proceso ";
	char * conexionDeProceso = malloc(strlen(str) + strlen(nombreDeProceso(modulo)) + 1);
	strcpy(conexionDeProceso,str);
    strcat(conexionDeProceso, nombreDeProceso(modulo));

    return conexionDeProceso;
}


char* armarStringACK(uint32_t cola, uint32_t idMensaje, uint32_t socket ){ //falta concatenar el número de mensaje y socket
	char * mensaje = "Se recibió un ACK de la cola: ";
	char * mensajeNro = ", el id de mensaje es: ";
	char * suscriptor = "y el suscriptor es: ";
	char * cadena = malloc(sizeof(mensaje) + sizeof(nombreDeCola(cola)) + sizeof(mensajeNro) + sizeof(suscriptor) + 1);
	strcpy(cadena,mensaje);
	strcat(cadena, nombreDeCola(cola));
	strcat(cadena, mensajeNro);
	strcat(cadena, suscriptor);
	return cadena;
}

void terminar_programa(t_log* logger, t_config* config)
{
	log_destroy(logger);
	config_destroy(config);
}
