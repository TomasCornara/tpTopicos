#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "func.h"

typedef struct alumno {
        unsigned long long dni;
        unsigned long long fechaDeInscripcion;
        char nombreYApellido[30];
        float promedio;
} alumno;

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
    FILE *archivoFinal = fopen("merge.dat","wb+");
    if (!archivoFinal){
        fprintf(stderr,"Error al crear el archivo de merge.\n");
        return(1);
    };

    FILE *mejoresAlumnos = fopen("MejoresAlumnos.csv","w");
    if (!mejoresAlumnos){
        fprintf(stderr,"Error al crear el archivo de mejores alumnos.\n");
        return(1);
    }

    printf("ARCHIVOS ABIERTOS:\n");
    int cantArchivos = atoi(argv[2]); //Conversion del 2do arg en numero

    FILE **vectorArchivos = malloc(cantArchivos * sizeof(FILE*)); //direcciones de memorias de los archivos
    for(int i = 0; i < cantArchivos; i++) //Inicializacion en vacio
        vectorArchivos[i] = NULL;

    int cantArchCargados = cargarArchivos(vectorArchivos,argv[1],cantArchivos); //Cargo las direcciones de los archivos y recibo cuantas se cargaron bien

    alumno top5Alum[5] = {
        { -1, -1, "", 0.0 },
        { -1, -1, "", 0.0 },
        { -1, -1, "", 0.0 },
        { -1, -1, "", 0.0 },
        { -1, -1, "", 0.0 }
    };
    alumno buffer;
    //FIN DEL BLOQUE DE DECLARACION





    //INICIO BLOQUE MAIN
    //CHECK CANT ARCHIVOS CARGADOS
    printf("-> La cantidad de archivos cargados fue %d.\n",cantArchCargados);


    //PROCESO DE MERGE
    mergeGenMult(vectorArchivos,cantArchCargados,sizeof(alumno),cmpFechaIns);


    //CHECK MERGE
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


    //CARGA DEL VECTOR MEJORES ALUMNOS
    fseek(archivoFinal, 0, SEEK_SET);



    //Leo el primer registro, siempre va a ser el mayor
    if (fread(&top5Alum[0], sizeof(alumno), 1, archivoFinal) != 1) {
        fprintf(stderr,"Error al leer el primer registro\n");
        fclose(archivoFinal);
        return 1;
    }

    /*for(int i = 0; i < 5; i++){
        fread(&top5Alum[i], sizeof(alumno), 1, archivoFinal);
    }*/

    //Leo el resto
    while(fread(&buffer, sizeof(alumno), 1, archivoFinal) == 1)
        instOrd(&buffer, top5Alum, sizeof(top5Alum) / sizeof(top5Alum[0]), sizeof(top5Alum[0]), myrProm);


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

/* FILE *archivo = fopen("./archivos/estudiantes5.dat","rb");
    if(!archivo){
        printf("Error al abrir archivo");
        return 1;
    }

    alumno buffer;
    while(fread(&buffer,sizeof(alumno),1,archivo)){
        printf("%llu %llu %s %.2f \n",
               buffer.dni,
               buffer.fechaDeInscripcion,
               buffer.nombreYApellido,
               buffer.promedio);
    }

    fclose(archivo);

*/
