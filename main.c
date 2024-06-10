#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "./func.h"

typedef struct alumno {
        unsigned long long dni;
        unsigned long long fechaDeInscripcion;
        char nombreYApellido[30];
        float promedio;
} alumno;

int main(int argc, char *argv[]){
    //argv[0] es el nombre del archivo
    //argv[1] es el prefijo
    //argv[2] es la cantidad de archivos


    //Chequea que se ingrese el prefijo de cada archivo y la cantidad a mergear
    if(argc < 3){
        if(argc < 2){
            printf("Se requiere la cantidad de archivos \n");
        } else {
            printf("Se requiere el prefijo de cada archivo");
        }
        return(-1);
    }

    //Paso el argumento de la cantidad a numero
    int cantArchivos = atoi(argv[2]);

    //Test
    printf("%d \n", cantArchivos);
    printf("%s",argv[1]);

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
