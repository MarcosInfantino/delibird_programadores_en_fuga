/*
 * planificador.c
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */
#include "Team.h"

uint32_t crearHiloPlanificador(pthread_t* hiloPlanificador){
	uint32_t err=pthread_create(hiloPlanificador,NULL,iniciarPlanificador,NULL);

						if(err!=0){
							printf("Hubo un problema en la creación del hilo para el planificador \n");
							return err;
						}

			pthread_detach(*hiloPlanificador);
			return 0;
}

void* iniciarPlanificador(void* arg){
	log_info(teamLogger2, "Se inicia el planificador");
	switch(algoritmoPlanificacion)
	{
	case FIFO:
		ejecucionPlanificadorFifo();break;

	}

	return NULL;
}

void ejecucionPlanificadorFifo(){
	sem_post(&semaforoEjecucionCpu);
	while(1){
		if(sizeColaMutex(colaEjecucionFifo)>0){
			sem_wait(&semaforoEjecucionCpu);
			dataEntrenador* entrenadorAEjecutar=(dataEntrenador*)popColaMutex(colaEjecucionFifo);
			sem_post(&(entrenadorAEjecutar->semaforo));
			log_info(teamLogger2, "elijo entrenador para ejecutar");
		}
	}
}

void obtenerAlgoritmoPlanificacion(t_config* config){
	char* algoritmo=config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	if(strcmp(algoritmo,"FIFO")==0){
		algoritmoPlanificacion=FIFO;
	}
}

