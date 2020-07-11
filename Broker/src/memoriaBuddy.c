
#include "broker.h"
#include "memoria.h"
//
//void registrarEnMemoriaBUDDYSYSTEM(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual){
//	struct nodoMemoria* backUp = partActual;
//	uint32_t response = intentarRamaIzquierda(mensajeNuevo, partActual);
//
//	if(response == 0){
//		partActual = backUp->hijoDer;
//		registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual);
//	}
//
//	if(response == 0){ //TODO VER que condición poner para que entre acá
//		elegirVictimaDeReemplazoYeliminarBD();
//		registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, nodoRaizMemoria);
//	}
//}

void registrarEnMemoriaBUDDYSYSTEM(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual){
	struct nodoMemoria* backUp = partActual;
	if(intentarRamaIzquierda(mensajeNuevo, partActual) < 0){
		partActual = backUp->hijoDer;
		registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual);
	}
}

void elegirVictimaDeReemplazoYeliminarBD(){
	struct nodoMemoria* victima;

	if(algoritmoReemplazo == FIFO){
		victima = buscarVictimaPor(tiempoDeCargaMenor);
	}else{ //LRU
		victima = buscarVictimaPor(tiempoDeUsoMenor);
		log_info(brokerLogger2,"VICTIMA: msg id %i" , victima->mensaje->idMensaje );
	}
	modificarNodoAlibre(victima);
}

struct nodoMemoria* buscarVictimaPor(bool(*condition)(struct nodoMemoria*,struct nodoMemoria*)){
	struct nodoMemoria* minimo = getListaMutex(nodosOcupados, 0);
	struct nodoMemoria* elNuevoNodo;

	for(uint32_t i=1; i<sizeListaMutex(nodosOcupados);i++){
		elNuevoNodo = getListaMutex(nodosOcupados, i);

		if(condition(elNuevoNodo,minimo)){
			minimo = elNuevoNodo;
		}
	}
	return minimo;
}

void modificarNodoAlibre(struct nodoMemoria* victima){
	victima->header.status = LIBRE;

	log_info(brokerLogger2,"ELIMINO el mensaje en nodo con offset: %i del msg con ID %i", victima->offset, victima->mensaje->idMensaje);
	log_info(brokerLogger2,"TIEMPO DE CARGA: %i:%i:%i ULTIMO ACCESO: %i:%i:%i",
			victima->header.tiempoDeCarga.tm_hour,
			victima->header.tiempoDeCarga.tm_min,
			victima->header.tiempoDeCarga.tm_sec,
			victima->header.ultimoAcceso.tm_hour,
			victima->header.ultimoAcceso.tm_min,
			victima->header.ultimoAcceso.tm_sec);
	removerDeListaOcupados(victima);
	evaluarConsolidacion(victima);
}

void evaluarConsolidacion(struct nodoMemoria* nodo){
	struct nodoMemoria* buddie;

	if(nodo->padre->hijoDer == nodo){
		buddie = nodo->padre->hijoIzq;
	}else{
		buddie = nodo->padre->hijoDer;
	}
	if(estaLibre(buddie)){
		nodo->padre->header.status = LIBRE;
		struct nodoMemoria* partActual = nodo->padre;
		log_info(brokerLogger2,"CONSOLIDO buddy con offset: %i con su buddy con offset %i", nodo->offset, buddie->offset);
		liberarNodo(nodo);
		liberarNodo(buddie);
		partActual->hijoDer = malloc(sizeof(struct nodoMemoria));
		partActual->hijoIzq = malloc(sizeof(struct nodoMemoria));
		evaluarConsolidacion(partActual);
	}else{
		return;
	}

}

uint32_t intentarRamaIzquierda(msgMemoriaBroker* mensajeNuevo,struct nodoMemoria* partActual){
	if (estaOcupado(partActual)){
	return 0;
	} else if (estaParticionado(partActual)){
		uint32_t retorno = intentarRamaIzquierda(mensajeNuevo,partActual->hijoIzq);
		if (retorno != 1)
			return intentarRamaIzquierda(mensajeNuevo,partActual->hijoDer);
		return retorno;
	}else{
		return evaluarTamanioParticionYasignar (partActual, mensajeNuevo);
	}
}

