#include <ctype.h>
#include <string.h>
#include "../func.h"

/// Cierra los archivos del vector
void cerrarArchivosYLiberarMemoria(FILE **vectorArchivos, int cantArchCargados) {
    int i;
    FILE **pv = vectorArchivos;


    for (i = 0; i < cantArchCargados; i++) {
        fclose(*pv);
        pv++;
    }
    free(vectorArchivos);
}

/// Funcion para insertar un elemento en orden en un vector
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
    while (posActual > inicio && cmp((char*)posActual - tamElem, elem) < 0) {
        memcpy(posActual, (char*)posActual - tamElem, tamElem);
        posActual = (char*)posActual - tamElem;
    }

    // Copia el nuevo elemento en su posicion correcta
    memcpy(posActual, elem, tamElem);

    // Devuelve la posicion del elemento insertado
    return posActual;
}


/// Merge multiple archivos cargados a un vector
FILE* mergeGenMult(FILE** vecArchivos, size_t cantArch, size_t tam, int (*funComp)(void*, void*)) {
    FILE* merge;
    void* vectorRegistros;
    size_t i;
    void* minReg;
    int minIdx;

    //Pido memoria para los registros
    vectorRegistros = malloc(cantArch * tam);
    if (!vectorRegistros) {
        printf("Error al crear el vectorRegistros\n");
        return NULL;
    }

    //Abro el archivo final
    merge = fopen("listadoTotalAlumnos.dat", "wb");
    if (!merge) {
        printf("Error al crear el archivo merge\n");
        free(vectorRegistros);
        return NULL;
    }


    // Cargo el primer registro de cada archivo dentro del vector
    for (i = 0; i < cantArch; i++) {
        if (fread((char*)vectorRegistros + i * tam, tam, 1, *(vecArchivos+i)) != 1) {
            printf("Error al leer del archivo %lu\n", (long)i);
            free(vectorRegistros);
            fclose(merge);
            return NULL;
        }
    }

    /// Merge de archivos

    //Mientras haya archivos:
    while (cantArch > 0) {
        // Encuentra el mejor registro en el vector y su indice

        //Asumo que el mejor registro es el primero
        minReg = vectorRegistros;
        minIdx = 0;

        //Busco si hay uno mejor
        for (i = 1; i < cantArch; i++) {
            if (funComp((char*)vectorRegistros + i * tam, minReg) < 0) {
                minReg = (char*)vectorRegistros + i * tam;
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
        for (i = minIdx; i < cantArch - 1; i++) {
            //Rellena el espacio del FILE* eliminado
            *(vecArchivos+i) = *(vecArchivos+(i + 1)); //Corrige el vector de punteros a archivos
            memcpy((char*)vectorRegistros + i * tam, (char*)vectorRegistros + (i + 1) * tam, tam); //Corrige los registros para que coincidan con los archivos
        }
        cantArch--;
    }

    // Devuelvo la memoria pedida
    free(vectorRegistros);
    return merge;
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
    alumno reg;
    size_t tam = sizeof(alumno);
    FILE* temp = fopen("temp.dat", "wb");
    if (!temp) {
        printf("Error al generar archivo temporal\n");
        return NULL;
    }

    archivo = freopen(nmbArch,"rb+",archivo);
    if(!archivo){
        printf("Error al ganar permisos de lectura para archivo\n");
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
    FILE* archivo;
    char nombreArchivo[256];
    //Armo el nombre del archivo y lo busco
    sprintf(nombreArchivo,"%s%d%s",nombre,(int)num,".dat");
    archivo = fopen(nombreArchivo,"rb");

    printf("%s\n",nombreArchivo);
    //Chequeo que se haya encontrado
    if(!archivo){
        printf("Error al leer el archivo %s\n",nombreArchivo);
        return NULL;
    }

    //Si esta todo bien, devuelvo la direccion del archivo
    return(archivo);
}

/// Carga multiples archivos a un vector
int cargarArchivos(FILE** vec, char* nombre, size_t cant){
    int cantCargados = 0;
    int i;
    for(i = 0; i < cant; i++){
        *vec = openFile(nombre,i+1);
        if(*vec != NULL){
            cantCargados+= 1;
        }
        vec++;
    }

    return cantCargados;
}


