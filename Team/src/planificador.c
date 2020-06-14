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
		colaEjecucionFifo=inicializarColaMutex();
		ejecucionPlanificadorFifo();
		break;
	case RR:
		colaEjecucionFifo=inicializarColaMutex();
		ejecucionPlanificadorRR();
		break;
	case SJF:
		listaEjecucionSjf=inicializarListaMutex();
		ejecucionPlanificadorSjf();
		break;
	case SJFCD:
		listaEjecucionSjf=inicializarListaMutex();
		ejecucionPlanificadorSjfConDesalojo();
		break;
	default:
		break;
	}

	return NULL;
}

void ejecucionPlanificadorFifo(){
	sem_post(&semaforoEjecucionCpu);
	while(1){
	//	if(sizeColaMutex(colaEjecucionFifo)>0){
			sem_wait(entrenadorEnCola); //OK6
			sem_wait(&semaforoEjecucionCpu);
			//ACA VA CAMBIO DE CONTEXTO

			dataEntrenador* entrenadorAEjecutar=(dataEntrenador*)popColaMutex(colaEjecucionFifo);
			ponerEnEjecucion(entrenadorAEjecutar);
			sem_post((entrenadorAEjecutar->semaforo)); //OK4
			log_info(teamLogger2, "elijo entrenador para ejecutar");
	//	}
	}
}

void ejecucionPlanificadorRR(){
	sem_post(&semaforoEjecucionCpu);
		while(1){
		//	if(sizeColaMutex(colaEjecucionFifo)>0){//despues borrarlo
				sem_wait(entrenadorEnCola); //OK6
				sem_wait(&semaforoEjecucionCpu);

				dataEntrenador* entrenadorAEjecutar=(dataEntrenador*)popColaMutex(colaEjecucionFifo);
				if(fueInterrumpido(entrenadorAEjecutar)){
					retomarEjecucion(entrenadorAEjecutar);
					sem_post((entrenadorAEjecutar->semaforoContinuarEjecucion));
				}else{
					ponerEnEjecucion(entrenadorAEjecutar);
					sem_post((entrenadorAEjecutar->semaforo)); //OK1 //OK4
				}
				setearTimer(quantumRR, entrenadorAEjecutar);
				//ACA VA CAMBIO DE CONTEXTO

				log_info(teamLogger2, "elijo entrenador para ejecutar");
			//}
		}
}

void ejecucionPlanificadorSjf(){
	sem_post(&semaforoEjecucionCpu);
			while(1){

					sem_wait(entrenadorEnCola); //OK6
					sem_wait(&semaforoEjecucionCpu);
					//ACA VA CAMBIO DE CONTEXTO

					dataEntrenador* entrenadorAEjecutar=sacarEntrenadorMenorEstimacion();//encontrar el mas copado

					ponerEnEjecucion(entrenadorAEjecutar);
					sem_post((entrenadorAEjecutar->semaforo)); //OK1 //OK4
					log_info(teamLogger2, "elijo entrenador para ejecutar");

			}
}

void ejecucionPlanificadorSjfConDesalojo(){
	sem_post(&semaforoEjecucionCpu);
				while(1){

						sem_wait(entrenadorEnCola); //OK6
						sem_wait(&semaforoEjecucionCpu);

						dataEntrenador* entrenadorAEjecutar=sacarEntrenadorMenorEstimacion();//encontrar el mas copado


						if(fueInterrumpido(entrenadorAEjecutar)){
							retomarEjecucion(entrenadorAEjecutar);
							sem_post((entrenadorAEjecutar->semaforoContinuarEjecucion));
						}else{
							ponerEnEjecucion(entrenadorAEjecutar);
							sem_post((entrenadorAEjecutar->semaforo)); //OK1 //OK4
						} //OK1 //OK4

						verificarDesalojo(entrenadorAEjecutar);

						//ACA VA CAMBIO DE CONTEXTO

						log_info(teamLogger2, "elijo entrenador para ejecutar");

				}
}

uint32_t obtenerPosicionEntrenadorMenorEstimacion(){
	uint32_t posMejorEntrenador=-1;
	double mejorEstimacion;
	//dataEntrenador* mejorEntrenador;
	for(uint32_t i=0; i<sizeListaMutex(listaEjecucionSjf);i++){
		dataEntrenador* entrenadorActual=(dataEntrenador*)getListaMutex(listaEjecucionSjf,i);
		double estimacionActual=obtenerEstimacion(entrenadorActual);
		if(estimacionActual<mejorEstimacion || i==0){
			mejorEstimacion=estimacionActual;
			//mejorEntrenador=entrenadorActual;
			posMejorEntrenador=i;
		}
	}
	//removeListaMutex(listaEjecucionSjf,posMejorEntrenador);
	return posMejorEntrenador;
}

dataEntrenador* sacarEntrenadorMenorEstimacion(){
	uint32_t pos= obtenerPosicionEntrenadorMenorEstimacion();
	if(pos!=-1){
		dataEntrenador* entrenador=(dataEntrenador*)getListaMutex(listaEjecucionSjf,( pos ));
		removeListaMutex(listaEjecucionSjf, pos);
		return entrenador;
	}else{
		return NULL;
	}

}

dataEntrenador* obtenerEntrenadorMenorEstimacion(){
	uint32_t pos= obtenerPosicionEntrenadorMenorEstimacion();
	if(pos!=-1){
		return (dataEntrenador*)getListaMutex(listaEjecucionSjf,( pos ));
	}else{
		return NULL;
	}

}

uint32_t verificarDesalojo(dataEntrenador* entrenador){
	while(1){
			sleep(1);
			dataEntrenador* entrenadorMenorEstimacion=obtenerEntrenadorMenorEstimacion();
			if(!estaEjecutando(entrenador)){
				return 0;
			}else if(entrenadorMenorEstimacion!=NULL && obtenerEstimacion(entrenador)> obtenerEstimacion( obtenerEntrenadorMenorEstimacion() )){
				interrumpir(entrenador);
				log_info(teamLogger2, "interrumpo al entrendador %i ", entrenador->id);
				return 0;
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
	}else if(strcmp(algoritmo, "SJF")==0){
		algoritmoPlanificacion=SJF;
		log_info(teamLogger2, "SJF sin desalojo.");
	}else if(strcmp(algoritmo, "SJFCD")==0){
		algoritmoPlanificacion=SJFCD;
		log_info(teamLogger2, "Algortimo planificacion SJF con desalojo.");
	}else{
		log_info(teamLogger2, "El algoritmo ingresado no está contemplado.");
	}
}

