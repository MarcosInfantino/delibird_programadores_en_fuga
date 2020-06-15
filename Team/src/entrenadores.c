/*
 * entrenadores.c
 *
 *  Created on: 25 may. 2020
 *      Author: utnso
 */


#include "Team.h"

//----------------------------------------------GENERAL-------------------------------------------------------

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
	sem_t* semaforoEntrenador=(infoEntrenador->semaforo);
	while(1){
		sem_wait(semaforoEntrenador); //OK1

		removeListaMutex(entrenadoresLibres,encontrarPosicionEntrenadorLibre(infoEntrenador));
		poneteEnReady(infoEntrenador);

		entrarEnEjecucion(infoEntrenador); //despues de esto enviaria el catch, recibe id y se pone en BLOCKED
		//IMPORTANTE: CUANDO LLEGUE LA RESPUESTA DEL CATCH SE TIENE QUE HACER UN UNLOCK AL ENTRENADOR CORRESPONDIENTE
		sem_wait(semaforoEntrenador);//OK2 // ESPERA A QUE EL TEAM LE AVISE QUE LLEGO LA RESPUESTA DEL POKEMON QUE QUISO ATRAPAR
		//log_info(teamLogger2, "El entrenador %i salio de la espera a la respuesta caught.",infoEntrenador->id);

		if(infoEntrenador->estado==BLOCKED && leFaltaCantidadDePokemones(infoEntrenador)){
			//log_info(teamLogger2, "El entrenador %i queda libre.",infoEntrenador->id);
			addListaMutex(entrenadoresLibres, (void*)infoEntrenador);//vuelve a agregar al entrenador a la lista de entrenadores libres
		}else{
			//log_info(teamLogger2, "El entrenador %i no queda libre.",infoEntrenador->id);
		}

		while(entrenadorEnDeadlock(infoEntrenador)){// && infoEntrenador!=entrenadorBloqueadoParaDeadlock){
			//log_info(teamLogger2, "El entrenador %i entra a la ejecucion entra al while del deadlock.", infoEntrenador->id);
			sem_wait(semaforoEntrenador); //OK3
			poneteEnReady(infoEntrenador);
			entrarEnEjecucionParaDeadlock(infoEntrenador);

		}
	}
	return NULL;
}

void habilitarHiloEntrenador(uint32_t idEntrenador){
	log_info(teamLogger, "Habilito el hilo del entrenador %i.", idEntrenador);
	sem_post((((dataEntrenador*)(getListaMutex(entrenadores,idEntrenador)))->semaforo)); //OK2 //OK1
}

void entrarEnEjecucion(dataEntrenador* infoEntrenador){

	sem_wait((infoEntrenador->semaforo)); //OK4
	loggearPokemonAAtrapar(infoEntrenador, teamLogger);

	//log_info(teamLogger2,"El entrenador %i entro en ejecución.", infoEntrenador->id);

	ponerEnEjecucion(infoEntrenador);
	moverEntrenadorAPosicion(infoEntrenador, ((infoEntrenador->pokemonAAtrapar)->posicion));
	enviarCatch(infoEntrenador);

	poneteEnBlocked(infoEntrenador);

	actualizarRafagaAnterior(infoEntrenador);
	sem_post(&semaforoEjecucionCpu);
	log_info(teamLogger2, "El entrenador %i libera la CPU por su cuenta.", infoEntrenador->id);
}

void replanificarEntrenador(dataEntrenador* entrenador){

	if(leFaltaCantidadDePokemones(entrenador)){
		if(sizeColaMutex(pokemonesPendientes)>0){
			log_info(teamLogger2, "El entrenador %i va a buscar pokemones pendientes.", entrenador->id);
			pokemonPosicion* pokePosicion=(pokemonPosicion*)popColaMutex(pokemonesPendientes);
			asignarPokemonAEntrenador(entrenador, pokePosicion);
			free(pokePosicion);
		}else{
			log_info(teamLogger2, "El entrenador %i se bloquea porque no hay pokemones para atrapar.", entrenador->id);
			poneteEnBlocked(entrenador);
			habilitarHiloEntrenador(entrenador->id);
			//addListaMutex(entrenadoresLibres,entrenador);
		}

	}else{
		if(cumplioObjetivo(entrenador)){
			log_info(teamLogger2, "El entrenador %i cumplio su objetivo.", entrenador->id);
			poneteEnExit(entrenador);
			habilitarHiloEntrenador(entrenador->id); //preguntar si aca se mata el hilo
			addListaMutex(entrenadoresExit, (void*)entrenador);
			if(objetivoCumplido()){
				sem_post(semaforoObjetivoCumplido);
			}
			}else{	//DEADLOCK
				log_info(teamLogger2, "El entrenador %i forma parte de un interbloqueo.", entrenador->id);
				log_info(teamLogger2, "Inicio del algoritmo de detección de deadlock.");
				log_info(teamLogger, "Inicio del algoritmo de detección de deadlock.");
				addListaMutex(entrenadoresDeadlock, (void*) entrenador);
				if(todosLosEntrenadoresTerminaronDeAtrapar()){
					log_info(teamLogger, "Se encontró deadlock.");
					habilitarHiloEntrenador(entrenador->id);
					sem_post(iniciarResolucionDeadlock);
					//resolverDeadlock();
					}else{
						habilitarHiloEntrenador(entrenador->id);
						log_info(teamLogger, "No se encontró deadlock.");
						log_info(teamLogger2, "No se encontró deadlock.");
					}
				}
	}
}

