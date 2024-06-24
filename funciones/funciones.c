#include <ctype.h>
#include <string.h>
#include "../func.h"

/// Cierra los archivos del vector y libera la memoria
void cerrarArchivosYLiberarMemoria(FILE **vectorArchivos, int cantArchCargados) {
    FILE **pv = vectorArchivos;
    for (int i = 0; i < cantArchCargados; i++) {
        fclose(*pv);
        pv++;
    }
    free(vectorArchivos);
}

/// Función para ordenar alumnos dentro de un vector por mayor promedio
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

/// Funcion para insertar un elemento en orden en un vector
void* insOrd(void* inicio, void* elem, size_t* cantElems, size_t capVec, size_t tamElem, int (*cmp)(const void* a, const void* b)) {
    char* posActual = (char*)inicio;
    char* posAnterior;
    char* posElem = (char*)elem;

    // Si el vector no esta lleno
    if (*cantElems < capVec) {
        // Coloca el puntero en la última posicion y aumenta la cantidad de elementos
        posActual = posActual + (*cantElems) * tamElem;
        (*cantElems)++;
    } else {
        // Evita salirse del vector
        posActual = posActual + (*cantElems - 1) * tamElem;

        // Si el elemento es menor que los elementos del vector, no se puede insertar
        if (cmp(posActual, posElem) > 0) {
            return NULL;
        }
    }

    // Hago espacio para el nuevo elemento
    posAnterior = posActual - tamElem;
    while (posActual > (char*)inicio && cmp(posAnterior, posElem) < 0) {
        memcpy(posActual, posAnterior, tamElem);
        posActual -= tamElem;
        posAnterior -= tamElem;
    }

    // Copia el nuevo elemento en su posicion correcta
    memcpy(posActual, posElem, tamElem);

    // Devuelve la posicion del elemento insertado
    return posActual;
}


/// Merge multiple archivos cargados a un vector
FILE* mergeGenMult(FILE** vecArchivos, size_t cantArch, size_t tam, int (*funComp)(void*, void*)) {
    void* vectorRegistros;
    FILE* merge;
    size_t i, minIdx;
    void* minReg;

    // Pido memoria para los registros
    vectorRegistros = malloc(cantArch * tam);
    if (!vectorRegistros) {
        fprintf(stderr, "Error al crear el vectorRegistros\n");
        return NULL;
    }

    // Abro el archivo final
    merge = fopen("listadoTotalAlumnos.dat", "wb");
    if (!merge) {
        fprintf(stderr, "Error al crear el archivo merge\n");
        free(vectorRegistros);
        return NULL;
    }

    // Cargo el primer registro de cada archivo dentro del vector
    for (i = 0; i < cantArch; i++) {
        if (fread((char*)vectorRegistros + i * tam, tam, 1, *(vecArchivos+ i)) != 1) {
            fprintf(stderr, "Error al leer del archivo %zu\n", i);
            free(vectorRegistros);
            fclose(merge);
            return NULL;
        }
    }

    // Merge de archivos
    while (cantArch > 0) {
        // Encuentra el mejor registro en el vector y su índice
        minReg = vectorRegistros;
        minIdx = 0;

        for (i = 1; i < cantArch; i++) {
            if (funComp((char*)vectorRegistros + i * tam, minReg) < 0) {
                minReg = (char*)vectorRegistros + i * tam;
                minIdx = i;
            }
        }

        // Normalizar el nombre antes de escribir el registro
        normalizarNombre(((alumno*)minReg)->nombreYApellido);

        // Escribe el mejor registro encontrado en el archivo merge
        fwrite(minReg, tam, 1, merge);

        // Usando el índice de ese registro, cargo el siguiente registro del archivo correspondiente
        if (fread(minReg, tam, 1, *(vecArchivos + minIdx)) != 1) {
            // Si no se puede leer más del archivo, lo elimino del vector
            fclose(*(vecArchivos + minIdx));
            for (i = minIdx; i < cantArch - 1; i++) {
                *(vecArchivos + i) = *(vecArchivos + i + 1);
                memcpy((char*)vectorRegistros + i * tam, (char*)vectorRegistros + (i + 1) * tam, tam);
            }
            cantArch--;
        }
    }

    // Libero la memoria pedida
    free(vectorRegistros);
    return merge;
}

/// Función para ser usada dentro de la función Merge
int cmpFechaIns(void* elemA, void* elemB) {
    alumno* alumnoA = (alumno*)elemA;
    alumno* alumnoB = (alumno*)elemB;

    if (alumnoB->fechaDeInscripcion < alumnoA->fechaDeInscripcion) // Cuando se encuentra un reg mejor
        return 1;

    if (alumnoB->fechaDeInscripcion > alumnoA->fechaDeInscripcion)
        return -1;

    return 0; // No debería ocurrir nunca este caso
}

/// Pone mayúsculas en las iniciales y minúsculas en las demás
char* normalizarNombre(char* str) {
    char* aux = str;
    int bandera = 1; // 1 = Mayúsculas, 0 = Minúsculas

    // Recorro el vector
    while (*aux != '\0') {
        if (isalpha((unsigned char)*aux)) {
            if (bandera == 1) {
                *aux = toupper((unsigned char)*aux);
                bandera = 0;
            } else {
                *aux = tolower((unsigned char)*aux);
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

/// Abre un archivo dado su prefijo y número
FILE* openFile(char* nombre, size_t num) {
    // Armo el nombre del archivo y lo busco
    char nombreArchivo[256];
    sprintf(nombreArchivo, "%s%zu%s", nombre, num, ".dat");
    FILE* archivo = fopen(nombreArchivo, "rb");

    printf("%s\n", nombreArchivo);
    // Chequeo que se haya encontrado
    if (!archivo) {
        fprintf(stderr, "Error al leer el archivo %s\n", nombreArchivo);
        return NULL;
    }

    // Si está todo bien, devuelvo la dirección del archivo
    return archivo;
}

/// Carga múltiples archivos a un vector
int cargarArchivos(FILE** vec, char* nombre, size_t cant) {
    int cantCargados = 0;

    for (int i = 0; i < cant; i++) {
        *(vec + i) = openFile(nombre, i + 1);
        if (*(vec + i) != NULL) {
            cantCargados += 1;
        }
    }

    return cantCargados;
}


