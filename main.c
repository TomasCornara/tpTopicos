#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "func.h"
#define TAM 5
#define PARAMETROS_TOTALES 3
#define PARAMETRO_EJECUTABLE 0
#define PREFIJO_ARCHIVOS 1
#define CANT_ARCHIVOS 2

int main(int argc, char *argv[]){
    //BLOQUE DE ARGUMENTOS
    /*
        argv[0] contiene el nombre del programa
        argv[1] contiene el prefijo de los archivos
        argv[2] contiene la cantidad de archivos
    */

    if(argc < 3){ //Chequeo que la cant de args sea correcta
        if(argc == 2){
            fprintf(stderr,"Se requiere la cantidad de archivos.\n");
        } else {
            fprintf(stderr,"Se requiere el prefijo de cada archivo y la cantidad de archivos.");
        }
        return(1);
    }
    //FIN DE BLOQUE DE ARGUMENTOS





    //BLOQUE DE DECLARACION
    FILE *archivoFinal;
    FILE *mejoresAlumnos = fopen("MejoresAlumnos.csv","wt");
    if (!mejoresAlumnos){
        fprintf(stderr,"Error al crear el archivo de mejores alumnos.\n");
        return(1);
    }

    printf("ARCHIVOS ABIERTOS:\n");
    int cantArchivos = atoi(argv[2]); //Conversion del 2do arg en numero

    FILE **vectorArchivos = malloc(cantArchivos * sizeof(FILE*)); //direcciones de memorias de los archivos
    if(!vectorArchivos){
        fclose(mejoresAlumnos);
        fprintf(stderr,"Error al asignar memoria para el vector de archivos");
        return 1;
    }
    for(int i = 0; i < cantArchivos; i++) //Inicializacion en vacio
        vectorArchivos[i] = NULL;

    int cantArchCargados = cargarArchivos(vectorArchivos,argv[1],cantArchivos); //Cargo las direcciones de los archivos y recibo cuantas se cargaron bien

    alumno top5Alum[TAM] = {};
    size_t ce = 0;
    alumno buffer;
    //FIN DEL BLOQUE DE DECLARACION





    //INICIO BLOQUE MAIN
    //Chequeo cantidad de archivos cargados
    printf("-> La cantidad de archivos cargados fue %d.\n",cantArchCargados);


    //Proceso de merge
    archivoFinal= mergeGenMult(vectorArchivos,cantArchCargados,sizeof(alumno),cmpFechaIns);
    if(!archivoFinal){
        fprintf(stderr,"Error al crear el archivo de mejores alumnos.\n");
        fclose(mejoresAlumnos);
        free(vectorArchivos);
    }


    //Chequeo el merge
    printf("\nARCHIVO MERGE RESULTANTE: \n");
    fseek(archivoFinal, 0, SEEK_SET);
    while(fread(&buffer,sizeof(alumno),1,archivoFinal)){
        printf("%llu %llu %s %.2f \n",
               buffer.dni,
               buffer.fechaDeInscripcion,
               buffer.nombreYApellido,
               buffer.promedio);
    }
    printf("\n\n");


    //Carga vectror alumnos
    fseek(archivoFinal, 0, SEEK_SET);

    //Insertamos alumnos en el vector
    while(fread(&buffer, sizeof(alumno), 1, archivoFinal) == 1)
        insOrd(top5Alum,&buffer,&ce,TAM,sizeof(alumno),myrProm);

    //Check vector
    printf("TOP 5 ALUMNOS:\n");
    for (int i = 0; i < 5; i++) {
        printf("%llu %llu %s %.2f\n",
               top5Alum[i].dni,
               top5Alum[i].fechaDeInscripcion,
               top5Alum[i].nombreYApellido,
               top5Alum[i].promedio);
    }


    fclose(archivoFinal);
    return 0;
}

