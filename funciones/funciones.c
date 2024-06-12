#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct alumno {
        unsigned long long dni;
        unsigned long long fechaDeInscripcion;
        char nombreYApellido[30];
        float promedio;
} alumno;

//Funcion de ordenamiento generica usando bubble sort
void genSort(void* vector, size_t tam, size_t cantElem, int (*compInt)(void*, void*)) {
    for (int i = 0; i < cantElem - 1; i++) {
        void* aux = vector;
        int cantCambios = 0;

        for (int j = 0; j < cantElem - i - 1; j++) {
            if (compInt(aux, aux + tam) == 1) {
                void* buffer = malloc(tam);
                memcpy(buffer, aux, tam);
                memcpy(aux, aux + tam, tam);
                memcpy(aux + tam, buffer, tam);
                free(buffer);
                cantCambios++;
            }
            aux += tam;
        }

        if (cantCambios == 0) {
            break;
        }
    }
}

//Funcion para ser usada dentro de la funcion Merge
int cmpFechaIns(void* elemA, void*elemB){
    alumno *alumnoA = (alumno*)elemA;
    alumno *alumnoB = (alumno*)elemB;

    if(alumnoB->fechaDeInscripcion < alumnoA->fechaDeInscripcion) //Cuando se encuentra un reg mejor
        return -1;

    if(alumnoB->fechaDeInscripcion > alumnoA->fechaDeInscripcion)
        return 1;

    return 0; //No debiera ocurrir nunca este caso
}

//Pone mayusculas en las iniciales y minusculas en las demas
char *normalizarNombre(char *str) {
    char *aux = str;
    int bandera = 1; //1 = Mayus, 0 = Minus

    // Recorro el vector
    while (*aux != '\0') {
        if (isalpha(*aux)) {
            if (bandera == 1) {
                *aux = toupper(*aux);
                bandera = 0;
            } else {
                *aux = tolower(*aux);
            }
        }

        // Si encuentro un espacio, reseteo la bandera y continúo
        if (*aux == ' ') {
            bandera = 1;
        }

        aux++;
    }

    return str;
}

//Abre un archivo dado su prefijo y numero
FILE* cargarArch(char* nombre, size_t num){
    //Armo el nombre del archivo y lo busco
    char nombreArchivo[256];
    sprintf(nombreArchivo,"%s%zu%s",nombre,num,".dat");
    FILE* archivo = fopen(nombreArchivo,"rb");

    printf("%s\n",nombreArchivo);
    //Chequeo que se haya encontrado
    if(!archivo){
        fprintf(stderr,"Error al leer el archivo %s\n",nombreArchivo);
        return NULL;
    }

    //Si esta todo bien, devuelvo la direccion del archivo
    return(archivo);
}

//Carga multiples archivos a un vector
int cargarArchivos(FILE** vec, char* nombre, size_t cant){
    int cantCargados = 0;

    for(int i = 1; i <= cant; i++){
        vec[i] = cargarArch(nombre,i);
        if(vec[i] != NULL){
            cantCargados+= 1;
        }
    }

    return cantCargados;
}


