#include <ctype.h>
#include <string.h>
#include "../func.h"

/// Cierra los archivos del vector
void cerrarArchivosYLiberarMemoria(FILE **vectorArchivos, int cantArchCargados) {
    FILE **pv = vectorArchivos;
    for (int i = 0; i < cantArchCargados; i++) {
        fclose(*pv);
        pv++;
    }
    free(vectorArchivos);
}

/// Funcion para ordenar alumnos dentro de un vector por mayor promedio
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
    // Puntero al vector
    void* posActual = inicio;

    // Si el vector no esta lleno
    if (*cantElems < capVec) {
        // Coloca el puntero en la última posicion y aumenta la cantidad de elementos
        posActual = posActual + (*cantElems) * tamElem;
        (*cantElems)++;
    } else {
        // Evita salirse del vector
        posActual = posActual + (*cantElems - 1) * tamElem;

        // Si el elemento es menor que los elementos del vector, no se puede insertar
        if (cmp(posActual, elem) > 0) {
            return NULL;
        }
    }

    // Hago espacio para el nuevo elemento
    while (posActual > inicio && cmp(posActual - tamElem, elem) < 0) {
        memcpy(posActual, posActual - tamElem, tamElem);
        posActual = posActual - tamElem;
    }

    // Copia el nuevo elemento en su posicion correcta
    memcpy(posActual, elem, tamElem);

    // Devuelve la posicion del elemento insertado
    return posActual;
}


/// Merge multiple archivos cargados a un vector
FILE* mergeGenMult(FILE** vecArchivos, size_t cantArch, size_t tam, int (*funComp)(void*, void*)) {
    //Pido memoria para los registros
    void* vectorRegistros = malloc(cantArch * tam);
    if (!vectorRegistros) {
        fprintf(stderr, "Error al crear el vectorRegistros");
        return NULL;
    }

    //Abro el archivo final
    FILE* merge = fopen("listadoTotalAlumnos.dat", "wb");
    if (!merge) {
        fprintf(stderr, "Error al crear el archivo merge");
        free(vectorRegistros);
        return NULL;
    }


    // Cargo el primer registro de cada archivo dentro del vector
    for (size_t i = 0; i < cantArch; i++) {
        if (fread(vectorRegistros + i * tam, tam, 1, *(vecArchivos+i)) != 1) {
            fprintf(stderr, "Error al leer del archivo %zu", i);
            free(vectorRegistros);
            fclose(merge);
            return NULL;
        }
    }

    /// Merge de archivos

    //Mientras haya archivos:
    while (cantArch > 0) {
        // Encuentra el mejor registro en el vector y su indice

        //Asumo que es el primero
        void* minReg = vectorRegistros;
        int minIdx = 0;

        //Busco si hay uno mejor
        for (size_t i = 1; i < cantArch; i++) {
            if (funComp(vectorRegistros + i * tam, minReg) < 0) {
                minReg = vectorRegistros + i * tam;
                minIdx = i;
            }
        }

        // Escribe el mejor registro que encontre en el vector
        fwrite(minReg, tam, 1, merge);

        // Usando el indice de ese registro, cargo el resto de registros del archivo que me dio el mejor registro en el paso anterior
        while(fread(minReg, tam, 1, *(vecArchivos + minIdx)) == 1){
            fwrite(minReg, tam, 1, merge);
        }

        // Si no se puede leer más del archivo, lo elimino del vector
        fclose(*(vecArchivos + minIdx));
        for (size_t i = minIdx; i < cantArch - 1; i++) {
            *(vecArchivos+i) = *(vecArchivos+(i + 1));
            memcpy(vectorRegistros + i * tam, vectorRegistros + (i + 1) * tam, tam);
        }
        cantArch--;


        /*if(fread(minReg, tam, 1, *(vecArchivos + minIdx)) != 1) {
            // Si no se puede leer más del archivo, lo elimino del vector
            fclose(*(vecArchivos + minIdx));
            for (size_t i = minIdx; i < cantArch - 1; i++) {
                *(vecArchivos+i) = *(vecArchivos+(i + 1));
                memcpy(vectorRegistros + i * tam, vectorRegistros + (i + 1) * tam, tam);
            }
            cantArch--;
        }*/
    }

    // Devuelvo la memoria pedida
    free(vectorRegistros);
    return merge;
}


