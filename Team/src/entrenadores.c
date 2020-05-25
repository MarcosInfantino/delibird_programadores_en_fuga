/*
 * entrenadores.c
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */


#include "Team.h"
uint32_t distanciaEntrePosiciones(posicion pos1, posicion pos2){

	return abs((pos1.x)-(pos2.x))+abs((pos1.y)-(pos2.y));
}

uint32_t distanciaEntrenadorPosicion(dataEntrenador* entrenador, posicion posicion){

	return distanciaEntrePosiciones(entrenador->posicion,posicion);
}

uint32_t obtenerIdEntrenadorMasCercano(posicion pos){ //el id es el index del entrenador enla lista de entrenadores
    dataEntrenador* entrenadorMasCercano;
    uint32_t distanciaASuperar;
    pthread_mutex_lock(entrenadoresLibres.mutex);
    for(uint32_t i=0;i<list_size(entrenadoresLibres.lista);i++){
        dataEntrenador* entrenadorActual = (dataEntrenador*) list_get(entrenadoresLibres.lista,i);
        if (i == 0){
            entrenadorMasCercano = entrenadorActual;
            distanciaASuperar    = distanciaEntrenadorPosicion(entrenadorActual,pos);
        } else {
        if(distanciaEntrenadorPosicion(entrenadorActual,pos) < distanciaASuperar){
            entrenadorMasCercano = entrenadorActual;
            distanciaASuperar    = distanciaEntrenadorPosicion(entrenadorActual,pos);
        }
    }

}
    pthread_mutex_unlock(entrenadoresLibres.mutex);
    return idEntrenadorEnLista(entrenadorMasCercano);}

dataEntrenador* obtenerEntrenadorPorId(uint32_t id){
	return (dataEntrenador*)getListaMutex(entrenadores,id);
}

uint32_t idEntrenadorEnLista(dataEntrenador* entrenadorMasCercano){
    pthread_mutex_lock(entrenadoresLibres.mutex);
	for(uint32_t j=0;j<sizeListaMutex(entrenadores);j++){
        if((dataEntrenador*) getListaMutex(entrenadores,j) == entrenadorMasCercano){
            pthread_mutex_unlock(entrenadoresLibres.mutex);
        	return j;
        }
    }
	pthread_mutex_unlock(entrenadoresLibres.mutex);
    return -1;
}

bool leFaltaCantidadDePokemones(dataEntrenador* entrenador){
	return !(list_size(entrenador->pokemones)==list_size(entrenador->objetivoPersonal));
}

void replanificarEntrenador(dataEntrenador* entrenador){
	if(leFaltaCantidadDePokemones(entrenador)){
		if(sizeColaMutex(pokemonesPendientes)>0){
			habilitarHiloEntrenador(entrenador->id);
			pokemonPosicion* pokePosicion=(pokemonPosicion*)popColaMutex(pokemonesPendientes);
			asignarPokemonAEntrenador(entrenador, pokePosicion);
			free(pokePosicion);
		}else{
			entrenador->estado=BLOCKED;
			habilitarHiloEntrenador(entrenador->id);
			//addListaMutex(entrenadoresLibres,entrenador);

		}
	}else{
			if(cumplioObjetivo(entrenador)){
							entrenador->estado=EXIT;
							habilitarHiloEntrenador(entrenador->id);//preguntar si aca se mata el hilo
							addListaMutex(entrenadoresExit, (void*)entrenador);

						}else{
							//DEADLOCK
						}
	}
}

bool cumplioObjetivo(dataEntrenador* entrenador){
	uint32_t i;
	for(i=0;i<list_size(entrenador->objetivoPersonal);i++){
		char* pokemon=list_get(entrenador->objetivoPersonal,i);
		if(buscarMismoPokemon(entrenador->pokemones,pokemon)==-1)
			return false;
	}
	return true;
}

void asignarPokemonAEntrenador(dataEntrenador* entrenador, pokemonPosicion* pokePosicion){
	if(entrenador->pokemonAAtrapar!=NULL){
		free(entrenador->pokemonAAtrapar);
	}
	entrenador->pokemonAAtrapar=pokePosicion;
	entrenador->estado=READY;
	habilitarHiloEntrenador(entrenador->id);
}

int inicializarEntrenadores(t_list* entrenadores){
	uint32_t i;

	for(i=0;i<list_size(entrenadores);i++){
		void* entrenadorActual = list_get(entrenadores,i);
		uint32_t err		   = pthread_create(&(arrayIdHilosEntrenadores[i]),NULL,ejecucionHiloEntrenador,entrenadorActual);
		if(err!=0){
			printf("Hubo un problema en la creación del hilo del entrenador \n");
			return err;
		}

//		}else{
//			printf("Todo bien\n");
//		}
		pthread_detach(arrayIdHilosEntrenadores[i]);
	}
	return 0;
}

