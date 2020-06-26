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

uint32_t archivoExiste(char* path) {
	if(access(path, F_OK) != -1) {
		//printf("Se encontro el archivo: \"%s\" \n", path);
		return 1;
	} else {
		//printf("No se encontro el archivo: \"%s\" \n",path);
		return -1;
	}
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

//void iniciarFileSystem(){
//
//	//iniciarMetadata();
//	//iniciarBitmap();
//
//	//crear cliente
//}


void iniciarMetadata() {
	if(directorioExiste(buscarPath("/Metadata"))<0){
		printf("No se encontro el directorio metadata\n");
	}else{
		printf("Se encontro el directorio metadata\n");
		if(archivoExiste(buscarPath("/Metadata/Metadata.bin"))<0){
			printf("No se encontro el archivo metadata.bin\n");
		}
		else{
			printf("Se encontro el archivo metadata.bin\n");
			//char* pathMetadata = buscarPath("/Metadata/Metadata.bin");
		//	int descriptorFichero = open(pathMetadata,O_RDONLY);
		//	fstat(descriptorFichero,&bufferEstado);
		//	char* memoria = mmap(NULL,bufferEstado.st_size,PROT_READ,MAP_PRIVATE,descriptorFichero,0);

			t_config* config_metadata = config_create("/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/Metadata/metadata.bin");
			tallGrass.block_size  = config_get_int_value(config_metadata, "BLOCK_SIZE");
			tallGrass.blocks = config_get_int_value(config_metadata, "BLOCKS");
			tallGrass.magic_number = config_get_string_value(config_metadata, "MAGIC_NUMBER");
			printf("Inicio de Metadata\n Tamanio bloques: %i\n Cantidad de bloques: %i\n Magic number: %s\n", tallGrass.block_size, tallGrass.blocks, tallGrass.magic_number);
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
	char* pathBitmap = "/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/Metadata/bitmap.bin";

	FILE* archivoBitmap = fopen(pathBitmap,"w+b");
	fseek(archivoBitmap,0, SEEK_SET);
	for(uint32_t i=0;i<tallGrass.blocks;i++){
		fputs("0",archivoBitmap);
	}
	fclose(archivoBitmap);

	uint32_t descriptor = open(pathBitmap, O_RDWR , S_IRUSR | S_IWUSR);
	fsync(descriptor);
	mmapBitmap = mmap(NULL, tallGrass.blocks, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);


	close(descriptor);

	 bitmap = bitarray_create_with_mode((char*) mmapBitmap, tallGrass.blocks, LSB_FIRST);
	//bitmap = bitarray_create_(mmapBitmap, sb.st_size);
//	for(uint32_t i = 0; i < tallGrass.blocks; i++){
//			 bitarray_clean_bit(bitmap, i);
//		}
	//bitarray_set_bit(bitmap, 0);
	//msync(mmapBitmap,tallGrass.blocks,MS_ASYNC);

}


int32_t crearArchivoBloque(blockHeader* bloque) {

		FILE* archivoBloque = fopen(string_from_format("%s/Blocks/%d.bin", puntoMontaje, bloque->id), "w");

		if (archivoBloque == NULL){
				return -1;
		}
		fclose(archivoBloque);
		ocuparBloque(bloque->id);


	return 0;
}

int32_t crearDirectorio(char* nombre, char* pathDestino){
	int32_t status;
	errno = 0;
	char* direc = string_new();
	string_append(&direc,pathDestino);
	string_append(&direc,nombre);

	status = mkdir(direc, 0755);
	if(status != 0){
		if(errno == EEXIST){
			printf("El directorio ya existe\n");
			//Ver que hacer aca
			return 0;
		}
		if(status < 0){
			printf("Error al crear el directorio\n");
			return -1;
		}
	}
	crearMetadata(ARCHIVO, pathDestino);
	return status;
}

void eliminarDirectorio(char* path){
	rmdir(path);
}

void crearMetadata(uint32_t tipo, char* direccion){
	archivoHeader* metadataFile = malloc(sizeof(archivoHeader));
	char* nuevaDirec = malloc(strlen(direccion)+strlen("metadata.bin"));
	strcpy(nuevaDirec,direccion);
	string_append(&nuevaDirec,"metadata.bin");
	FILE* metadataArchivo = fopen(direccion,"w+b");
	switch(tipo){
	case DIRECTORIO:;
		printf("Soy directorio\n");
		metadataFile->esDirectorio = 'Y';
		metadataFile->estaAbierto = 'N';
		metadataFile->bloquesUsados = NULL;
		metadataFile->tamanioArchivo = 0;
		metadataFile->tipo = DIRECTORIO;
		escribirMetadata(metadataArchivo,metadataFile);
		//fwrite(&metadataFile,sizeof(archivoHeader),1, metadataArchivo);
		fclose(metadataArchivo);
		break;
	case ARCHIVO:;
		printf("Soy archivo\n");
		metadataFile->esDirectorio = 'N';
		metadataFile->estaAbierto = 'N';
		metadataFile->bloquesUsados = list_create();
		metadataFile->tamanioArchivo = 0;
		metadataFile->tipo = ARCHIVO;
		escribirMetadata(metadataArchivo,metadataFile);
		break;
	default:; printf("Manqueada\n");break;
	}
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
		list_add(listaBloques,(void*) bloqueActual);
	}
}

bool poseeArchivo(blockHeader* bloque){
	return bloque->file!=NULL;
}

bool estaLibre(uint32_t idBloque){
	return !bitarray_test_bit(bitmap, idBloque-1);
}

void ocuparBloque(uint32_t idBloque){
	bitarray_set_bit(bitmap,idBloque-1);
}

void liberarBloque(uint32_t idBloque){
	bitarray_clean_bit(bitmap,idBloque-1);
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

void escribirMetadata(FILE* archivoMetadata, archivoHeader* metadata){
	switch(metadata->tipo){
	case DIRECTORIO:;
		char* tipoArchivo = "DIRECTORY=Y\n";
		fwrite(tipoArchivo,strlen(tipoArchivo)+1,1,archivoMetadata);
		break;
	case ARCHIVO:;
		char* lineaDirectorio ="DIRECTORY=N\n";
		char* lineaSize = "SIZE=0\n";
		char* lineaBloques = "BLOCKS=[]\n";
		char* lineaOpen = "OPEN=N\n";
		fwrite(lineaDirectorio,strlen(lineaDirectorio)+1,1,archivoMetadata);
		fwrite(lineaSize,strlen(lineaSize)+1,1,archivoMetadata);
		fwrite(lineaBloques,strlen(lineaBloques)+1,1,archivoMetadata);
		fwrite(lineaOpen,strlen(lineaOpen)+1,1,archivoMetadata);
	}

}
