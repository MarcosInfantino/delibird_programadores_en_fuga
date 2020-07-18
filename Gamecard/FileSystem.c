/*
 * FileSystem.c
 *
 *  Created on: 13 jun. 2020
 *      Author: utnso
 */

#include "gamecard.h"

struct stat bufferEstado;

char* buscarPath (char *path){
	char* direccion = string_new();
	string_append(&direccion, puntoMontaje);
	string_append(&direccion, path);
	return direccion;
}

bool archivoExiste(char* path) {
	if(access(path, F_OK) != -1) {
		//printf("Se encontro el archivo: \"%s\" \n", path);
		return true;
	} else {
		//printf("No se encontro el archivo: \"%s\" \n",path);
		return false;
	}
}

int32_t existe(char *path){
  int32_t fd=open(path, O_RDONLY);
  if (fd<0)
    return (errno==ENOENT)?-1:-2;
  close(fd);
  return 0;
}

uint32_t directorioExiste (char *path){
  int descriptor = open(path, O_RDONLY);
  if (descriptor<0){
    return -1;
  } else{
  close(descriptor);
  return 1;
  	  }
  return -1;
}

FILE* verificarApertura(archivoHeader* archivo){
	while(estaAbierto(archivo)){
		sleep(tiempoReintentoOperacion);
		log_info(gamecardLogger2, "Reintento abrir el archivo en %i sengundos. ", tiempoReintentoOperacion);
	}
	return abrirArchivo(archivo);
}

void iniciarMetadata() {
	if(directorioExiste(buscarPath("/Metadata"))<0){
		log_info(gamecardLogger,"No se encontro el directorio metadata");
		//log_info(gamecardLogger2,"No se encontro el directorio metadata");
	}else{
		log_info(gamecardLogger,"Se encontro el directorio metadata");
		//log_info(gamecardLogger2,"Se encontro el directorio metadata");
		if(!archivoExiste("/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/Metadata/metadata.bin")){
			log_info(gamecardLogger,"No se encontro el archivo metadata.bin");
		}
		else{
			log_info(gamecardLogger,"Se encontro el archivo metadata.bin");
			t_config* config_metadata = config_create("/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/Metadata/metadata.bin");
			tallGrass.block_size  = config_get_int_value(config_metadata, "BLOCK_SIZE");
			tallGrass.blocks = config_get_int_value(config_metadata, "BLOCKS");
			tallGrass.magic_number = config_get_string_value(config_metadata, "MAGIC_NUMBER");
			log_info(gamecardLogger,"Inicio de Metadata, Tamanio bloques: %i,  Cantidad de bloques: %i,  Magic number: %s", tallGrass.block_size, tallGrass.blocks, tallGrass.magic_number);
			config_destroy(config_metadata);
		}
	}

}

/*
void iniciarBitmap() {
	//FILE* archivoBitmap = fopen(buscarPath("Metadata/bitmap.bin"),"r");
	FILE* archivoBitmap = fopen("TALL_GRASS/Metadata/bitmap.bin","r");
	uint32_t tamanioActual;
	fseek(archivoBitmap, 0L , SEEK_END);
	tamanioActual = ftell(archivoBitmap);
	printf("El tamaño actual es de: %i\n", tamanioActual);
	fseek(archivoBitmap, 0L, SEEK_SET);
	char* bufferBitmap = malloc(tamanioActual);
	fread(bufferBitmap, tamanioActual, 1, archivoBitmap);
	bufferBitmap = string_substring_until(bufferBitmap, tamanioActual);
	if(bufferBitmap == NULL){
		//Esta vacio
	}
	bitmap = bitarray_create(bufferBitmap, tallGrass.blocks);
	uint32_t tope = bitarray_get_max_bit(bitmap);
	for(int i=0;i<tope;i++){
		bitarray_clean_bit(bitmap,i);
	}


	//free(buffer);
	fclose(archivoBitmap);
}
*/