///ANTIGUA VERSION DEL MERGE
/*FILE* mergeGenMult(FILE** vecArchivos, size_t cantArch, size_t tam, int (*funComp)(void*, void*)) {
    void* vectorRegistros = malloc(cantArch * tam);
    void* buffer = malloc(tam);
    void* auxReg = vectorRegistros;
    FILE* merge;
    // Creo el archivo donde voy a mergear
    merge = fopen("listadoTotalAlumnos.dat", "wb+");
    if (!merge) {
        fprintf(stderr, "Error al crear el archivo merge");
        return NULL;
    }

    // Creo el vector donde voy a cargar los registros
    if (!vectorRegistros) {
        fprintf(stderr, "Error al crear el vectorRegistros");
        fclose(merge);
        return NULL;
    }

    // Creo la memoria que voy a usar de buffer
    if (!buffer) {
        fprintf(stderr, "Error al crear el buffer para registros");
        fclose(merge);
        return NULL;
    }

    // Cargo el primer bache de registros manualmente
    for (int i = 0; i < cantArch; i++) {
        if (fread(auxReg, tam, 1, *(vecArchivos+i)) != 1) {
            fprintf(stderr, "Error al leer del archivo %zu", i);
            free(vectorRegistros);
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
}*/

/// Funcion para ser usada dentro de la funcion Merge
int cmpFechaIns(void* elemA, void*elemB){
    alumno *alumnoA = (alumno*)elemA;
    alumno *alumnoB = (alumno*)elemB;

    if(alumnoB->fechaDeInscripcion < alumnoA->fechaDeInscripcion) //Cuando se encuentra un reg mejor
        return 1;

    if(alumnoB->fechaDeInscripcion > alumnoA->fechaDeInscripcion)
        return -1;

    return 0; //No debiera ocurrir nunca este caso
}

/// NORMALIZAMOS EL ARCHIVO RESULTANTE DE ALUMNOS
FILE* normalizarArchivo(FILE *archivo, char* nmbArch) {
    archivo = freopen(nmbArch,"rb+",archivo);
    if(!archivo){
        fprintf(stderr, "Error al ganar permisos de lectura para archivo\n");
        return NULL;
    }

    alumno reg;
    size_t tam = sizeof(alumno);
    FILE* temp = fopen("temp.dat", "wb");
    if (!temp) {
        fprintf(stderr, "Error al generar archivo temporal\n");
        return NULL;
    }

    fseek(archivo, 0, SEEK_SET);
    while (fread(&reg, tam, 1, archivo) == 1) {
        strcpy(reg.nombreYApellido,normalizarNombre(reg.nombreYApellido));
        fwrite(&reg, tam, 1, temp);
    }

    fclose(temp);
    fclose(archivo);
    remove(nmbArch);
    rename("temp.dat", nmbArch);
    return archivo;

}
/// Pone mayusculas en las iniciales y minusculas en las demas
char *normalizarNombre(char *str) {
    char *aux = str;
    int bandera = 1; // 1 = Mayus, 0 = Minus

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

/// Abre un archivo dado su prefijo y numero
FILE* openFile(char* nombre, size_t num){
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

/// Carga multiples archivos a un vector
int cargarArchivos(FILE** vec, char* nombre, size_t cant){
    int cantCargados = 0;

    for(int i = 0; i < cant; i++){
        vec[i] = openFile(nombre,i+1);
        if(vec[i] != NULL){
            cantCargados+= 1;
        }
    }

    return cantCargados;
}


