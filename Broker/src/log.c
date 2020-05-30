/*
 * log.c
 *
 *  Created on: 29 may. 2020
 *      Author: utnso
 */

#include "log.h"
#include "broker.h"

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


t_log* iniciar_logger()
{
	t_log* logger;
	if((logger = log_create("logBroker.log", "BROKER", 1, LOG_LEVEL_INFO))==NULL){
		printf("No pude crear el logger del Broker\n");
		exit(1);
	}
	return logger;
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


//void terminar_programa(t_log* logger){
//	log_destroy(logger);
//}
