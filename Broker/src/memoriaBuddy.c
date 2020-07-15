#include "broker.h"
#include "memoria.h"

void registrarEnMemoriaBUDDYSYSTEM(msgMemoriaBroker* mensajeNuevo, struct nodoMemoria* partActual){
	struct nodoMemoria* backUp = partActual;
	if(intentarRamaIzquierda(mensajeNuevo, partActual) < 0){
		partActual = backUp->hijoDer;
		registrarEnMemoriaBUDDYSYSTEM(mensajeNuevo, partActual);
	}
}

void elegirVictimaDeReemplazoYeliminarBD(){
	struct nodoMemoria* victima;

	switch(algoritmoReemplazo){
	case FIFO:
		victima = buscarVictimaPor(fifoNodos);
		break;
	case LRU:
		victima = buscarVictimaPor(lruNodos);
		log_info(brokerLogger2,"VICTIMA: id de mensaje: %i" , victima->mensaje->idMensaje );
		break;
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

	removerDeListaBuddy(nodosOcupados,victima);
	addListaMutex(nodosLibres, victima);

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

		addListaMutex(nodosLibres, nodo->padre);
		removerDeListaBuddy(nodosLibres, buddie);
		removerDeListaBuddy(nodosLibres, nodo);

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
	//struct nodoMemoria* nodo;
	uint32_t tamanioMsg = msg->sizeStream;

	if(tamanioParticion(partActual) >= tamanioMsg){
		while(tamanioParticion(partActual)/2 >= tamanioMsg && tamanioParticion(partActual)/2>=particionMinima){
			log_info(brokerLogger2,"PARTICIONO: para msg %d y la cola %s", msg->idMensaje, nombreDeCola(msg->cola));
			particionarMemoriaBUDDY(partActual);
			partActual->header.status = PARTICIONADO;
			removerDeListaBuddy(nodosLibres, partActual);

			partActual = partActual->hijoIzq;
		}
		partActual->header.status = OCUPADO;
		partActual->mensaje = msg;

		addListaMutex(nodosOcupados,partActual);
		removerDeListaBuddy(nodosLibres, partActual);

		partActual->header.tiempoDeCarga=temporal_get_string_time();
		partActual->header.ultimoAcceso=temporal_get_string_time();
		asignarPuntero(partActual->offset, partActual->mensaje->stream, partActual->mensaje->sizeStream);

		msg->stream = memoria + partActual->offset;
		//TODO HAY QUE AGREGAR ESTO Y REVISAR QUE FUNCIONE. Es necesario, en particiones lo hicimos asi
		//Atte Mari y Marquitos :D

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

	addListaMutex(nodosLibres,particionActual->hijoIzq);
	addListaMutex(nodosLibres,particionActual->hijoDer);

	particionActual->hijoIzq->padre   = particionActual;
	particionActual->hijoDer->padre   = particionActual;

	particionActual->hijoIzq->offset = particionActual->offset;
	particionActual->hijoDer->offset = particionActual->offset + tamanioParticion(particionActual)/2;

	particionActual->header.status = PARTICIONADO;
}

msgMemoriaBroker* buscarMensajeEnMemoriaBuddy(uint32_t id){
	pthread_mutex_lock(mutexMemoria);
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
	pthread_mutex_lock(mutexMemoria);
	struct nodoMemoria* nodoActual = nodoRaizMemoria;
	struct nodoMemoria* aux;
	if(estaLibre(nodoActual)) {
		pthread_mutex_unlock(mutexMemoria);
		return false;
	}
	if(msgGet != NULL){
		log_info(brokerLogger2, "Valido si ya existe el mensaje Get en memoria buddy.");
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
		log_info(brokerLogger2, "Valido si ya existe el mensaje Catch en memoria buddy.");
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

		partActual->header.ultimoAcceso=temporal_get_string_time();
		log_info(brokerLogger2, "Encontré mensaje en memoria: %s", partActual->header.ultimoAcceso);

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
	struct nodoMemoria* nodoRaiz = inicializarNodo();
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
    free(nodo->hijoIzq);
    free(nodo->hijoDer);
    free(nodo);
}

void removerDeListaBuddy(listaMutex* lista, struct nodoMemoria* nodo){
	struct nodoMemoria* aux;

	for(int i=0; i<sizeListaMutex(lista);i++){
		aux = getListaMutex(lista, i);
		if(aux == nodo){
			removeListaMutex(lista,i);
		}
	}
}

bool ambosHijosOcupados(struct nodoMemoria* padre){
	return estaOcupado(padre->hijoDer) && estaOcupado(padre->hijoIzq);
}

bool esHijoDerecho(struct nodoMemoria* particion){
	return particion == (particion->padre->hijoDer);
}

bool estaOcupado(struct nodoMemoria* partActual){
	return (partActual->header).status == OCUPADO;
}

bool estaParticionado(struct nodoMemoria* partActual){
	return (partActual->header).status == PARTICIONADO;
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

char* tiempoCarga(struct nodoMemoria* nodo){
	return nodo->header.tiempoDeCarga;
}

char* tiempoUso(struct nodoMemoria* nodo){
	return nodo->header.ultimoAcceso;
}

void enviarMsjsASuscriptorNuevoBuddySystem(uint32_t colaParametro, uint32_t socket, uint32_t idProceso){
	for(uint32_t varI = 0; varI < sizeListaMutex(nodosOcupados); varI ++){
		struct nodoMemoria* nodoEvaluado = (struct nodoMemoria*) getListaMutex (nodosOcupados, varI);
		if(nodoEvaluado->mensaje->cola == colaParametro){
			if(!envieMensajeDeNodoAIdProceso(nodoEvaluado, idProceso)){
				paquete * paqueteAEnviar = llenarPaquete(nodoEvaluado->mensaje->modulo, nodoEvaluado->mensaje->cola, nodoEvaluado->mensaje->sizeStream,nodoEvaluado->mensaje->stream);

				paqueteAEnviar->id = nodoEvaluado->mensaje->idMensaje;
				paqueteAEnviar->idCorrelativo = nodoEvaluado->mensaje->idCorrelativo;
				void* paqStream = serializarPaquete(paqueteAEnviar);

				(nodoEvaluado->header).ultimoAcceso=temporal_get_string_time();

				log_info(brokerLogger2, "Actualizo el ultimo acceso del MENSAJE %i : %s ",nodoEvaluado->mensaje->idMensaje ,(nodoEvaluado->header).ultimoAcceso);
				send(socket, paqStream, sizePaquete(paqueteAEnviar), 0);
				free(paqStream);
				log_info(brokerLogger2, "GUARDO ENVIADO POR ENVIAR MENSAJES, id: %i, id cor: %i", paqueteAEnviar->id, paqueteAEnviar->idCorrelativo);
				guardarYaEnviados(paqueteAEnviar, idProceso);
 				//TODO memory leak para el paquete
			}
		}
	}
}

bool envieMensajeDeNodoAIdProceso(struct nodoMemoria* nodoEvaluado, uint32_t idProceso){
	for(uint32_t varH = 0; varH < sizeListaMutex(nodoEvaluado->mensaje->subsYaEnviado); varH ++){
		uint32_t* idActual = (uint32_t*) getListaMutex (nodoEvaluado->mensaje->subsYaEnviado, varH);

		if (*idActual == idProceso){
			return true;
		}
	}
	return false;
}