uint32_t evaluarTamanioParticionYasignar(struct nodoMemoria* partActual, msgMemoriaBroker* msg){
	uint32_t tamanioMsg = msg->sizeStream;
	if(tamanioParticion(partActual) >= tamanioMsg){
		while(tamanioParticion(partActual)/2 >= tamanioMsg && tamanioParticion(partActual)/2>=particionMinima){
			log_info(brokerLogger2,"PARTICIONO: para msg %d y la cola %s", msg->idMensaje, nombreDeCola(msg->cola));
			particionarMemoriaBUDDY(partActual);
			partActual->header.status = PARTICIONADO;
			partActual = partActual->hijoIzq;
		}
		partActual->header.status = OCUPADO;
		partActual->mensaje = msg;

		addListaMutex(nodosOcupados,partActual);

		time_t t;
		t=time(NULL);
		partActual->header.tiempoDeCarga = *localtime(&t);
		partActual->header.ultimoAcceso = *localtime(&t);
		asignarPuntero(partActual->offset, partActual->mensaje->stream, partActual->mensaje->sizeStream);
		log_info(brokerLogger2,"ASIGNE: Size de buddy: %i. Id mensaje: %i. Size del mensaje: %i.", (partActual->header).size, partActual->mensaje->idMensaje, partActual->mensaje->sizeStream);
		return 1;
	}

	return 0;
}

uint32_t tamanioMinimo(struct nodoMemoria* partActual){
	return (partActual->header).size;}


void particionarMemoriaBUDDY(struct nodoMemoria* particionActual){
	log_info(brokerLogger2,"NODO A PARTICIONAR: su offset %i, su tamaño: %i", particionActual->offset, particionActual->header.size);
	particionActual->hijoIzq = inicializarNodo();
	particionActual->hijoDer = inicializarNodo();
	uint32_t tamanoHijos = tamanioParticion(particionActual)/2;

	particionActual->hijoIzq->header.status = LIBRE;
	particionActual->hijoDer->header.status = LIBRE;
	particionActual->hijoIzq->header.size   = tamanoHijos;
	particionActual->hijoDer->header.size   = tamanoHijos;

	particionActual->hijoIzq->padre   = particionActual;
	particionActual->hijoDer->padre   = particionActual;

	particionActual->hijoIzq->offset = particionActual->offset;
	particionActual->hijoDer->offset = particionActual->offset + tamanioParticion(particionActual)/2;

	particionActual->header.status = PARTICIONADO;
}

msgMemoriaBroker* buscarMensajeEnMemoriaBuddy(uint32_t id){
	pthread_mutex_lock(mutexMemoria);
	//struct nodoMemoria* backUp = nodoActual;
	struct nodoMemoria* nodoActual = nodoRaizMemoria;

	if(estaLibre(nodoActual)) {
		pthread_mutex_unlock(mutexMemoria);
		return NULL;}

	if(nodoActual->mensaje->idMensaje != id){
		msgMemoriaBroker* mensajeEnRamaIzq = buscarPorRama(id, nodoActual->hijoIzq);
		msgMemoriaBroker* mensajeEnRamaDer;

		if(mensajeEnRamaIzq == NULL){
			mensajeEnRamaDer = buscarPorRama(id, nodoActual->hijoDer);
			pthread_mutex_unlock(mutexMemoria);
			return mensajeEnRamaDer;
		}
		pthread_mutex_unlock(mutexMemoria);
		return mensajeEnRamaIzq;
	}
	pthread_mutex_unlock(mutexMemoria);


	return nodoActual->mensaje;
}

bool existeMensajeEnMemoriaBuddy(mensajeGet* msgGet, mensajeCatch*  msgCatch){
	//struct nodoMemoria* backUp = nodoActual;
	pthread_mutex_lock(mutexMemoria);
	struct nodoMemoria* nodoActual = nodoRaizMemoria;
	struct nodoMemoria* aux;
	if(estaLibre(nodoActual)) {
		pthread_mutex_unlock(mutexMemoria);
		return false;
	}
	if(msgGet != NULL){
		log_info(brokerLogger2, "Existe Get");
		for (uint32_t o = 0; o < sizeListaMutex(nodosOcupados); o ++){
			aux = (struct nodoMemoria*) getListaMutex(nodosOcupados, o);
			if (aux->mensaje->cola == GET_POKEMON){
				if (compararGet(deserializarGet(aux->mensaje->stream),msgGet)){
					pthread_mutex_unlock(mutexMemoria);
					return true;
				}
			}
		}
		pthread_mutex_unlock(mutexMemoria);
		return false;
	}else if(msgCatch != NULL){
		log_info(brokerLogger2, "Existe Catch");
		for (uint32_t p = 0; p < sizeListaMutex(nodosOcupados); p ++){
			aux = (struct nodoMemoria*) getListaMutex(nodosOcupados, p);
					if (aux->mensaje->cola == CATCH_POKEMON){
						if (compararCatch(deserializarCatch(aux->mensaje->stream),msgCatch)){
							pthread_mutex_unlock(mutexMemoria);
							return true;
						}
					}
				}
		pthread_mutex_unlock(mutexMemoria);
		return false;
	}
	pthread_mutex_unlock(mutexMemoria);
	return true;
}