void* ejecucionHiloEntrenador(void* argEntrenador){
	dataEntrenador* infoEntrenador=(dataEntrenador*) argEntrenador;
	sem_t* semaforoEntrenador=&(infoEntrenador->semaforo);
	while(1){
		sem_wait(semaforoEntrenador);
		infoEntrenador->estado=READY;
		poneteEnReady(infoEntrenador);

		removeListaMutex(entrenadoresLibres,encontrarPosicionEntrenadorLibre(infoEntrenador));
		entrarEnEjecucion(infoEntrenador);
		//despues de esto enviaria el catch, recibe id y se pone en BLOCKED

		infoEntrenador->estado=BLOCKED;//IMPORTANTE: CUANDO LLEGUE LA RESPUESTA DEL CATCH SE TIENE QUE HACER UN UNLOCK AL ENTRENADOR CORRESPONDIENTE
		sem_wait(semaforoEntrenador);// ESPERA A QUE EL TEAM LE AVISE QUE LLEGO LA RESPUESTA DEL POKEMON QUE QUISO ATRAPAR
		//meter un if() para verificar estado y ver que hacer despues
		if(infoEntrenador->estado==BLOCKED){
			addListaMutex(entrenadoresLibres, (void*)infoEntrenador);//vuelve a agregar al entrenador a la lista de entrenadores libres
		}
	}
	return NULL;
}

void poneteEnReady(dataEntrenador* entrenador){
	switch(algoritmoPlanificacion){
		case FIFO:
			pushColaMutex(colaEjecucionFifo,(void*)entrenador);
	}
}
uint32_t encontrarPosicionEntrenadorLibre(dataEntrenador* entrenador){
	pthread_mutex_lock(entrenadoresLibres.mutex);
	for(uint32_t i=0;i<list_size(entrenadoresLibres.lista);i++){
	        dataEntrenador* entrenadorActual = (dataEntrenador*) list_get(entrenadoresLibres.lista,i);
	        if(entrenadorActual->id == entrenador->id){
	        	pthread_mutex_unlock(entrenadoresLibres.mutex);
	        	return i;
	        }
	        }
	 pthread_mutex_unlock(entrenadoresLibres.mutex);
	 return -1;
}

void entrarEnEjecucion(dataEntrenador* infoEntrenador){

	sem_wait(&(infoEntrenador->semaforo));
	infoEntrenador->estado = EXEC;
	moverEntrenadorAPosicion(infoEntrenador, ((infoEntrenador->pokemonAAtrapar)->posicion));
	enviarCatch(infoEntrenador);
	sem_post(&semaforoEjecucionCpu);
}

void seleccionarEntrenador(pokemonPosicion* pokemon){
	uint32_t idEntrenadorMasCercano      = obtenerIdEntrenadorMasCercano(pokemon->posicion);
	dataEntrenador* entrenadorMasCercano = getListaMutex(entrenadores,idEntrenadorMasCercano);
	asignarPokemonAEntrenador(entrenadorMasCercano,pokemon);
	//habilitarHiloEntrenador(idEntrenadorMasCercano);
}

void habilitarHiloEntrenador(uint32_t idEntrenador){
	sem_post(&(((dataEntrenador*)(getListaMutex(entrenadores,idEntrenador)))->semaforo));
}

void moverEntrenadorAPosicion(dataEntrenador* entrenador, posicion pos){
	uint32_t restaX=pos.x-(entrenador->posicion).x;
	uint32_t restaY=pos.y-(entrenador->posicion).y;
	moverEntrenadorX(entrenador, restaX);
	moverEntrenadorY(entrenador, restaY);
}

void moverEntrenadorX(dataEntrenador* entrenador, uint32_t movimientoX){

	if(movimientoX!=0){
			uint32_t unidad=movimientoX/abs(movimientoX);
			for(uint32_t i=0;i< abs(movimientoX);i++){
			sleep(retardoCicloCpu);
				(entrenador->posicion).x+=unidad;
			}
		}
}

void moverEntrenadorY(dataEntrenador* entrenador, uint32_t movimientoY){
	if(movimientoY!=0){
		uint32_t unidad=movimientoY/abs(movimientoY);
		for(uint32_t i=0;i< abs(movimientoY);i++){
		sleep(retardoCicloCpu);
			(entrenador->posicion).y+=unidad;
		}
	}
}
