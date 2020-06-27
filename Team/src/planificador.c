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

void terminarPlanificador(){
	pthread_cancel(hiloPlanificador);
	switch(algoritmoPlanificacion){
		case FIFO:
		case RR:
			destruirColaMutex(colaEjecucionFifo);
			break;
		case SJF:
		case SJFCD:
			destruirListaMutex(listaEjecucionSjf);
			break;
	}
}

void ejecucionPlanificadorFifo(){
	sem_post(&semaforoEjecucionCpu);
	while(1){

			sem_wait(entrenadorEnCola);
			sem_wait(&semaforoEjecucionCpu);
			team->cantidadCambiosContexto++;
			//ACA VA CAMBIO DE CONTEXTO

			dataEntrenador* entrenadorAEjecutar=(dataEntrenador*)popColaMutex(colaEjecucionFifo);
			ponerEnEjecucion(entrenadorAEjecutar);
			sem_post((entrenadorAEjecutar->semaforo));
			log_info(teamLogger2, "elijo entrenador para ejecutar");

	}
}

void ejecucionPlanificadorRR(){

		while(1){


				sem_wait(entrenadorEnCola);
				team->cantidadCambiosContexto++;


				dataEntrenador* entrenadorAEjecutar=(dataEntrenador*)popColaMutex(colaEjecucionFifo);

				log_info(teamLogger2, "Elijo al entrenador %i para ejecutar.", entrenadorAEjecutar->id);

				if(fueInterrumpido(entrenadorAEjecutar)){
					retomarEjecucion(entrenadorAEjecutar);


				}else{
					ponerEnEjecucion(entrenadorAEjecutar);
					sem_post((entrenadorAEjecutar->semaforo)); //OK1 //OK4
					esperarPedidoCicloCpu(entrenadorAEjecutar);
				}

				pthread_t hiloTimer;
				pthread_create(&hiloTimer, NULL, setearTimer, (void*) entrenadorAEjecutar);
				pthread_detach(hiloTimer);

				//ACA VA CAMBIO DE CONTEXTO

				sem_wait(&semaforoEjecucionCpu);
				pthread_cancel(hiloTimer);
				if(esperaPedido>0){
					sem_post(entrenadorAEjecutar->semaforoPedidoCiclo);
					esperaPedido--;
					}

		}
}

void ejecucionPlanificadorSjf(){
	sem_post(&semaforoEjecucionCpu);
			while(1){

					sem_wait(entrenadorEnCola);
					sem_wait(&semaforoEjecucionCpu);
					team->cantidadCambiosContexto++;
					//ACA VA CAMBIO DE CONTEXTO

					dataEntrenador* entrenadorAEjecutar=sacarEntrenadorMenorEstimacion();//encontrar el mas copado

					ponerEnEjecucion(entrenadorAEjecutar);
					sem_post((entrenadorAEjecutar->semaforo));
					log_info(teamLogger2, "elijo entrenador para ejecutar");

			}
}

void ejecucionPlanificadorSjfConDesalojo(){

				while(1){

						sem_wait(entrenadorEnCola);
						team->cantidadCambiosContexto++;

						dataEntrenador* entrenadorAEjecutar=sacarEntrenadorMenorEstimacion();//encontrar el mas copado
						log_info(teamLogger2, "Elijo al entrenador %i para ejecutar.", entrenadorAEjecutar->id);



						if(fueInterrumpido(entrenadorAEjecutar)){
								retomarEjecucion(entrenadorAEjecutar);


						}else{
								ponerEnEjecucion(entrenadorAEjecutar);
								sem_post((entrenadorAEjecutar->semaforo));
								esperarPedidoCicloCpu(entrenadorAEjecutar);
								log_info(teamLogger2, "Le llego el pedido de ciclo del entrenador %i al planificador. ", entrenadorAEjecutar->id);
						}

						pthread_t hiloVerificacionDesalojo;
						pthread_create(&hiloVerificacionDesalojo,NULL,verificarDesalojo,(void*) entrenadorAEjecutar);
						pthread_detach(hiloVerificacionDesalojo);

						//ACA VA CAMBIO DE CONTEXTO
						sem_wait(&semaforoEjecucionCpu);
						pthread_cancel(hiloVerificacionDesalojo);

						if(esperaPedido>0){
							sem_post(entrenadorAEjecutar->semaforoPedidoCiclo);
							esperaPedido--;
						}




				}
}

uint32_t obtenerPosicionEntrenadorMenorEstimacion(){
	uint32_t posMejorEntrenador=-1;
	double mejorEstimacion;

	for(uint32_t i=0; i<sizeListaMutex(listaEjecucionSjf);i++){
		dataEntrenador* entrenadorActual=(dataEntrenador*)getListaMutex(listaEjecucionSjf,i);
		double estimacionActual=obtenerEstimacion(entrenadorActual);
		if( i==0 || estimacionActual<mejorEstimacion ){
			mejorEstimacion=estimacionActual;

			posMejorEntrenador=i;
		}
	}

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

void* verificarDesalojo(void* arg){
	dataEntrenador* entrenador=(dataEntrenador*) arg;
	while(1){
			habilitarCiclo(entrenador);
			esperarTerminoCiclo();
			esperarPedidoCicloCpu(entrenador);
			dataEntrenador* entrenadorMenorEstimacion=obtenerEntrenadorMenorEstimacion();

			if(entrenadorMenorEstimacion!=NULL && obtenerEstimacion(entrenador)> obtenerEstimacion( entrenadorMenorEstimacion )){
				interrumpir(entrenador);
				log_info(teamLogger2, "interrumpo al entrendador %i ", entrenador->id);
				return NULL;
			}

		}
	return NULL;

}

void* setearTimer(void* arg){

	uint32_t quantum=quantumRR;
	dataEntrenador* entrenador=(dataEntrenador*) arg;

	while(quantum>0){
		habilitarCiclo(entrenador);

		esperarTerminoCiclo();
		quantum--;
		esperarPedidoCicloCpu(entrenador);

	}

	//solo sale del while si hizo un pedido de más, en caso contrario se queda esperando en el while y el hilo es terminado
	interrumpir(entrenador);

	return NULL;
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

void pedirCicloCpu(dataEntrenador* entrenador){
	sem_post(entrenador->semaforoPedidoCiclo);
	int valorSemaforo=-500;
	sem_getvalue(entrenador->semaforoPedidoCiclo,&valorSemaforo);
	//log_info(teamLogger2,"El entrenador %i pide un ciclo. Valor del semaforo: %i. ", entrenador->id, valorSemaforo);
}

int esperarPedidoCicloCpu(dataEntrenador* entrenador){
	int valorSemaforo=-500;
	sem_getvalue(entrenador->semaforoPedidoCiclo,&valorSemaforo);

	esperaPedido++;
	sem_wait(entrenador->semaforoPedidoCiclo);
	esperaPedido--;

	return 0;
}
