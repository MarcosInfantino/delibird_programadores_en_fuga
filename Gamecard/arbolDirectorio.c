/*
 * arbolDirectorio.c
 *
 *  Created on: 5 jul. 2020
 *      Author: utnso
 */
#include "gamecard.h"

struct nodoArbolDirectorio* crearNodoDirectorio(archivoHeader* archivo, struct nodoArbolDirectorio* nodoDirectorioPadre){
	struct nodoArbolDirectorio* nodo=malloc(sizeof(struct nodoArbolDirectorio));
	nodo->dataNodo=archivo;
	nodo->nodoPadre=nodoDirectorioPadre;
	nodo->nodosHijos=list_create();
	if(nodoDirectorioPadre!=NULL){
		list_add(nodoDirectorioPadre->nodosHijos,(void*)nodo);// si tengo un padre le digo que soy su hijo
	}
	return nodo;
}