void iniciarBitmap() {
//	char* pathBitmap = "/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/Metadata/bitmap.bin";
	char* pathBitmap = string_new();
	string_append(&pathBitmap, "%s/Metadata/bitmap.bin", puntoMontaje);
	if(!archivoExiste(pathBitmap)){
		FILE* archivoBitmap = fopen(pathBitmap,"w+b");
			fseek(archivoBitmap,0, SEEK_SET);
			for(uint32_t i=0;i<tallGrass.blocks;i++){
				fputs("0",archivoBitmap);
			}
			fclose(archivoBitmap);
	}


	//uint32_t fd = open(string_from_format("%s/Blocks/%d.bin", puntoMontaje, bloque->id),O_RDWR|O_CREAT,0777);


	//uint32_t descriptor = open(pathBitmap, O_RDWR , S_IRUSR | S_IWUSR);
	uint32_t descriptor = open(pathBitmap, O_RDWR , S_IRUSR | S_IWUSR | O_RDWR|O_CREAT, 0777);
	ftruncate(descriptor,tallGrass.blocks);
	fsync(descriptor);
	mmapBitmap = (char*) mmap(NULL, tallGrass.blocks, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);


	close(descriptor);

	 //bitmap = bitarray_create_with_mode((char*) mmapBitmap, tallGrass.blocks, LSB_FIRST);

//	 for(uint32_t i=0;i<tallGrass.blocks;i++){
//	 	bitarray_clean_bit(bitmap,i);
//	 }

	//bitmap = bitarray_create_(mmapBitmap, sb.st_size);
//	for(uint32_t i = 0; i < tallGrass.blocks; i++){
//			 bitarray_clean_bit(bitmap, i);
//		}
	//bitarray_set_bit(bitmap, 0);
	//msync(mmapBitmap,tallGrass.blocks,MS_ASYNC);

}


int32_t crearArchivoBloque(blockHeader* bloque) {

	//FILE* archivoBloque = fopen(string_from_format("%s/Blocks/%d.bin", puntoMontaje, bloque->id), "w");
	char* pathArchivoBloque=string_from_format("%s/Blocks/%d.bin", puntoMontaje, bloque->id);
	if(!archivoExiste(pathArchivoBloque)){
		uint32_t fd = open(pathArchivoBloque,O_RDWR|O_CREAT,0777);
		ftruncate(fd,tallGrass.block_size);
		close(fd);
	}
	free(pathArchivoBloque);

		//ocuparBloque(bloque->id);
	return 0;
}

archivoHeader* crearDirectorio(char* nombre, char* pathDestino, uint32_t tipo){

	int32_t status;
	errno = 0;
	char* direc = string_new();
	string_append(&direc,pathDestino);
	string_append(&direc,nombre);

	status = mkdir(direc, 0755);
	if(status != 0){
		if(errno == EEXIST){
			//log_info(gamecardLogger2,"El directorio ya existe");
			//Ver que hacer aca
			//si el directorio ya existe devuelve null
		}
		if(status < 0){
			//log_info(gamecardLogger2,"Error al crear el directorio");
			//return -1;
		}
	}
	if(strcmp("Blocks", nombre)==0){

		return NULL;
	}else{
		return crearMetadata(nombre,tipo, direc);
	}

	//return crearNodoDirectorio(header, directorioPadre);



}

archivoHeader* obtenerArchivoPokemon(char* nombre){
	return crearDirectorio(nombre, pathFiles, ARCHIVO);
}

void eliminarDirectorio(char* path){
	rmdir(path);
}