bool buscarPorRamaGet(mensajeGet* msgGet, struct nodoMemoria* partActual ){
	if (estaOcupado(partActual) && compararGet(deserializarGet(partActual->mensaje->stream), msgGet)){
		return partActual->mensaje;
	}else if (estaParticionado(partActual)){
		bool retorno = buscarPorRamaGet(msgGet,partActual->hijoIzq);
		if (!retorno)
			return buscarPorRamaGet(msgGet,partActual->hijoDer);
		return retorno;
	}else{
		return false;
	}
}

bool buscarPorRamaCatch(mensajeCatch* msgCatch, struct nodoMemoria* partActual ){
	if (estaOcupado(partActual) && compararCatch(deserializarCatch(partActual->mensaje->stream), msgCatch)){
		return partActual->mensaje;
	}else if (estaParticionado(partActual)){
		bool retorno = buscarPorRamaCatch(msgCatch,partActual->hijoIzq);
		if (!retorno)
			return buscarPorRamaCatch(msgCatch,partActual->hijoDer);
		return retorno;
	}else{
		return false;
	}
}

msgMemoriaBroker* buscarPorRama(uint32_t id, struct nodoMemoria* partActual ){
	if (estaOcupado(partActual) && (partActual->mensaje->idMensaje)==id){
		log_info(brokerLogger2, "Duermo para generar bache de 1 segundo y diferenciar LRU/FIFO");
		sleep(1);

		time_t sTime;
		sTime=time(NULL);
		partActual->header.ultimoAcceso = *localtime(&sTime); //actualizo la fecha de acceso

		log_info(brokerLogger2, "Encontre mensaje en mem: %i:%i:%i",
				partActual->header.ultimoAcceso.tm_hour,
				partActual->header.ultimoAcceso.tm_min,
				partActual->header.ultimoAcceso.tm_sec);
		return partActual->mensaje;
	}else if (estaParticionado(partActual)){
		msgMemoriaBroker* retorno = buscarPorRama(id,partActual->hijoIzq);
		if (retorno == NULL)
			return buscarPorRama(id,partActual->hijoDer);
		return retorno;
	}else{
		return NULL;
	}
}



struct nodoMemoria* crearRaizArbol(void){
	struct nodoMemoria* nodoRaiz = inicializarNodo();    //no estoy liberando malloc
	nodoRaiz->header.size   = tamMemoria;
	nodoRaiz->header.status = LIBRE;
	nodoRaiz->offset = 0;

	return nodoRaiz;
}

struct nodoMemoria* inicializarNodo(){
    struct nodoMemoria* nodo = malloc(sizeof(struct nodoMemoria));
	nodo->hijoDer = malloc(sizeof(struct nodoMemoria));
	nodo->hijoIzq = malloc(sizeof(struct nodoMemoria));
	nodo->mensaje = malloc(sizeof(msgMemoriaBroker));

	return nodo;
}

void liberarNodo(struct nodoMemoria* nodo){
//	free(nodo->mensaje->subsYaEnviado);
//	free(nodo->mensaje->subsACK);
//	free(nodo->mensaje->stream);
//    free(nodo->mensaje);
    free(nodo->hijoIzq);
    free(nodo->hijoDer);
    free(nodo);
}

void removerDeListaOcupados(struct nodoMemoria* nodo){
	struct nodoMemoria* aux;

	for(int i=0; i<sizeListaMutex(nodosOcupados);i++){
		aux = getListaMutex(nodosOcupados, i);
		if(aux == nodo){
			removeListaMutex(nodosOcupados,i);
		}
	}
}