bool cumplioObjetivo(dataEntrenador* entrenador){
	return mismaListaPokemones(entrenador->objetivoPersonal, entrenador->pokemones);
}

void destruirPokemonPosicion(pokemonPosicion* poke){
	free(poke->pokemon);
	free(poke);
}

//---------------------------------------ESTADO ENTRENADORES---------------------------------------------------
bool estaEjecutando(dataEntrenador* entrenador){
	return entrenador->estado==EXEC;
}

void ponerEnEjecucion(dataEntrenador* entrenador){
	entrenador->estado=EXEC;
	log_info(teamLogger,"El entrenador %i ahora está en EXEC.",entrenador->id);
}

void poneteEnNew(dataEntrenador* entrenador){
	entrenador->estado=NEW;
	log_info(teamLogger,"El entrenador %i ahora está en NEW.",entrenador->id);
}

void poneteEnReady(dataEntrenador* entrenador){
	log_info(teamLogger2,"El entrenador %i se pone en READY.", entrenador->id);
	switch(algoritmoPlanificacion){
		case RR:
		case FIFO:
			entrenador->estado=READY;
			pushColaMutex(colaEjecucionFifo,(void*)entrenador);
			sem_post(entrenadorEnCola); //OK6
			break;
		case SJF:
		case SJFCD:

			entrenador->estado=READY;
			addListaMutex(listaEjecucionSjf,(void*)entrenador);
			sem_post(entrenadorEnCola);
			//ACA CAMBIO DE CONTEXTO
			break;
	}
	log_info(teamLogger,"El entrenador %i ahora está en READY.",entrenador->id);
}

void poneteEnExit(dataEntrenador* entrenador){
	entrenador->estado=EXIT;
	addListaMutex(entrenadoresExit, entrenador);
	log_info(teamLogger,"El entrenador %i ahora está en EXIT.",entrenador->id);
}

void poneteEnBlocked(dataEntrenador* entrenador){
	entrenador->estado=BLOCKED;
	log_info(teamLogger,"El entrenador %i ahora está en BLOCKED.",entrenador->id);
}

bool estaBloqueado(dataEntrenador* entrenador){
	return entrenador->estado==BLOCKED;
}

void interrumpir(dataEntrenador* entrenador){
	guardarContexto(entrenador);
	poneteEnReady(entrenador);
	sem_post(&semaforoEjecucionCpu);
	log_info(teamLogger2, "El entrenador %i fue sacado de ejecución.", entrenador->id);

}

bool fueInterrumpido(dataEntrenador* entrenador){
	return entrenador->ejecucionEnPausa;
}

//--------------------------------------CONTADOR RAFAGAS--------------------------------------

contadorRafagas* inicializarContadorRafagas(){
	contadorRafagas* contador=malloc(sizeof(contadorRafagas));
	contador->cantRafagas=0;
	contador->mutex=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(contador->mutex,NULL);

	return contador;
}

void incrementarContadorRafagas(dataEntrenador* entrenador){
	pthread_mutex_lock(entrenador->contadorCpu->mutex);
	(entrenador->contadorCpu->cantRafagas)++;
	pthread_mutex_unlock(entrenador->contadorCpu->mutex);
}

uint32_t obtenerContadorRafagas(dataEntrenador* entrenador){
	uint32_t i;
	pthread_mutex_lock(entrenador->contadorCpu->mutex);
	i=(entrenador->contadorCpu->cantRafagas);
	pthread_mutex_unlock(entrenador->contadorCpu->mutex);
	return i;
}

