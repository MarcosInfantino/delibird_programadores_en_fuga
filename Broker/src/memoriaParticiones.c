
#include "broker.h"
#include "memoria.h"
#include "memoriaParticiones.h"

//bool menorAMayorSegunTiempoCarga (void* part1, void* part2){
//	particion* parti1 = (particion*) part1;
//	particion* parti2 = (particion*) part2;
//	return (((parti1->tiempoDeCargaPart.tm_hour == parti2->tiempoDeCargaPart.tm_hour) &&
//			(parti1->tiempoDeCargaPart.tm_min < parti2->tiempoDeCargaPart.tm_min)) ||
//					((parti1->tiempoDeCargaPart.tm_hour == parti2->tiempoDeCargaPart.tm_hour) &&
//					(parti1->tiempoDeCargaPart.tm_min == parti2->tiempoDeCargaPart.tm_min) &&
//					(parti1->tiempoDeCargaPart.tm_sec < parti2->tiempoDeCargaPart.tm_sec)));
//}

bool menorAMayorSegunLru (void* part1, void* part2){
	particion* parti1 = (particion*) part1;
	particion* parti2 = (particion*) part2;
	return (parti1->lru.tm_hour < parti2->lru.tm_hour || (parti1->lru.tm_hour==parti2->lru.tm_hour && parti1->lru.tm_min<parti2->lru.tm_min));
}

particion* particionLibreALaIzquierda(particion* particionLibreNueva){
	for(int i = 0; i<sizeListaMutex(particionesLibres); i++){
		particion* partiActual = getListaMutex(particionesLibres, i);
		if(partiActual->offset + partiActual->sizeParticion == particionLibreNueva->offset){
			removeListaMutex(particionesLibres, i);
			return partiActual;
		}
	}
	return NULL;
}

bool hayParticionLibreALaDerecha(particion* particionLibreNueva){
	for(int i = 0; i<sizeListaMutex(particionesLibres); i++){
		particion* partiActual = getListaMutex(particionesLibres, i);
		//printf("PartiActual: base: %i, size: %i\n", partiActual->offset, partiActual->sizeParticion);
		if(partiActual->offset == (particionLibreNueva->offset + particionLibreNueva->sizeParticion)){
			//printf("Hay para consolidar a derecha\n");
			return true;
		}
	}
		return false;
}

particion* particionLibreALaDerecha(particion* particionLibreNueva){
	for(int i = 0; i<sizeListaMutex(particionesLibres); i++){
		particion* partiActual = getListaMutex(particionesLibres, i);
		//printf("EL size de las libre es: %i\n", sizeListaMutex(particionesLibres));
//		printf("La parti libre base: %i size: %i", particionLibreNueva->offset, particionLibreNueva->sizeParticion);
//		printf("La parti actual base: %i size: %i", partiActual->offset, partiActual->sizeParticion);
		if(partiActual->offset == (particionLibreNueva->offset + particionLibreNueva->sizeParticion)){
			removeListaMutex(particionesLibres, i);
			return partiActual;
		}
	}
	return NULL;
}

bool hayParticionLibreALaIzquierda(particion* particionLibre){
	for(int i = 0; i<sizeListaMutex(particionesLibres); i++){
		particion* partiActual = getListaMutex(particionesLibres, i);
		//printf("EL size de las libre es: %i\n", sizeListaMutex(particionesLibres));
		//printf("PartiActual: base: %i, size: %i\n", partiActual->offset, partiActual->sizeParticion);
		if(partiActual->offset + partiActual->sizeParticion == particionLibre->offset){
			//printf("Hay para consolidar a izquierda\n");
			return true;
		}
	}
	return false;
}

void consolidarSiSePuede(particion* particionLibre){
	//printf("Entro a consolidar\n");
	 if(hayParticionLibreALaDerecha(particionLibre)){
		 //printf("hay a la derecha\n");
		 particion* partAConsolidar = particionLibreALaDerecha(particionLibre);
		 particionLibre->sizeParticion = particionLibre->sizeParticion + partAConsolidar->sizeParticion;
		 destroyParticionLibre(partAConsolidar);
		 //printf("Mi particion consolidada comienza en: %i\n", particionLibre->offset);
	 }else if(hayParticionLibreALaIzquierda(particionLibre)){
		 //printf("hay a la izquierda\n");
		 particion* partAConsolidar = particionLibreALaIzquierda(particionLibre);
		 particionLibre->offset = partAConsolidar->offset;
		 particionLibre->sizeParticion = particionLibre->sizeParticion + partAConsolidar->sizeParticion;
		// printf("Como queda despues de consolidar: base: %i, size: %i\n", particionLibre->offset, particionLibre->sizeParticion);
		 destroyParticionLibre(partAConsolidar);
	 }
	 //printf("No se puede consolidar\n");
}

