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
		log_info(gamecardLogger2,"No se encontro el directorio metadata");
	}else{
		log_info(gamecardLogger2,"Se encontro el directorio metadata");
		if(archivoExiste(buscarPath("/Metadata/Metadata.bin"))<0){
			log_info(gamecardLogger2,"No se encontro el archivo metadata.bin");
		}
		else{
			log_info(gamecardLogger2,"Se encontro el archivo metadata.bin");
			//char* pathMetadata = buscarPath("/Metadata/Metadata.bin");
		//	int descriptorFichero = open(pathMetadata,O_RDONLY);
		//	fstat(descriptorFichero,&bufferEstado);
		//	char* memoria = mmap(NULL,bufferEstado.st_size,PROT_READ,MAP_PRIVATE,descriptorFichero,0);

			t_config* config_metadata = config_create("/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/Metadata/metadata.bin");
			tallGrass.block_size  = config_get_int_value(config_metadata, "BLOCK_SIZE");
			tallGrass.blocks = config_get_int_value(config_metadata, "BLOCKS");
			tallGrass.magic_number = config_get_string_value(config_metadata, "MAGIC_NUMBER");
			log_info(gamecardLogger2,"Inicio de Metadata, Tamanio bloques: %i,  Cantidad de bloques: %i,  Magic number: %s", tallGrass.block_size, tallGrass.blocks, tallGrass.magic_number);
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

	//FILE* archivoBloque = fopen(string_from_format("%s/Blocks/%d.bin", puntoMontaje, bloque->id), "w");
	uint32_t fd = open(string_from_format("%s/Blocks/%d.bin", puntoMontaje, bloque->id),O_RDWR|O_CREAT,0777);
	ftruncate(fd,tallGrass.block_size);
	close(fd);
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
			log_info(gamecardLogger2,"El directorio ya existe");
			//Ver que hacer aca
			//return 0;
		}
		if(status < 0){
			log_info(gamecardLogger2,"Error al crear el directorio");
			//return -1;
		}
	}

	return crearMetadata(tipo, direc);
	//return crearNodoDirectorio(header, directorioPadre);



}

void eliminarDirectorio(char* path){
	rmdir(path);
}

archivoHeader* crearMetadata(uint32_t tipo, char* direccion){
	archivoHeader* metadataFile = malloc(sizeof(archivoHeader));
	char* nuevaDirec = malloc(strlen(direccion)+strlen("metadata.bin")+1);
	strcpy(nuevaDirec,direccion);
	string_append(&nuevaDirec,"/metadata.bin");
	metadataFile->pathArchivo = malloc(strlen(nuevaDirec)+1);
	strcpy(metadataFile->pathArchivo,nuevaDirec);
	FILE *archivoMetadata=fopen(nuevaDirec,"w+b");

	switch(tipo){
	case DIRECTORIO:;
		log_info(gamecardLogger2,"Soy directorio");
		metadataFile->esDirectorio = 'Y';
		metadataFile->estaAbierto = false;
		metadataFile->bloquesUsados = NULL;
		metadataFile->tamanioArchivo = 0;
		metadataFile->tipo = DIRECTORIO;
		escribirMetadata(metadataFile);
		//fwrite(&metadataFile,sizeof(archivoHeader),1, metadataArchivo);

		break;
	case ARCHIVO:;
		log_info(gamecardLogger2,"Soy archivo");
		metadataFile->esDirectorio = 'N';
		metadataFile->estaAbierto = false;
		metadataFile->bloquesUsados = list_create();
		metadataFile->tamanioArchivo = 0;
		metadataFile->tipo = ARCHIVO;
		var=metadataFile;
		escribirMetadata(metadataFile);
		break;
	default:; log_info(gamecardLogger2,"Manqueada");break;
	}

	fclose(archivoMetadata);
	return metadataFile;

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
	free(metadata);

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

int32_t escribirBloque(int32_t bloque, int32_t offset, int32_t longitud, char* buffer) {
	blockHeader* headerBloque=obtenerBloquePorId(bloque);
	if(tieneCapacidad(headerBloque,longitud)){
	disminuirCapacidad(headerBloque, longitud);
	log_info(gamecardLogger2, "Escribiendo bloque:%d, offset=%d, longitud=%d, buffer=%s", bloque, offset, longitud, buffer);
	//const char* bufferin = buffer;
	//FILE* block = fopen(string_from_format("%s%d.bin", pathBlocks, bloque), "w+b");
	//FILE* block = fopen(string_from_format("%s1.bin", pathBlocks), "r+");
	FILE* block = fopen("/home/utnso/tp-2020-1c-Programadores-en-Fuga/Gamecard/TALL_GRASS/Blocks/1.bin","r+");
	fseek(block, offset, SEEK_SET);
	fwrite(buffer, strlen(buffer)+1, 1, block);
	//fputs(buffer,block);
	headerBloque->pos=ftell(block);
	fclose(block);

	return 0;
	}else{
		return -1;
	}
}