void resetearContadorRafagas(dataEntrenador* entrenador){
	pthread_mutex_lock(entrenador->contadorCpu->mutex);
	(entrenador->contadorCpu->cantRafagas)=0;
	pthread_mutex_unlock(entrenador->contadorCpu->mutex);
}

//-----------------------------------------------ASIGNACION POKEMON A ENTRENADOR--------------------------------

void seleccionarEntrenador(pokemonPosicion* pokemon){
	uint32_t idEntrenadorMasCercano      = obtenerIdEntrenadorMasCercano(pokemon->posicion);
	dataEntrenador* entrenadorMasCercano = getListaMutex(entrenadores,idEntrenadorMasCercano);
	asignarPokemonAEntrenador(entrenadorMasCercano,pokemon);
	log_info(teamLogger2, "Se selecciono el entrenador %i para atrapar al pokemon %s", entrenadorMasCercano->id,pokemon->pokemon);
	//habilitarHiloEntrenador(idEntrenadorMasCercano);
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
    return idEntrenadorEnLista(entrenadorMasCercano);
}

void asignarPokemonAEntrenador(dataEntrenador* entrenador, pokemonPosicion* pokePosicion){

//	if(entrenador->pokemonAAtrapar!=NULL){
//		free(entrenador->pokemonAAtrapar);//HACER DESTRUIR POKEMONAATRAPAR
//	}
	entrenador->pokemonAAtrapar=pokePosicion;
	entrenador->estado=READY;

	habilitarHiloEntrenador(entrenador->id);
}

void atraparPokemonYReplanificar (dataEntrenador* entrenador){
	list_add(entrenador->pokemones,(void*)(entrenador->pokemonAAtrapar->pokemon));
	registrarPokemonAtrapado(entrenador->pokemonAAtrapar->pokemon);
	log_info(teamLogger,"El entrenador %i atrapó al pokemon %s en la posición (%i,%i).", entrenador->id,entrenador->pokemonAAtrapar->pokemon,
			(entrenador->pokemonAAtrapar->posicion).x,(entrenador->pokemonAAtrapar->posicion).y);

	replanificarEntrenador(entrenador);
}

void moverEntrenadorAPosicion(dataEntrenador* entrenador, posicion pos){

	log_info(teamLogger2, "El entrenador %i, que estaba en (%i,%i), comenzará a moverse a (%i,%i)."
			,entrenador->id,(entrenador->posicion).x,(entrenador->posicion).y,pos.x,pos.y);

	int32_t restaX=pos.x-(entrenador->posicion).x;
	int32_t restaY=pos.y-(entrenador->posicion).y;

	moverEntrenadorX(entrenador, restaX);
	moverEntrenadorY(entrenador, restaY);
	log_info(teamLogger, "El entrenador %i se movió a la posición (%i, %i).", entrenador->id, (entrenador->posicion).x, (entrenador->posicion).y);
	log_info(teamLogger2, "El entrenador %i se movió a la posición (%i, %i).", entrenador->id, (entrenador->posicion).x, (entrenador->posicion).y);
}

void moverEntrenadorX(dataEntrenador* entrenador, int32_t movimientoX){

	if(movimientoX!=0){
			uint32_t unidad=movimientoX/abs(movimientoX);

			for(uint32_t i=0;i< abs(movimientoX);i++){
				simularCicloCpu(1,entrenador);
				(entrenador->posicion).x+=unidad;
				log_info(teamLogger2,"El entrenador %i se mueve a : (%i,%i).", entrenador->id,(entrenador->posicion).x,(entrenador->posicion).y);
			}
		}
}

void moverEntrenadorY(dataEntrenador* entrenador, int32_t movimientoY){
	if(movimientoY!=0){
		uint32_t unidad=movimientoY/abs(movimientoY);

		for(uint32_t i=0;i< abs(movimientoY);i++){
			simularCicloCpu(1,entrenador);
			(entrenador->posicion).y+=unidad;
			log_info(teamLogger2,"El entrenador %i se mueve a : (%i,%i).", entrenador->id,(entrenador->posicion).x,(entrenador->posicion).y);
		}
	}
}

uint32_t distanciaEntrePosiciones(posicion pos1, posicion pos2){

	return abs((pos1.x)-(pos2.x))+abs((pos1.y)-(pos2.y));
}

