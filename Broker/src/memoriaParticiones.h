/*
 * memoriaParticiones.h
 *
 *  Created on: 9 jul. 2020
 *      Author: utnso
 */

#ifndef MEMORIAPARTICIONES_H_
#define MEMORIAPARTICIONES_H_

#include "broker.h"
#include "memoria.h"

uint32_t TiempoCarga;

typedef enum{
	PARTICION_LIBRE,
	PARTICION_OCUPADA
}estadoParticion;

typedef struct{
	uint32_t offset;
	uint32_t sizeParticion;
	char* lru;
	uint32_t tiempoDeCargaPart;
	msgMemoriaBroker* mensaje;
	uint32_t estadoParticion;
}particion;

particion* crearPrimeraParticionLibre();
void registrarEnParticiones(msgMemoriaBroker*);
particion* obtenerParticionLibrePARTICIONES(uint32_t tamStream);
void asignarMensajeAParticion(particion* partiLibre, msgMemoriaBroker* mensaje);

bool menorAmayorSegunSize(void* primero, void* segundo);
bool menorAmayorSegunOffset(void* primero, void* segundo);
bool esSuficientementeGrandeParaElMSG(void* elemento);
bool menorAMayorSegunTiempoCarga (void* part1, void* part2);
bool compararTiempoLru(char* time1, char* time2);
bool menorAMayorSegunLru (void* part1, void* part2);
bool noHayNingunaSuficientementeGrande(uint32_t tamStream);

uint32_t cantidadMemoriaLibre ();

void generarParticionLibre(uint32_t base);
bool sePuedeCompactar();
void compactar();

void eliminarParticion (particion* particion);
void elegirParticionVictimaYEliminarla();
void destroyParticionOcupada (void* particion);
void destroyParticionLibre (void* particion);

t_list* buscarMensajesDeColaEnParticiones (uint32_t cola);
void enviarMsjsASuscriptorNuevoParticiones (uint32_t cola, uint32_t socket, uint32_t idProceso);
msgMemoriaBroker*  buscarMensajeEnMemoriaParticiones(uint32_t idMensajeBuscado);

bool hayParticionLibreALaIzquierda(particion* particionLibre);
particion* particionLibreALaIzquierda(particion* particionLibreNueva);
bool hayParticionLibreALaDerecha(particion* particionLibreNueva);
particion* particionLibreALaDerecha(particion* particionLibreNueva);
void consolidarSiSePuede(particion* particionLibre);

#endif /* MEMORIAPARTICIONES_H_ */
