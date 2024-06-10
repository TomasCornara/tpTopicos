#ifndef FUNC_H_INCLUDED
#define FUNC_H_INCLUDED

char *normalizarNombre(char *str); //Pone mayusculas en las iniciales y minusculas en las demas
int cargarArchivos(FILE** vec, char* nombre, size_t cant); //Carga multiples archivos a un vector
FILE* cargarArch(char* nombre, size_t num); //Abre un archivo dado su prefijo y numero

#endif // FUNC_H_INCLUDED
