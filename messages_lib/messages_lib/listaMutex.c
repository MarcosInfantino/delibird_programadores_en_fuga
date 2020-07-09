/*
 * listaMutex.c
 *
 *  Created on: 31 may. 2020
 *      Author: utnso
 */

#include "messages_lib.h"

listaMutex* inicializarListaMutex(){
	listaMutex* list=malloc(sizeof(listaMutex));
	(*list).lista=list_create();
	(*list).mutex=malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((*list).mutex,NULL);
	return list;
}

void addListaMutex(listaMutex* list,void* elemento){
	pthread_mutex_lock(list->mutex);
	list_add((*list).lista,elemento);
	pthread_mutex_unlock(list->mutex);
}

void* getListaMutex(listaMutex* list, uint32_t index){
	pthread_mutex_lock(list->mutex);
	void* value=list_get(list->lista,index);
	pthread_mutex_unlock(list->mutex);
	return value;
}

uint32_t sizeListaMutex(listaMutex* list){
	pthread_mutex_lock(list->mutex);
	uint32_t size=list_size(list->lista);
	pthread_mutex_unlock(list->mutex);
	return size;
}

void* removeListaMutex(listaMutex* list,uint32_t pos){
	pthread_mutex_lock(list->mutex);
	void* retorno =list_remove(list->lista,pos);
	pthread_mutex_unlock(list->mutex);
	return retorno;
}

void* list_remove_by_condition_Mutex(listaMutex* list, bool(*condition)(void*)){
	pthread_mutex_lock(list->mutex);
	void* retorno = list_remove_by_condition(list->lista, condition);
	pthread_mutex_unlock(list->mutex);

	return retorno;
}

void list_sort_Mutex(listaMutex* list, bool (*comparator)(void *, void *)){
	pthread_mutex_lock(list->mutex);
	list_sort(list->lista, comparator);
	pthread_mutex_unlock(list->mutex);
}

void removeAndDestroyElementListaMutex(listaMutex* list,uint32_t pos,void(*element_destroyer)(void*)){
	pthread_mutex_lock(list->mutex);
	list_remove_and_destroy_element(list->lista,pos, element_destroyer);
	pthread_mutex_unlock(list->mutex);
}

void destruirListaMutexYElementos(listaMutex* lista,void(*element_destroyer)(void*)){
	free(lista->mutex);
	list_destroy_and_destroy_elements(lista->lista, element_destroyer);
}

void destruirListaMutex(listaMutex* lista){
	free(lista->mutex);
	list_destroy(lista->lista);
}

listaMutex* convertirAListaMutex(t_list* lista){
	listaMutex* list=inicializarListaMutex();
	list_destroy(list->lista);
	list->lista=lista;
	return list;
}

void listAddAllMutex(t_list* list, listaMutex* list1){
	pthread_mutex_lock(list1->mutex);
	list_add_all(list,list1->lista);
	pthread_mutex_unlock(list1->mutex);
}
