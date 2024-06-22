#ifndef FUNC_H_INCLUDED
#define FUNC_H_INCLUDED

char *normalizarNombre(char *str); //Pone mayusculas en las iniciales y minusculas en las demas
int cargarArchivos(FILE** vec, char* nombre, size_t cant); //Carga multiples archivos a un vector
FILE* cargarArch(char* nombre, size_t num); //Abre un archivo dado su prefijo y numero
void mergeGenMult(FILE** vecArchivos, size_t cantArch, size_t tam,int (*funComp)(void*, void*)); //Merge multiple archivos cargados a un vector
int cmpFechaIns(void* elemA, void*elemB);//Funcion para ser usada dentro de la funcion Merge
void* instOrd(void* key, void* vec, size_t ce, size_t tam, int(*cmpFunc)(const void*, const void*));
int myrProm(const void* baseVd, const void* compVd);

#endif // FUNC_H_INCLUDED