archivoHeader* crearMetadata(char* nombre, uint32_t tipo, char* direccion){
	archivoHeader* metadataFile;
	char* nuevaDirec = malloc(strlen(direccion)+strlen("metadata.bin")+1);
	strcpy(nuevaDirec,direccion);
	string_append(&nuevaDirec,"/metadata.bin");




	if(!archivoHeaderYaRegistrado(nombre) && !archivoExiste(nuevaDirec)){
	 metadataFile= malloc(sizeof(archivoHeader));

	metadataFile->pathArchivo = malloc(strlen(nuevaDirec)+1);
	strcpy(metadataFile->pathArchivo,nuevaDirec);

	FILE *archivoMetadata=fopen(nuevaDirec,"w+b");

	metadataFile->nombreArchivo=malloc(strlen(nombre)+1);
	strcpy(metadataFile->nombreArchivo, nombre);

	switch(tipo){
	case DIRECTORIO:;
		//log_info(gamecardLogger2,"Soy directorio");
		metadataFile->esDirectorio = 'Y';
		metadataFile->estaAbierto = false;
		metadataFile->bloquesUsados = NULL;
		metadataFile->tamanioArchivo = 0;
		metadataFile->tipo = DIRECTORIO;
		escribirMetadata(metadataFile);
		metadataFile->mutex=malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(metadataFile->mutex,NULL);
		//fwrite(&metadataFile,sizeof(archivoHeader),1, metadataArchivo);

		break;
	case ARCHIVO:;
		//log_info(gamecardLogger2,"Soy archivo");
		metadataFile->esDirectorio = 'N';
		metadataFile->estaAbierto = false;
		metadataFile->bloquesUsados = list_create();
		metadataFile->tamanioArchivo = 0;
		metadataFile->tipo = ARCHIVO;
		metadataFile->mutex=malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(metadataFile->mutex,NULL);
		escribirMetadata(metadataFile);
		break;
	default:; log_info(gamecardLogger2,"Manqueada");break;
	}


	fclose(archivoMetadata);
	if(tipo==ARCHIVO){
		addListaMutex(listaArchivos, (void*)metadataFile);
	}
	}else if(archivoExiste(nuevaDirec) && archivoHeaderYaRegistrado(nombre)){
		free(nuevaDirec);
		metadataFile=buscarArchivoHeaderPokemon(nombre);
	}else{

		metadataFile= cargarMetadata(nuevaDirec,tipo, nombre);
		addListaMutex(listaArchivos, (void*)metadataFile);
	}

	return metadataFile;

}

archivoHeader* cargarMetadata(char* path, uint32_t tipo, char* nombre){

	archivoHeader* archivo=malloc(sizeof(archivoHeader));
	archivo->nombreArchivo=malloc(strlen(nombre)+1);
	strcpy(archivo->nombreArchivo, nombre);
	t_config* configLoco=config_create(path);
	archivo->pathArchivo=path;
		switch(tipo){
		case DIRECTORIO:;

			//fputs("DIRECTORY=Y\n",archivoMetadata);
			archivo->esDirectorio=*(config_get_string_value(configLoco, "DIRECTORY"));
			archivo->mutex=malloc(sizeof(pthread_mutex_t));
			pthread_mutex_init(archivo->mutex,NULL);
			archivo->tipo=DIRECTORIO;
			break;
		case ARCHIVO:;

	//		fputs("DIRECTORY=N\n", archivoMetadata);
	//		fputs("SIZE=0\n", archivoMetadata);
	//		fputs("BLOCKS=[]\n",archivoMetadata);
	//		fputs("OPEN=N\n", archivoMetadata);
			//log_info(gamecardLogger2, "entro a cargar metadata");
			archivo->esDirectorio=*(config_get_string_value(configLoco, "DIRECTORY"));
			archivo->tamanioArchivo=config_get_int_value(configLoco, "SIZE");
			log_info(gamecardLogger2, "empiezo con los bloques %s", nombre);
			char** listaBloques=config_get_array_value(configLoco, "BLOCKS");
			archivo->bloquesUsados=arrayBloquesStringToList(listaBloques);
			log_info(gamecardLogger2, "termino con los bloques %s", nombre);
			archivo->estaAbierto=false;

			archivo->mutex=malloc(sizeof(pthread_mutex_t));
			pthread_mutex_init(archivo->mutex,NULL);
			archivo->tipo=ARCHIVO;
			liberarArrayBidimensionalChar(listaBloques);
			break;

		}

		config_destroy(configLoco);
		return archivo;
}

void actualizarArchivoBitmap() {
	FILE* archivoBitmap = fopen(buscarPath("Metadata/bitmap.bin"),"w");
	fputs(string_substring(bitmap->bitarray, 0, tallGrass.blocks / 8), archivoBitmap);
	fclose(archivoBitmap);
}


void inicializarListaBloques(){
	listaBloques=list_create();
	for(uint32_t i=1;i<=tallGrass.blocks;i++){
		blockHeader* bloqueActual=malloc(sizeof(blockHeader));
		bloqueActual->file=NULL;
		bloqueActual->bytesLeft=tallGrass.block_size;
		bloqueActual->id=i;
		bloqueActual->pos=0;
		list_add(listaBloques,(void*) bloqueActual);
		crearArchivoBloque(obtenerBloquePorId(i));//obtenerBloquePorId(1)
	}
}

bool poseeArchivo(blockHeader* bloque){
	return bloque->file!=NULL;
}

