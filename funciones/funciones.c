#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



typedef struct alumno {
        unsigned long long dni;
        unsigned long long fechaDeInscripcion;
        char nombreYApellido[30];
        float promedio;
} alumno;

// Funcion para ordenar alumnos dentro de un vector por mayor promedio
int myrProm(void* baseVd, void* compVd){
    alumno* base = (alumno*)baseVd;
    alumno* comp = (alumno*)compVd;

    //Son equivalentes
    if(base->promedio == comp->promedio)
        return 0;

    //El elemento entrante es mejor que el que estaba
    if(base->promedio < comp->promedio)
        return -1;

    //El elemento entrante es peor que el que estaba
    return 1;
}

// Inserta elementos en un vector de forma ordenada y genérica
void* instOrd(void* key, void* vec, size_t ce, size_t tam, int (*cmpFunc)(const void*, const void*)) {
    int indRemplazo = 0;
    void* fin = vec + (ce * tam);
    int cmp = 1;

    for (void* aux = vec; aux < fin && cmp > 0; aux += tam, indRemplazo++) {
        cmp = cmpFunc(aux, key);
        // Si son iguales, la pos a cambiar sera la que sigue
        if (cmp == 0) {
            indRemplazo++;
        }
    }

    // Me aseguro de no salirme del vector
    if (indRemplazo > ce) {
        return NULL;
    }

    // Key es mejor que el elemento del vector
    void* posRemplazo = vec + ((indRemplazo - 1) * tam);

    // Mueve los elementos hacia adelante para hacer espacio para la nueva clave
    for (void* aux = fin - tam; aux >= posRemplazo; aux -= tam) {
        memcpy(aux + tam, aux, tam);
    }

    // Ya parado en donde voy a reemplazar, inserto key
    memcpy(posRemplazo, key, tam);

    // Devuelvo la posición que cambié, por si interesa guardarla
    return posRemplazo;
}


// Merge multiple archivos cargados a un vector
void mergeGenMult(FILE** vecArchivos, size_t cantArch, size_t tam, int (*funComp)(void*, void*)) {
    // Creo el archivo donde voy a mergear
    FILE* merge = fopen("merge.dat", "wb");
    if (!merge) {
        fprintf(stderr, "Error al crear el archivo merge");
        return;
    }

    // Creo el vector donde voy a cargar los registros
    void* vectorRegistros = malloc(cantArch * tam);
    if (!vectorRegistros) {
        fprintf(stderr, "Error al crear el vectorRegistros");
        fclose(merge);
        return;
    }

    // Creo la memoria que voy a usar de buffer
    void* buffer = malloc(tam);
    if (!buffer) {
        fprintf(stderr, "Error al crear el buffer para registros");
        free(vectorRegistros);
        fclose(merge);
        return;
    }

    // Cargo el primer bache de registros manualmente
    void* auxReg = vectorRegistros;
    for (int i = 0; i < cantArch; i++) {
        if (fread(auxReg, tam, 1, *(vecArchivos+i)) != 1) {
            fprintf(stderr, "Error al leer del archivo %zu", i);
            free(vectorRegistros);
            free(buffer);
            fclose(merge);
            return;
        }
        auxReg += tam;
    }

    // Merge de archivos
    while (cantArch > 0) {
        // Encuentra el mejor registro en el vector
        void* minReg = vectorRegistros;
        int minIdx = 0;
        for (int i = 0; i < cantArch; i++) {
            if (funComp(vectorRegistros + i * tam, minReg) < 0) {
                minReg = vectorRegistros + i * tam;
                minIdx = i;
            }
        }

        // Escribe el mejor
        fwrite(minReg, tam, 1, merge);

        // Lee el siguiente registro del archivo correspondiente
        if (fread(minReg, tam, 1, *(vecArchivos+minIdx) ) != 1) {
            // Si no se puede leer más del archivo, lo elimino del vector
            fclose(*(vecArchivos+minIdx));
            for (int i = minIdx; i < cantArch - 1; i++) {
                *(vecArchivos+i) = *(vecArchivos + (i+1));
                memcpy(vectorRegistros + i * tam, vectorRegistros + (i + 1) * tam, tam);
            }
            cantArch--;
        }
    }

    // Devuelvo la memoria pedida
    free(buffer);
    free(vectorRegistros);
    fclose(merge);
}



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
        return 1;

    if(alumnoB->fechaDeInscripcion > alumnoA->fechaDeInscripcion)
        return -1;

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

    for(int i = 0; i < cant; i++){
        vec[i] = cargarArch(nombre,i+1);
        if(vec[i] != NULL){
            cantCargados+= 1;
        }
    }

    return cantCargados;
}


