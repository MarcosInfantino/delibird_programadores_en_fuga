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
		//char a= '0';
		fputs("0",archivoBitmap);
	}
	fclose(archivoBitmap);

	uint32_t descriptor = open(pathBitmap, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
	//struct stat sb;

	//ftruncate(descriptor, tallGrass.blocks);
	//fstat(descriptor,&sb);
	fsync(descriptor);
	mmapBitmap = mmap(NULL, tallGrass.blocks, PROT_READ | PROT_WRITE, MAP_SHARED, descriptor, 0);

	if(mmapBitmap == NULL){
		printf("El bitmap esta vacio.\n");
		//Chequear si es un error
	}
	close(descriptor);
	//mmapBitmap[0]='1';
//	 bitmap = bitarray_create_with_mode((char*) mmapBitmap, tallGrass.blocks, LSB_FIRST);
//	//bitmap = bitarray_create_(mmapBitmap, sb.st_size);
////	for(uint32_t i = 0; i < tallGrass.blocks; i++){
////			 bitarray_clean_bit(bitmap, i);
////		}
//	bitarray_set_bit(bitmap, 0);
//	msync(mmapBitmap,tallGrass.blocks,MS_ASYNC);

}


uint32_t crearBloque() {
	printf("Creando Bloque\n");
	int i;
	for (i = 0; i < (bitmap->size * 8); i++) {
		if (bitarray_test_bit(bitmap, i) == 0) {
			bitarray_set_bit(bitmap, 1);
			FILE* bloque = fopen(string_from_format("%s/Blocks/%d.bin", puntoMontaje, i), "w");

			if (bloque == NULL){
				return -1;
			}
			fclose(bloque);
			actualizarArchivoBitmap();
			printf("Bloque %i creado", i);
			return i;
		}
	}

	return -1;
}

void actualizarArchivoBitmap() {
	FILE* archivoBitmap = fopen(buscarPath("Metadata/bitmap.bin"),"w");
	fputs(string_substring(bitmap->bitarray, 0, tallGrass.blocks / 8), archivoBitmap);
	fclose(archivoBitmap);
}