void eliminarParticion (particion* part){
	particion* partiNueva = malloc(sizeof(particion));
	partiNueva->offset = part->offset;
	partiNueva->sizeParticion = part->sizeParticion;
	partiNueva->estadoParticion = PARTICION_LIBRE;
	addListaMutex(particionesLibres, (void*)partiNueva);
	log_info(loggerBroker, "Elimino particion que comienza en: %i", part->offset);
	//printf("Elimino particion base: %i, size: %i\n", partiNueva->offset, partiNueva->sizeParticion);
	removeAndDestroyElementListaMutex(particionesOcupadas, 0, destroyParticionOcupada);
	consolidarSiSePuede(partiNueva);
}

bool menorAMayorSegunTC(void* part1, void* part2){
	particion* parti1 = (particion*) part1;
	particion* parti2 = (particion*) part2;
	return parti1->tiempoDeCargaPart<parti2->tiempoDeCargaPart;
}

void elegirParticionVictimaYEliminarla(){
	particion* particionVictima;
	switch(algoritmoReemplazo){
	case FIFO:
		//list_sort_Mutex(particionesOcupadas, menorAMayorSegunTiempoCarga);
		list_sort_Mutex(particionesOcupadas, menorAMayorSegunTC);
		particionVictima = (particion*)getListaMutex(particionesOcupadas, 0);
		break;
	case LRU:
		list_sort_Mutex(particionesLibres, menorAMayorSegunLru);
		particionVictima = (particion*)getListaMutex(particionesOcupadas, 0);
		break;
	}
	eliminarParticion(particionVictima);
}

bool sePuedeCompactar(){
	if(frecuenciaCompactacion==-1){
		return (sizeListaMutex(particionesOcupadas)==0);
	}else if(frecuenciaCompactacion>=0){
		return cantidadBusquedasFallidas >= frecuenciaCompactacion;
	}
	return NULL;
}

void registrarEnParticiones(msgMemoriaBroker* mensajeNuevo){
	particion* particionLibre;
	particionLibre = obtenerParticionLibrePARTICIONES(mensajeNuevo->sizeStream);
	if(particionLibre == NULL){
		cantidadBusquedasFallidas++;
		//printf("Cantidad de busquedas fallidas: %i\n", cantidadBusquedasFallidas);
		if(sePuedeCompactar()){
			compactar();
			particionLibre = obtenerParticionLibrePARTICIONES(mensajeNuevo->sizeStream);
			if(particionLibre == NULL){
				cantidadBusquedasFallidas++;
				elegirParticionVictimaYEliminarla();
				registrarEnParticiones (mensajeNuevo);
			}
		}else{
			elegirParticionVictimaYEliminarla();
			registrarEnParticiones(mensajeNuevo);
		}
	}

	if(particionLibre != NULL){
		asignarMensajeAParticion(particionLibre, mensajeNuevo);
	}

	//printf("Asigno mensaje a particion\n");
}