bool ambosHijosOcupados(struct nodoMemoria* padre){
	return estaOcupado(padre->hijoDer) && estaOcupado(padre->hijoIzq);
}

bool esHijoDerecho(struct nodoMemoria* particion){
	return particion==(particion->padre->hijoDer);
}

bool estaOcupado(struct nodoMemoria* partActual){
	return (partActual->header).status==OCUPADO;
}

bool estaParticionado(struct nodoMemoria* partActual){
	return (partActual->header).status==PARTICIONADO;
}
bool entraEnLaMitad(struct nodoMemoria* partActual, msgMemoriaBroker* mensajeNuevo){
	return mensajeNuevo->sizeStream <= tamanioParticion(partActual);
}

bool esParticionMinima(struct nodoMemoria* particion){

	return (particion->header.size == particionMinima);
}

bool estaLibre(struct nodoMemoria* particion){
	return particion->header.status == LIBRE;
}

uint32_t tamanioParticion(struct nodoMemoria* part){
	return part->header.size;
}

struct tm tiempoCarga(struct nodoMemoria* nodo){
	return nodo->header.tiempoDeCarga;
}

struct tm tiempoUso(struct nodoMemoria* nodo){
	return nodo->header.ultimoAcceso;
}

void enviarMsjsASuscriptorNuevoBuddySystem(uint32_t colaParametro, uint32_t* socket){
	for(uint32_t varI = 0; varI < sizeListaMutex(nodosOcupados); varI ++){
		struct nodoMemoria* nodoEvaluado = (struct nodoMemoria*) getListaMutex (nodosOcupados, varI);
		if(nodoEvaluado->mensaje->cola == colaParametro){
			if(!envieMensajeDeNodoASocket(nodoEvaluado, socket)){
				paquete * paqueteAEnviar = llenarPaquete(nodoEvaluado->mensaje->modulo, nodoEvaluado->mensaje->cola, nodoEvaluado->mensaje->sizeStream,nodoEvaluado->mensaje->stream);
				paqueteAEnviar->id = nodoEvaluado->mensaje->idMensaje;
				paqueteAEnviar->idCorrelativo = nodoEvaluado->mensaje->idCorrelativo;
				void* paqStream = serializarPaquete(paqueteAEnviar);
				send(*socket, paqStream, sizePaquete(paqueteAEnviar), 0);
				free(paqStream);
				log_info(brokerLogger2, "GUARDO ENVIADO POR ENVIAR MENSAJES, id: %i, id cor: %i", paqueteAEnviar->id, paqueteAEnviar->idCorrelativo);
				guardarYaEnviados(paqueteAEnviar, *socket);
			}
		}
	}
}

bool envieMensajeDeNodoASocket(struct nodoMemoria* nodoEvaluado, uint32_t* socket){
	for(uint32_t varH = 0; varH < sizeListaMutex(nodoEvaluado->mensaje->subsYaEnviado); varH ++){
		uint32_t* socketEvaluado = (uint32_t*) getListaMutex (nodoEvaluado->mensaje->subsYaEnviado, varH);
		if (socketEvaluado == socket){
			return true;
		}
	}
	return false;
}




bool tiempoDeCargaMenor(struct nodoMemoria* nuevoNodo, struct nodoMemoria* minimoHastaElMomento){
	if (tiempoCarga(nuevoNodo).tm_hour < tiempoCarga(minimoHastaElMomento).tm_hour){
			return true;
		}else if(tiempoCarga(nuevoNodo).tm_min < tiempoCarga(minimoHastaElMomento).tm_min){
			return true;
		}else if (tiempoCarga(nuevoNodo).tm_sec < tiempoCarga(minimoHastaElMomento).tm_sec){
			return true;
		}
	return false;
}

bool tiempoDeUsoMenor(struct nodoMemoria* nuevoNodo, struct nodoMemoria* minimoHastaElMomento){
	if (tiempoUso(nuevoNodo).tm_hour < tiempoUso(minimoHastaElMomento).tm_hour){
		return true;
	}else if(tiempoUso(nuevoNodo).tm_min < tiempoUso(minimoHastaElMomento).tm_min){
		return true;
	}else if (tiempoUso(nuevoNodo).tm_sec < tiempoUso(minimoHastaElMomento).tm_sec){
		return true;
	}
	return false;
}
