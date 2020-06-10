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
    pthread_mutex_lock(entrenadoresLibres->mutex);
    for(uint32_t i=0;i<list_size(entrenadoresLibres->lista);i++){
        dataEntrenador* entrenadorActual = (dataEntrenador*) list_get(entrenadoresLibres->lista,i);
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
    pthread_mutex_unlock(entrenadoresLibres->mutex);
    return idEntrenadorEnLista(entrenadorMasCercano);}

dataEntrenador* obtenerEntrenadorPorId(uint32_t id){
	return (dataEntrenador*)getListaMutex(entrenadores,id);
}

uint32_t idEntrenadorEnLista(dataEntrenador* entrenadorMasCercano){
    pthread_mutex_lock(entrenadoresLibres->mutex);
	for(uint32_t j=0;j<sizeListaMutex(entrenadores);j++){
        if((dataEntrenador*) getListaMutex(entrenadores,j) == entrenadorMasCercano){
            pthread_mutex_unlock(entrenadoresLibres->mutex);
        	return j;
        }
    }
	pthread_mutex_unlock(entrenadoresLibres->mutex);
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
							habilitarHiloEntrenador(entrenador->id); //preguntar si aca se mata el hilo
							addListaMutex(entrenadoresExit, (void*)entrenador);

						}else{
							//DEADLOCK
							log_info(teamLogger, "Inicio del algoritmo de detección de deadlock.");
							addListaMutex(entrenadoresDeadlock, (void*) entrenador);
							if(todosLosEntrenadoresTerminaronDeAtrapar()){
								log_info(teamLogger, "Se encontró deadlock.");
								resolverDeadlock();
							}else{
								log_info(teamLogger, "No se encontró deadlock.");
							}
						}
	}
}

bool todosLosEntrenadoresTerminaronDeAtrapar(){
	return (sizeListaMutex(entrenadoresDeadlock)+sizeListaMutex(entrenadoresExit))==sizeListaMutex(entrenadores);
}
bool cumplioObjetivo(dataEntrenador* entrenador){
	return mismaListaPokemones(entrenador->objetivoPersonal, entrenador->pokemones);
}

void asignarPokemonAEntrenador(dataEntrenador* entrenador, pokemonPosicion* pokePosicion){
	loggearPokemonAAtrapar(pokePosicion, teamLogger);
//	if(entrenador->pokemonAAtrapar!=NULL){
//		free(entrenador->pokemonAAtrapar);//HACER DESTRUIR POKEMONAATRAPAR
//	}
	entrenador->pokemonAAtrapar=pokePosicion;
	entrenador->estado=READY;
	habilitarHiloEntrenador(entrenador->id);
}

