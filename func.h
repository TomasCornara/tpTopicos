#ifndef FUNC_H_INCLUDED
#define FUNC_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>


typedef struct alumno {
        unsigned long long dni;
        unsigned long long fechaDeInscripcion;
        char nombreYApellido[30];
        float promedio;
} alumno;

char *normalizarNombre(char *str); //Pone mayusculas en las iniciales y minusculas en las demas
int cargarArchivos(FILE** vec, char* nombre, size_t cant); //Carga multiples archivos a un vector
FILE* openFile(char* nombre, size_t num); //Abre un archivo dado su prefijo y numero
FILE* mergeGenMult(FILE** vecArchivos, size_t cantArch, size_t tam,int (*funComp)(void*, void*)); //Merge multiple archivos cargados a un vector
int cmpFechaIns(void* elemA, void*elemB);//Funcion para ser usada dentro de la funcion Merge
int myrProm(const void* baseVd,const void* compVd); //Compara por mayor promedio
void* insOrd(void* inicio, void* elem, size_t* cantElems, size_t capVec, size_t tamElem, int (*cmp)(const void* a, const void* b)); //Inserta un elemento ordenado en un vector
void cerrarArchivosYLiberarMemoria(FILE **vectorArchivos, int cantArchCargados); // Cierra los archivos del vector
FILE* normalizarArchivo(FILE *archivo, char* nmbArch);// Pone mayusculas en las iniciales y minusculas en las demas

#endif // FUNC_H_INCLUDED
