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

int main()
{
    char vector[]= "hOLA sOY Yo";
    printf("%s",normalizarNombre(vector));

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