void destruirPokemonPosicion(pokemonPosicion* poke){
	free(poke->pokemon);
	free(poke);
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
		entrarEnEjecucion(infoEntrenador); //despues de esto enviaria el catch, recibe id y se pone en BLOCKED
		infoEntrenador->estado=BLOCKED;//IMPORTANTE: CUANDO LLEGUE LA RESPUESTA DEL CATCH SE TIENE QUE HACER UN UNLOCK AL ENTRENADOR CORRESPONDIENTE
		sem_wait(semaforoEntrenador);// ESPERA A QUE EL TEAM LE AVISE QUE LLEGO LA RESPUESTA DEL POKEMON QUE QUISO ATRAPAR
		//meter un if() para verificar estado y ver que hacer despues


		if(infoEntrenador->estado==BLOCKED && leFaltaCantidadDePokemones(infoEntrenador)){
			addListaMutex(entrenadoresLibres, (void*)infoEntrenador);//vuelve a agregar al entrenador a la lista de entrenadores libres
		}

		while(entrenadorEnDeadlock(infoEntrenador) && infoEntrenador!=entrenadorBloqueadoParaDeadlock){
			sem_wait(semaforoEntrenador);
			infoEntrenador->estado=READY;
			poneteEnReady(infoEntrenador);
			entrarEnEjecucionParaDeadlock(infoEntrenador);


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
	for(uint32_t i=0;i<sizeListaMutex(entrenadoresLibres);i++){
	        dataEntrenador* entrenadorActual = (dataEntrenador*) getListaMutex(entrenadoresLibres,i);
	        if(entrenadorActual->id == entrenador->id){
	        	return i;
	        }
	        }

	 return -1;
}

void entrarEnEjecucion(dataEntrenador* infoEntrenador){

	sem_wait(&(infoEntrenador->semaforo));
	log_info(teamLogger2,"El entrenador %i entro en ejecución.", infoEntrenador->id);
	infoEntrenador->estado = EXEC;
	moverEntrenadorAPosicion(infoEntrenador, ((infoEntrenador->pokemonAAtrapar)->posicion));
	enviarCatch(infoEntrenador);
	sem_post(&semaforoEjecucionCpu);
}

void seleccionarEntrenador(pokemonPosicion* pokemon){
	uint32_t idEntrenadorMasCercano      = obtenerIdEntrenadorMasCercano(pokemon->posicion);
	dataEntrenador* entrenadorMasCercano = getListaMutex(entrenadores,idEntrenadorMasCercano);
	asignarPokemonAEntrenador(entrenadorMasCercano,pokemon);
	log_info(teamLogger2, "Se selecciono el entrenador %i para atrapar al pokemon %s", entrenadorMasCercano->id,pokemon->pokemon);
	//habilitarHiloEntrenador(idEntrenadorMasCercano);
}

void habilitarHiloEntrenador(uint32_t idEntrenador){
	sem_post(&(((dataEntrenador*)(getListaMutex(entrenadores,idEntrenador)))->semaforo));
}

void moverEntrenadorAPosicion(dataEntrenador* entrenador, posicion pos){
	log_info(teamLogger2, "El entrenador estaba originalmente en la posicion (%i,%i).",(entrenador->posicion).x,(entrenador->posicion).y);
	log_info(teamLogger2, "El entrenador comenzara a mover a la posicion (%i,%i).",pos.x,pos.y);

	uint32_t restaX=pos.x-(entrenador->posicion).x;
	uint32_t restaY=pos.y-(entrenador->posicion).y;

	moverEntrenadorX(entrenador, restaX);
	moverEntrenadorY(entrenador, restaY);
	log_info(teamLogger, "El entrenador %i se movió a la posición (%i, %i)\n", entrenador->id, (entrenador->posicion).x, (entrenador->posicion).y);
	log_info(teamLogger2, "El entrenador %i se movió a la posición (%i, %i)\n", entrenador->id, (entrenador->posicion).x, (entrenador->posicion).y);
}

void simularCicloCpu(uint32_t cantidadCiclos, dataEntrenador* entrenador){
	sleep(retardoCicloCpu*cantidadCiclos);
	entrenador->cantidadCiclosCpu += cantidadCiclos;
	team->cantidadCiclosCpuTotales+=cantidadCiclos;
}

void moverEntrenadorX(dataEntrenador* entrenador, uint32_t movimientoX){

//	if(movimientoX!=0){
//			uint32_t unidad=movimientoX/abs(movimientoX);
//			log_info(teamLogger2,"Unidad de movimiento en x: %i.", unidad);
//			for(uint32_t i=0;i< abs(movimientoX);i++){
//				simularCicloCpu(1,entrenador);
//				(entrenador->posicion).x+=unidad;
//				log_info(teamLogger2,"El entrenador %i se mueve a : (%i,%i).", entrenador->id,(entrenador->posicion).x,(entrenador->posicion).y);
//			}
//		}
	//log_info(teamLogger2, "Movimiento x: %i", movimientoX);
	if(movimientoX>0){//ESTA AL REVES PERO ASI ANDA

		for(uint32_t i=0;i< abs(movimientoX);i++){
			simularCicloCpu(1,entrenador);
			((entrenador->posicion).x)--;
			log_info(teamLogger2,"El entrenador %i se mueve a : (%i,%i).", entrenador->id,(entrenador->posicion).x,(entrenador->posicion).y);
		}
	}else if(movimientoX<0){

		for(uint32_t i=0;i< abs(movimientoX);i++){
			simularCicloCpu(1,entrenador);
			((entrenador->posicion).x)++;
			log_info(teamLogger2,"El entrenador %i se mueve a : (%i,%i).", entrenador->id,(entrenador->posicion).x,(entrenador->posicion).y);
			}
	}

}

void moverEntrenadorY(dataEntrenador* entrenador, uint32_t movimientoY){
//	if(movimientoY!=0){
//		uint32_t unidad=movimientoY/abs(movimientoY);
//		log_info(teamLogger2,"Unidad de movimiento en y: %i.", unidad);
//		for(uint32_t i=0;i< abs(movimientoY);i++){
//			simularCicloCpu(1,entrenador);
//			(entrenador->posicion).y+=unidad;
//			log_info(teamLogger2,"El entrenador %i se mueve a : (%i,%i).", entrenador->id,(entrenador->posicion).x,(entrenador->posicion).y);
//		}
//	}
	log_info(teamLogger2, "Movimiento y: %i", movimientoY);
	if(movimientoY>0){
			for(uint32_t i=0;i< abs(movimientoY);i++){//ESTA AL REVES PERO ASI ANDA
				simularCicloCpu(1,entrenador);
				((entrenador->posicion).y)--;
				log_info(teamLogger2,"El entrenador %i se mueve a : (%i,%i).", entrenador->id,(entrenador->posicion).x,(entrenador->posicion).y);
			}
		}else if(movimientoY<0){
			log_info(teamLogger2, "Movimiento y: %i", movimientoY);
			for(uint32_t i=0;i< abs(movimientoY);i++){
				simularCicloCpu(1,entrenador);
				((entrenador->posicion).y)++;
				log_info(teamLogger2,"El entrenador %i se mueve a : (%i,%i).", entrenador->id,(entrenador->posicion).x,(entrenador->posicion).y);
				}
		}
}

void atraparPokemonYReplanificar (dataEntrenador* entrenador){
	list_add(entrenador->pokemones,(void*)(entrenador->pokemonAAtrapar->pokemon));
	registrarPokemonAtrapado(entrenador->pokemonAAtrapar->pokemon);
	log_info(teamLogger,"El entrenador %i atrapó al pokemon %s en la posición (%i,%i).", entrenador->id,entrenador->pokemonAAtrapar->pokemon,
			(entrenador->pokemonAAtrapar->posicion).x,(entrenador->pokemonAAtrapar->posicion).y);

	replanificarEntrenador(entrenador);
}

t_list* obtenerPokemonesSobrantes(dataEntrenador* entrenador){//lista de pokemonSobrante
		t_list* pokemonesSobrantes=list_create();
		uint32_t i;
		t_list* copiaObjetivo = list_duplicate(entrenador->objetivoPersonal);//DESTRUIR
		for(i=0;i<list_size(entrenador->pokemones);i++){
			char *pokemonAComparar = (char*) list_get(entrenador->pokemones,i);
			uint32_t encontrado    = buscarMismoPokemon(copiaObjetivo,pokemonAComparar);

			if(encontrado != -1){
				list_remove(copiaObjetivo,encontrado);
			}else{
				pokemonSobrante* pokeSobrante=malloc(sizeof(pokemonSobrante));

				pokeSobrante->pokemon=malloc(strlen(pokemonAComparar)+1);
				strcpy(pokeSobrante->pokemon,pokemonAComparar);
				pokeSobrante->entrenador=entrenador;
				list_add(pokemonesSobrantes,(void*)pokeSobrante);
			}
		}
		list_destroy(copiaObjetivo);
		return pokemonesSobrantes;

}

t_list* obtenerPokemonesFaltantes(dataEntrenador* entrenador){
	t_list* pokemonesFaltantes=list_create();
			uint32_t i;
			t_list* copiaPokemones = list_duplicate(entrenador->pokemones);//DESTUIR
			for(i=0;i<list_size(entrenador->objetivoPersonal);i++){
				char *pokemonAComparar = (char*) list_get(entrenador->objetivoPersonal,i);
				uint32_t encontrado    = buscarMismoPokemon(copiaPokemones,pokemonAComparar);

				if(encontrado != -1){

					char* pokemonAAgregar=malloc(strlen(pokemonAComparar)+1);
					strcpy(pokemonAAgregar,pokemonAComparar);
					list_add(pokemonesFaltantes, (void*)pokemonAAgregar);
				}else{
					list_remove(copiaPokemones,encontrado);

				}
			}
			list_destroy(copiaPokemones);

			return pokemonesFaltantes;
}

bool pokemonLeInteresa(dataEntrenador* entrenador, char* pokemon){
	t_list* listaPokemonesFaltantes=obtenerPokemonesFaltantes(entrenador);
	uint32_t i=buscarMismoPokemon(listaPokemonesFaltantes,pokemon);
	list_destroy(listaPokemonesFaltantes);
	return i>=0;
}

void darPokemon(dataEntrenador* entrenadorDador, dataEntrenador* entrenadorReceptor, char* pokemon){
	uint32_t posPokemon=buscarMismoPokemon(entrenadorDador->pokemones,pokemon);
	list_remove(entrenadorDador->pokemones, posPokemon);//ESTO PUEDE ROMPER EN ALGUN LADO, CUANDO SACO UN ELEMENTO DE UNA LISTA DEBO HACER MEMCPY O STRCPY
	list_add(entrenadorReceptor->pokemones,(void*)pokemon);

}