void asignarMensajeAParticion(particion* partiLibre, msgMemoriaBroker* mensaje){
	//printf("Entro a asignar particion: base: %i, size: %i\n", partiLibre->offset, partiLibre->sizeParticion);
	//particion* partiOcupada = inicializarParticion();
	particion* partiOcupada = malloc(sizeof(particion));
	partiOcupada->offset = partiLibre->offset;
	partiOcupada->mensaje = mensaje;
	if(mensaje->sizeStream > particionMinima){
		partiOcupada->sizeParticion = mensaje->sizeStream;
	}else{
		partiOcupada->sizeParticion = particionMinima;
	}
	partiOcupada->estadoParticion = PARTICION_OCUPADA;
//	time_t t;
//	t=time(NULL);
//	partiOcupada->lru = *localtime(&t);
//	partiOcupada->tiempoDeCargaPart = *localtime(&t);
	partiOcupada->tiempoDeCargaPart=TC;
	TC++;
	memcpy(memoria + partiOcupada->offset, mensaje->stream, mensaje->sizeStream);

	mensaje->stream = memoria + partiOcupada->offset;

	addListaMutex(particionesOcupadas, (void*)partiOcupada);
	log_info(loggerBroker, "Almaceno mensaje en partición que comienza en: %i", partiOcupada->offset);
	//printf("Ocupada generada: base: %i, size: %i\n", partiOcupada->offset, partiOcupada->sizeParticion);
	if(partiLibre->sizeParticion > partiOcupada->sizeParticion){
		partiLibre->offset += partiOcupada->sizeParticion;
		partiLibre->sizeParticion -= partiOcupada->sizeParticion;
		partiLibre->estadoParticion = PARTICION_LIBRE;
		//printf("Parti libre generada: base: %i, size: %i\n", partiLibre->offset, partiLibre->sizeParticion);
		addListaMutex(particionesLibres, (void*)partiLibre);
	}else{ //osea que el tamaño es igual
		destroyParticionLibre (partiLibre);
	}
}
//particion* inicializarParticion(){//ESTA FUNCION ES AL PEDO
//	particion* particionADevolver = malloc(sizeof(particion));
//	particionADevolver->mensaje = malloc (sizeof(msgMemoriaBroker));
//	return particionADevolver;
//}

particion* crearPrimeraParticionLibre(void){
	particion* particionADevolver = malloc(sizeof(particion));
	particionADevolver->offset = 0;
	particionADevolver->sizeParticion = tamMemoria;
	particionADevolver->estadoParticion = PARTICION_LIBRE;
	return particionADevolver;
}

uint32_t cantidadMemoriaLibre (){
	uint32_t memoriaLibre=0;
	for(int i = 0; i<sizeListaMutex(particionesLibres); i++){
		particion* parti = (particion*)getListaMutex(particionesLibres,i);
		memoriaLibre += parti->sizeParticion;
	}
	return memoriaLibre;
}

bool noHayNingunaSuficientementeGrande(uint32_t tamStream){

	for(int i =0; i<sizeListaMutex(particionesLibres); i++){
		particion* parti = (particion*)getListaMutex(particionesLibres, i);
		if(parti->sizeParticion>=tamStream){
			return false;
		}
	}
	return true;
}

particion* obtenerParticionLibrePARTICIONES(uint32_t tamStream){
	if(noHayNingunaSuficientementeGrande(tamStream) || cantidadMemoriaLibre()<tamStream){

		return NULL;
	}
	//printf("Entro a ver si hay libres\n");
	//printf("El size es: %i\n", sizeListaMutex(particionesLibres));
	if (algoritmoParticionLibre == FIRST_FIT){
		if(sizeListaMutex(particionesLibres)>1){
			list_sort_Mutex(particionesLibres, menorAmayorSegunOffset);
			//printf("Ordeno lista\n");
		}
		particion* pSeleccionadaFIRST = (particion*)list_remove_by_condition_Mutex(particionesLibres, esSuficientementeGrandeParaElMSG);
		//printf("FIRST FIT offset %i\n", pSeleccionadaFIRST->offset);
		return pSeleccionadaFIRST;
	}else if(algoritmoParticionLibre == BEST_FIT){
		list_sort_Mutex(particionesLibres, menorAmayorSegunSize);
		particion* pSeleccionadaBEST = (particion*)list_remove_by_condition_Mutex(particionesLibres, esSuficientementeGrandeParaElMSG);
		//printf("BEST FIT Selecciono %i , cuyo estado es: %i \n", pSeleccionadaBEST->offset, pSeleccionadaBEST->estadoParticion);
		return pSeleccionadaBEST;
	}
	return NULL;
}

