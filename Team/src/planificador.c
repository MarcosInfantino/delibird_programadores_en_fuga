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

void* iniciarPlanificador(void* arg ){
	log_info(teamLogger2, "Se inicia el planificador");
	switch(algoritmoPlanificacion)
	{
	case FIFO:
		ejecucionPlanificadorFifo();break;
	case RR:
		ejecucionPlanificadorRR(); break;

	}

	return NULL;
}

void ejecucionPlanificadorFifo(){
	sem_post(&semaforoEjecucionCpu);
	while(1){
		if(sizeColaMutex(colaEjecucionFifo)>0){
			sem_wait(entrenadorEnCola);
			sem_wait(&semaforoEjecucionCpu);
			dataEntrenador* entrenadorAEjecutar=(dataEntrenador*)popColaMutex(colaEjecucionFifo);
			ponerEnEjecucion(entrenadorAEjecutar);
			sem_post((entrenadorAEjecutar->semaforo));
			log_info(teamLogger2, "elijo entrenador para ejecutar");
		}
	}
}

void ejecucionPlanificadorRR(){
	sem_post(&semaforoEjecucionCpu);
		while(1){
			if(sizeColaMutex(colaEjecucionFifo)>0){
				sem_wait(entrenadorEnCola);
				sem_wait(&semaforoEjecucionCpu);

				dataEntrenador* entrenadorAEjecutar=(dataEntrenador*)popColaMutex(colaEjecucionFifo);
				if(fueInterrumpido(entrenadorAEjecutar)){
					retomarEjecucion(entrenadorAEjecutar);
					sem_post((entrenadorAEjecutar->semaforoContinuarEjecucion));
				}else{
					ponerEnEjecucion(entrenadorAEjecutar);
					sem_post((entrenadorAEjecutar->semaforo));
				}
				setearTimer(quantumRR, entrenadorAEjecutar);
				log_info(teamLogger2, "elijo entrenador para ejecutar");
			}
		}
}

uint32_t setearTimer(uint32_t quantum, dataEntrenador* entrenador){

	while(quantum>0){
		if(!estaEjecutando(entrenador)){
			return 0;
		}
		sleep(1);
		quantum--;
	}

	if(estaEjecutando(entrenador)){
		interrumpir(entrenador);
	}
	return 0;
}

void obtenerAlgoritmoPlanificacion(t_config* config){
	entrenadorEnCola=malloc(sizeof(sem_t));
	sem_init(entrenadorEnCola, 0,0);
	char* algoritmo=config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	if(strcmp(algoritmo,"FIFO")==0){
		log_info(teamLogger2, "Algortimo de planificación: FIFO.");
		algoritmoPlanificacion=FIFO;
	}else if(strcmp(algoritmo, "RR")==0){
		algoritmoPlanificacion=RR;
		quantumRR=config_get_int_value(config, "QUANTUM");
		log_info(teamLogger2, "Algortimo de planificación: RR con Q=%i.", quantumRR);
	}
}

