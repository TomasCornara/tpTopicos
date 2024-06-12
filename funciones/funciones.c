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
        fclose(merge); // Cerrar el archivo antes de retornar
        return;
    }

    // Creo la memoria que voy a usar de buffer
    void* buffer = malloc(tam);
    if (!buffer) {
        fprintf(stderr, "Error al crear el buffer para registros");
        free(vectorRegistros); // Liberar memoria antes de retornar
        fclose(merge); // Cerrar el archivo antes de retornar
        return;
    }


    /*Test para debug
    fread(buffer,tam,1,vecArchivos[1]);
    printf("%llu %llu %s %.2f \n",
               ((alumno*)buffer)->dni,
               ((alumno*)buffer)->fechaDeInscripcion,
               ((alumno*)buffer)->nombreYApellido,
               ((alumno*)buffer)->promedio);*/

    // Cargo el primer bache de registros manualmente
    void* auxReg = vectorRegistros;
    for (size_t i = 1; i <= cantArch; i++) {
        if (fread(auxReg, tam, 1, vecArchivos[i]) != 1) {
            // Manejar error de lectura
            fprintf(stderr, "Error al leer del archivo %zu", i);
            free(vectorRegistros);
            free(buffer);
            fclose(merge);
            return;
        }
        auxReg += tam;
    }

    // Fusión de archivos
    while (cantArch > 0) {
        // Encuentra el registro mínimo en el vector
        void* minReg = vectorRegistros;
        size_t minIdx = 0;
        for (size_t i = 1; i < cantArch; i++) {
            if (funComp((char*)vectorRegistros + i * tam, minReg) < 0) {
                minReg = (char*)vectorRegistros + i * tam;
                minIdx = i;
            }
        }

        // Escribe el registro mínimo en el archivo de fusión
        fwrite(minReg, tam, 1, merge);

        // Lee el siguiente registro del archivo correspondiente
        if (fread(minReg, tam, 1, vecArchivos[minIdx]) != 1) {
            // Si no se puede leer más del archivo, lo eliminamos del vector de archivos
            fclose(vecArchivos[minIdx]);
            for (size_t i = minIdx; i < cantArch - 1; i++) {
                vecArchivos[i] = vecArchivos[i + 1];
                memcpy((char*)vectorRegistros + i * tam, (char*)vectorRegistros + (i + 1) * tam, tam);
            }
            cantArch--;
        }
    }

    // Devuelvo la memoria pedida
    free(buffer);
    free(vectorRegistros);
    fclose(merge); // Cerrar el archivo antes de retornar
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

    for(int i = 1; i <= cant; i++){
        vec[i] = cargarArch(nombre,i);
        if(vec[i] != NULL){
            cantCargados+= 1;
        }
    }

    return cantCargados;
}