uint32_t distanciaEntrenadorPosicion(dataEntrenador* entrenador, posicion posicion){

	return distanciaEntrePosiciones(entrenador->posicion,posicion);
}

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
	//log_info(teamLogger2, "Pregunto si al entrenador %i le faltan pokemones.",entrenador->id);
	return !(list_size(entrenador->pokemones)==list_size(entrenador->objetivoPersonal));
}

//------------------------------ALGORITMOS DE PLANI---------------------------------------------

double obtenerEstimacion(dataEntrenador* entrenador){
	if((entrenador->rafagaCpuAnterior)==0){
		return (double)entrenador->estimacionAnterior;
	}else{
		return ((double)(entrenador->rafagaCpuAnterior)+(double)(entrenador->estimacionAnterior))/2 - (double)obtenerContadorRafagas(entrenador);
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

void actualizarRafagaAnterior(dataEntrenador* entrenador){
	entrenador->rafagaCpuAnterior=obtenerContadorRafagas(entrenador);
	resetearContadorRafagas(entrenador);
}

void retomarEjecucion(dataEntrenador* entrenador){
	log_info(teamLogger2, "Retomo la ejecución del entrenador %i.", entrenador->id);
	entrenador->ejecucionEnPausa=false;
	ponerEnEjecucion(entrenador);
}

void guardarContexto(dataEntrenador* entrenador){
	entrenador->ejecucionEnPausa=true;
}
//--------------------------------------------------------------DEADLOCK------------------------------------

bool todosLosEntrenadoresTerminaronDeAtrapar(){
	return (sizeListaMutex(entrenadoresDeadlock)+sizeListaMutex(entrenadoresExit))==sizeListaMutex(entrenadores);
}

void entrarEnEjecucionParaDeadlock(dataEntrenador* infoEntrenador){
	//log_info(teamLogger, "El entrenador %i se mueve a la posición del entrenador %i.", infoEntrenador->id, entrenadorBloqueadoParaDeadlock->id);

	sem_wait((infoEntrenador->semaforo));//espera al planificador //OK4


	ponerEnEjecucion(infoEntrenador);
	moverEntrenadorAPosicion(infoEntrenador, ((infoEntrenador->pokemonAAtrapar)->posicion));
	realizarIntercambio(infoEntrenador);
	poneteEnBlocked(infoEntrenador);

	actualizarRafagaAnterior(infoEntrenador);
	sem_post(&semaforoEjecucionCpu);
}

void esperarHabilitacionPlanificador(dataEntrenador* entrenador){
	switch(algoritmoPlanificacion){
		case SJFCD:
		case RR:
			sem_wait(entrenador->semaforoContinuarEjecucion);
			break;
		default:
			break;
	}
}

void avisarFinalizacionCicloAlPlanificador(){
	switch(algoritmoPlanificacion){
			case SJFCD:
			case RR:
				sem_post(finalizacionCicloCpu);
				break;
			default:
				break;
		}
}

void habilitarCiclo(dataEntrenador* entrenador){
	sem_post(entrenador->semaforoContinuarEjecucion);
}

void esperarTerminoCiclo(){
	sem_wait(finalizacionCicloCpu);
}
void simularCicloCpu(uint32_t cantidadCiclos, dataEntrenador* entrenador){
	for(uint32_t i=0;i<cantidadCiclos;i++){
		simularUnidadCicloCpu(entrenador);
	}

}

void simularUnidadCicloCpu(dataEntrenador* entrenador){
	pedirCicloCpu(entrenador);
	//esperarRespuestaPlanificador

	esperarHabilitacionPlanificador(entrenador);
//	if(fueInterrumpido(entrenador)){
//		guardarContexto(entrenador);
//		sem_post(&semaforoEjecucionCpu);
//		poneteEnReady(entrenador);
//		log_info(teamLogger2, "El entrenador %i fue sacado de ejecución.", entrenador->id);
//		sem_wait((entrenador->semaforoContinuarEjecucion));
//	}
	sleep(retardoCicloCpu);
	entrenador->cantidadCiclosCpu ++;
	incrementarContadorRafagas(entrenador);
	team->cantidadCiclosCpuTotales++;
	avisarFinalizacionCicloAlPlanificador();
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
				int32_t encontrado    = buscarMismoPokemon(copiaPokemones,pokemonAComparar);

				if(encontrado == -1){

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
	int32_t i=buscarMismoPokemon(listaPokemonesFaltantes,pokemon);
	list_destroy(listaPokemonesFaltantes);
	return i>=0;
}

void darPokemon(dataEntrenador* entrenadorDador, dataEntrenador* entrenadorReceptor, char* pokemon){
	uint32_t posPokemon=buscarMismoPokemon(entrenadorDador->pokemones,pokemon);
	list_remove(entrenadorDador->pokemones, posPokemon);//ESTO PUEDE ROMPER EN ALGUN LADO, CUANDO SACO UN ELEMENTO DE UNA LISTA DEBO HACER MEMCPY O STRCPY
	list_add(entrenadorReceptor->pokemones,(void*)pokemon);
	log_info(teamLogger, "El entrenador% i le da el pokemon %s al entrenador %i", entrenadorDador->id,pokemon, entrenadorReceptor->id);

}

bool tieneUnPokemonQueMeInteresa(dataEntrenador* entrenador1, dataEntrenador* entrenador2 ){
	t_list* pokemonesSobrantesEntrenador2=obtenerPokemonesSobrantes(entrenador2);
	for(uint32_t i=0; i<list_size(pokemonesSobrantesEntrenador2);i++){
		pokemonSobrante* pokeActual= (pokemonSobrante*) list_get(pokemonesSobrantesEntrenador2,i);
		if(pokemonLeInteresa(entrenador1,pokeActual->pokemon)){
			list_destroy(pokemonesSobrantesEntrenador2);
			return true;
		}
	}
	list_destroy(pokemonesSobrantesEntrenador2);
	return false;
}

//bool hayIntercambioMutuo(dataEntrenador* entrenador1, dataEntrenador* entrenador2){
//	return tieneUnPokemonQueMeInteresa(entrenador1,entrenador2) &&tieneUnPokemonQueMeInteresa(entrenador2,entrenador1);
//}

//dataEntrenador* encontrarEntrenadorParaIntercambioMutuo(listaMutex* listaEntrenadores ){
//	for(uint32_t i=0; i<sizeListaMutex(listaEntrenadores);i++){
//		dataEntrenador* entrenadorActual= (dataEntrenador*) (getListaMutex(listaEntrenadores,i));
//
//		for(uint32_t j=0;j<sizeListaMutex(listaEntrenadores);j++){
//			dataEntrenador* entrenadorActual2= (dataEntrenador*) (getListaMutex(listaEntrenadores,i));
//			if(hayIntercambioMutuo(entrenadorActual, entrenadorActual2))
//				return entrenadorActual;
//		}
//
//	}
//	return NULL;
//}

//dataEntrenador* seleccionarEntrenadorInteresante(dataEntrenador* entrenadorInteresado, listaMutex* listaEntrenadores){
//	for(uint32_t i=0; i<sizeListaMutex(listaEntrenadores);i++){
//			dataEntrenador* entrenadorActual= (dataEntrenador*) (getListaMutex(listaEntrenadores,i));
//
//				if(tieneUnPokemonQueMeInteresa(entrenadorInteresado,entrenadorActual))
//					return entrenadorActual;
//			}
//	return NULL;
//}

//uint32_t cuantosEntrenadoresInteresantesHay(dataEntrenador* entrenador,listaMutex* listaEntrenadores){
//	uint32_t contador=0;
//	for(uint32_t i=0; i<sizeListaMutex(listaEntrenadores);i++){
//				dataEntrenador* entrenadorActual= (dataEntrenador*) (getListaMutex(listaEntrenadores,i));
//		if(tieneUnPokemonQueMeInteresa(entrenador,entrenadorActual))
//			contador++;
//	}
//	return contador;
//}

//t_list* encontrarEsperaCircular(listaMutex* listaEntrenadores,t_list* entrenadoresEnEsperaCircular, dataEntrenador* actual){
//	dataEntrenador* primerEntrenador=(dataEntrenador*) getListaMutex(listaEntrenadores,0);
//	if(actual==primerEntrenador){
//		return entrenadoresEnEsperaCircular;
//	}
//	if(actual==NULL){
//		actual=getListaMutex(listaEntrenadores,0);
//	}
//
//		uint32_t cantEntrenadoresInteresantes =cuantosEntrenadoresInteresantesHay(actual,listaEntrenadores);
//
//		for(uint32_t i=0;i<sizeListaMutex(listaEntrenadores);i++){
//			t_list* listaEntrenadoresParcial=list_create();
//			dataEntrenador* entrenadorActual= (dataEntrenador*)getListaMutex(listaEntrenadores,i);
//
//		}
//	return entrenadoresEnEsperaCircular;
//}
