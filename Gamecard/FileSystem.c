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
