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
    FILE *archivoFinal = fopen("merge.dat","wb");
    if (!archivoFinal){
        fprintf(stderr,"Error al crear el archivo de merge.\n");
        return(1);
    };

    FILE *mejoresAlumnos = fopen("MejoresAlumnos.csv","w");
    if (!mejoresAlumnos){
        fprintf(stderr,"Error al crear el archivo de mejores alumnos.\n");
        return(1);
    }

    int cantArchivos = atoi(argv[2]); //Conversion del 2do arg en numero

    FILE **vectorArchivos = malloc(cantArchivos * sizeof(FILE*)); //direcciones de memorias de los archivos
    for(int i = 0; i < cantArchivos; i++) //Inicializacion en vacio
        vectorArchivos[i] = NULL;

    int cantArchCargados = cargarArchivos(vectorArchivos,argv[1],cantArchivos); //Cargo las direcciones de los archivos y recibo cuantas se cargaron bien

    /*alumno top5Alum[5] = {
                            {0,0,"",-1},
                            {0,0,"",-1},
                            {0,0,"",-1},
                            {0,0,"",-1},
                            {0,0,"",-1},
                         }; */ /*Nota: Normalmente un top deberia iniciar en el primer elemento que se cargue porque
                              no sabes en que numero empieza la comparacion pero aca si sabemos que no es posible que
                              un promedio de notas sea negativo, asi que inicializar en negativo
                              nos ahorra trabajo de implementacion*/
    //FIN DEL BLOQUE DE DECLARACION



    //check
    printf("La cantidad de archivos cargados fue %d.\n",cantArchCargados);

    //proceso de merge
    mergeGenMult(vectorArchivos,cantArchCargados,sizeof(alumno),cmpFechaIns);


    //check
    FILE* archivo = fopen("merge.dat","rb");
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
