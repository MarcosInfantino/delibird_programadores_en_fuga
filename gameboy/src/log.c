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

//terminar_programa(gameboyLogger);

t_log* iniciar_logger()
{
	t_log* logger;
	if((logger=log_create("gameboy.log", "GAMEBOY", 1, LOG_LEVEL_INFO))==NULL){
		printf("No pude crear el logger\n");
		exit(1);
	}
	return logger;
}

//void terminar_programa(t_log* logger)
//{
//	log_destroy(logger);
//}