//bool estaLibre(uint32_t idBloque){
//	return !bitarray_test_bit(bitmap, idBloque-1);
//}
//
//void ocuparBloque(uint32_t idBloque){
//	bitarray_set_bit(bitmap,idBloque-1);
//}
//
//void liberarBloque(uint32_t idBloque){
//	bitarray_clean_bit(bitmap,idBloque-1);
//}

bool estaLibre(uint32_t idBloque){
	return *(mmapBitmap + idBloque-1)=='0';
}

void ocuparBloque(uint32_t idBloque){
	*(mmapBitmap + idBloque-1)='1';
}

void liberarBloque(uint32_t idBloque){
	*(mmapBitmap + idBloque-1)='0';
}

blockHeader* obtenerBloquePorId(uint32_t id){
	return (blockHeader*) list_get(listaBloques, id-1);
}

blockHeader* encontrarBloqueLibre(){// devuelve el bloque ya ocupado
	for(uint32_t i=0; i<list_size(listaBloques);i++){
		blockHeader* bloqueActual= list_get(listaBloques,i);
		if(estaLibre(bloqueActual->id)){
			ocuparBloque(bloqueActual->id);
			return bloqueActual;
		}
	}
	return NULL;
}

void escribirMetadata(archivoHeader* metadata){
	t_config* configLoco=config_create(metadata->pathArchivo);
	switch(metadata->tipo){
	case DIRECTORIO:;

		//fputs("DIRECTORY=Y\n",archivoMetadata);


		config_set_value(configLoco, "DIRECTORY", "Y");
		config_save(configLoco);

		break;
	case ARCHIVO:;

//		fputs("DIRECTORY=N\n", archivoMetadata);
//		fputs("SIZE=0\n", archivoMetadata);
//		fputs("BLOCKS=[]\n",archivoMetadata);
//		fputs("OPEN=N\n", archivoMetadata);

		config_set_value(configLoco, "DIRECTORY", "N");
		config_set_value(configLoco, "SIZE", "0");
		config_set_value(configLoco, "BLOCKS", "[]");
		config_set_value(configLoco, "OPEN", "N");
		config_save(configLoco);
		break;

	}
	config_destroy(configLoco);
	//free(metadata);

}
/*
void escribirBloque2(int32_t bloque, char* buffer){
	FILE* block = fopen("/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/Blocks/1.bin","wb+");
	fseek(block, 0, SEEK_SET);
	uint32_t resultado=fputs(buffer,block);
	printf("%i\n",resultado);
	fclose(block);
}
*/
//1-1=9\n\n
int32_t escribirBloque(int32_t bloque, int32_t offset, int32_t longitud, char* buffer) {
	blockHeader* headerBloque=obtenerBloquePorId(bloque);
	if(tieneCapacidad(headerBloque,longitud)){
	//disminuirCapacidad(headerBloque, longitud);
	//log_info(gamecardLogger2, "Escribiendo bloque:%d, offset=%d, longitud=%d, buffer=%s", bloque, offset, longitud, buffer);
	//const char* bufferin = buffer;
	//FILE* block = fopen(string_from_format("%s%d.bin", pathBlocks, bloque), "w+b");
	//FILE* block = fopen(string_from_format("%s1.bin", pathBlocks), "r+");
	char* path=pathBloque(bloque);
	FILE* block = fopen(path,"r+");
	fseek(block, offset, SEEK_SET);
	//fwrite(buffer, strlen(buffer)+1, 1, block);
	//fwrite(buffer, strlen(buffer), 1, block);

	fwrite(buffer, longitud, 1, block);
	//log_info(gamecardLogger2,"RESULTADO DEL FWRITE:%i",resultado);
	//fputs(buffer,block);
	headerBloque->pos=ftell(block);
	fclose(block);
	free(path);
	return 0;
	}else{
		return -1;
	}
}



archivoHeader* buscarArchivoHeaderPokemon(char* pokemon){
	for(uint32_t i=0; i< sizeListaMutex(listaArchivos);i++){
		archivoHeader* actual= (archivoHeader*) getListaMutex(listaArchivos,i);
		if(strcmp(actual->nombreArchivo,pokemon)==0){
			return actual;
		}
	}

	return NULL;

}

bool archivoHeaderYaRegistrado(char* pokemon){
	return buscarArchivoHeaderPokemon(pokemon)!=NULL;
}