void compactar(){
	//printf("Hay que compactar\n");
	particion* elemento;
	uint32_t base = 0;
	list_sort_Mutex(particionesOcupadas, menorAmayorSegunOffset);
	for(int i=0; i<sizeListaMutex(particionesOcupadas); i++){
		elemento = (particion*)getListaMutex(particionesOcupadas, i);

		void* aux= malloc(elemento->sizeParticion);
		memcpy(aux, memoria + elemento->offset, elemento->sizeParticion);

//		memcpy(memoria + base, memoria + elemento->offset, elemento->sizeParticion);
		memcpy(memoria + base, aux, elemento->sizeParticion);

		elemento->mensaje->stream=memoria + base;//IMPORTANTISIMO
		free(aux);

		elemento->offset = base;
		base += elemento->sizeParticion;
		//printf("La particion ocupada tiene base: %i y size: %i\n", elemento->offset, elemento->sizeParticion);
	}
	log_info(loggerBroker, "Se realiza la compactacion");
	generarParticionLibre(base);
	cantidadBusquedasFallidas = 0;
}

void generarParticionLibre(uint32_t base){
	particion* nuevaParticion = malloc(sizeof(particion));
	nuevaParticion->offset = base;
	nuevaParticion->sizeParticion = tamMemoria - base;
	nuevaParticion->estadoParticion = PARTICION_LIBRE;
	//printf("La particion libre generada es: base: %i, size: %i\n", nuevaParticion->offset,nuevaParticion->sizeParticion);
	uint32_t size = sizeListaMutex(particionesLibres);
	for(int j=0; j<size; j++){
		removeAndDestroyElementListaMutex(particionesLibres, 0, destroyParticionLibre);
	}
	addListaMutex(particionesLibres,(void*) nuevaParticion);
}

void destroyParticionLibre(void* parti){
	particion* part = (particion*)parti;
	free(part);
}

void destroyParticionOcupada (void* part){
	particion* parti = (particion*) part;
	free(parti->mensaje); //ver si destruir las listas o no
	free(parti);
}

bool menorAmayorSegunOffset(void* primero, void* segundo){
	return ((particion*)primero)->offset < ((particion*)segundo)->offset;
}

bool menorAmayorSegunSize(void* primero, void* segundo){
	return ((particion*)primero)->sizeParticion < ((particion*)segundo)->sizeParticion;
}

bool esSuficientementeGrandeParaElMSG(void* elemento){
	particion* partLibre = (particion*)elemento;
	return partLibre->sizeParticion >= auxTamanioStreamGlobal;
}

t_list* buscarMensajesDeColaEnParticiones (uint32_t cola){
	particion* particion1;
	t_list* msjsDeCola = list_create();
	for(int i = 0; i<sizeListaMutex(particionesOcupadas); i++){
		particion1 = (particion*) getListaMutex(particionesOcupadas, i);
		if(particion1->mensaje->cola==cola){
			list_add(msjsDeCola, (void*)particion1->mensaje);
		}
	}
	return msjsDeCola;
}

void enviarMsjsASuscriptorNuevoParticiones (uint32_t cola, uint32_t socket, uint32_t idProceso){
	t_list* listMsjs = buscarMensajesDeColaEnParticiones(cola);
	msgMemoriaBroker* msg;
	paquete* paqueteASerializar;
	void* paqueteSerializado;
	for(int i = 0; i<list_size(listMsjs); i++){
		msg = (msgMemoriaBroker*) list_get(listMsjs, i);
		paqueteASerializar = llenarPaquete(msg->modulo, msg->cola, msg->sizeStream, msg->stream);
		insertarIdPaquete(paqueteASerializar, msg->idMensaje);
		insertarIdCorrelativoPaquete(paqueteASerializar, msg->idCorrelativo);
		paqueteSerializado = serializarPaquete(paqueteASerializar);
		send(socket, paqueteSerializado, sizePaquete(paqueteASerializar),0);

		//memory leak
		//addListaMutex(msg->subsYaEnviado, (void*)socket);
		guardarYaEnviados(paqueteASerializar, idProceso);
	}
	list_destroy(listMsjs);
}

msgMemoriaBroker*  buscarMensajeEnMemoriaParticiones(uint32_t idMensajeBuscado){
	particion* particion1;
	for(int i = 0; i<sizeListaMutex(particionesOcupadas); i++){
		particion1 = (particion*) getListaMutex (particionesOcupadas, i);
		if(particion1->mensaje->idMensaje == idMensajeBuscado){
			return particion1->mensaje;
		}
	}
	return NULL;
}
