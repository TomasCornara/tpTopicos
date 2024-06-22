#include <ctype.h>
#include <string.h>
#include "../func.h"

// Funcion para ordenar alumnos dentro de un vector por mayor promedio
int myrProm(const void* baseVd, const void* compVd) {
    const alumno* base = (const alumno*)baseVd;
    const alumno* comp = (const alumno*)compVd;

    // El elemento base tiene un promedio mayor que el comparado
    if (base->promedio > comp->promedio)
        return 1;

    // El elemento base tiene un promedio menor que el comparado
    if (base->promedio < comp->promedio)
        return -1;

    // Ambos elementos tienen el mismo promedio
    return 0;
}


#include <stddef.h>
#include <string.h>

// Funcion para insertar un elemento en orden en un vector
void* insOrd(void* inicio, void* elem, size_t* cantElems, size_t capVec, size_t tamElem, int (*cmp)(const void* a, const void* b)) {
    // Puntero al vector
    void* posActual = inicio;

    // Si el vector no esta lleno
    if (*cantElems < capVec) {
        // Coloca el puntero en la última posicion y aumenta la cantidad de elementos
        posActual = (char*)posActual + (*cantElems) * tamElem;
        (*cantElems)++;
    } else {
        // Evita salirse del vector
        posActual = (char*)posActual + (*cantElems - 1) * tamElem;

        // Si el elemento es menor que los elementos del vector, no se puede insertar
        if (cmp(posActual, elem) > 0) {
            return NULL;
        }
    }

    // Hago espacio para el nuevo elemento
    while ((char*)posActual > (char*)inicio && cmp((char*)posActual - tamElem, elem) < 0) {
        memcpy(posActual, (char*)posActual - tamElem, tamElem);
        posActual = (char*)posActual - tamElem;
    }

    // Copia el nuevo elemento en su posicion correcta
    memcpy(posActual, elem, tamElem);

    // Devuelve la posicion del elemento insertado
    return posActual;
}


// Merge multiple archivos cargados a un vector
FILE* mergeGenMult(FILE** vecArchivos, size_t cantArch, size_t tam, int (*funComp)(void*, void*)) {
    // Creo el archivo donde voy a mergear
    FILE* merge = fopen("merge.dat", "wb+");
    if (!merge) {
        fprintf(stderr, "Error al crear el archivo merge");
        return NULL;
    }

    // Creo el vector donde voy a cargar los registros
    void* vectorRegistros = malloc(cantArch * tam);
    if (!vectorRegistros) {
        fprintf(stderr, "Error al crear el vectorRegistros");
        fclose(merge);
        return NULL;
    }

    // Creo la memoria que voy a usar de buffer
    void* buffer = malloc(tam);
    if (!buffer) {
        fprintf(stderr, "Error al crear el buffer para registros");
        free(vectorRegistros);
        fclose(merge);
        return NULL;
    }

    // Cargo el primer bache de registros manualmente
    void* auxReg = vectorRegistros;
    for (int i = 0; i < cantArch; i++) {
        if (fread(auxReg, tam, 1, *(vecArchivos+i)) != 1) {
            fprintf(stderr, "Error al leer del archivo %zu", i);
            free(vectorRegistros);
            free(buffer);
            fclose(merge);
            return NULL;
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
    return merge;
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


