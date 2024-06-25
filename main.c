#include "func.h"
#include <time.h>
#define TAM 5
#define PARAMETROS_TOTALES 3
#define PARAMETRO_EJECUTABLE 0
#define PREFIJO_ARCHIVOS 1
#define CANT_ARCHIVOS 2

int main(int argc, char *argv[]){
    /// BLOQUE DE DECLARACION
    FILE *archivoFinal;
    FILE *mejoresAlumnos;
    int cantArchivos;
    FILE **vectorArchivos;
    int cantArchCargados;
    alumno top5Alum[TAM];
    size_t ce = 0;
    alumno buffer;
    alumno *aux;
    time_t tiempo;
    struct tm* tiempoLocal;
    /// FIN DEL BLOQUE DE DECLARACION

    /// BLOQUE DE ARGUMENTOS
    /*
        argv[0] contiene el nombre del programa
        argv[1] contiene el prefijo de los archivos
        argv[2] contiene la cantidad de archivos
    */

    if(argc < PARAMETROS_TOTALES){ //Chequeo que la cant de args sea correcta
        if(argc == 2){
            printf("Se requiere la cantidad de archivos.\n");
        } else {
            printf("Se requiere el prefijo de cada archivo y la cantidad de archivos.");
        }
        return(1);
    }
    /// FIN DE BLOQUE DE ARGUMENTOS


    /// INICIO BLOQUE MAIN
    printf("ARCHIVOS ABIERTOS:\n");
    cantArchivos = atoi(argv[CANT_ARCHIVOS]); //Conversion del 2do arg en numero

    vectorArchivos = malloc(cantArchivos * sizeof(FILE*)); //direcciones de memorias de los archivos
    if(!vectorArchivos){
        printf("Error al asignar memoria para el vector de archivos");
        return 1;
    }

    cantArchCargados = cargarArchivos(vectorArchivos,argv[PREFIJO_ARCHIVOS],cantArchivos); //Cargo las direcciones de los archivos y recibo cuantas se cargaron bien


    //Chequeo cantidad de archivos cargados
    printf("-> La cantidad de archivos cargados fue %d.\n",cantArchCargados);
    if(cantArchCargados != cantArchivos){
        printf("Error: La cantidad de archivos cargados no fue la esperada. Cargados %d, Esperados %d\n",cantArchCargados,cantArchivos);
        cerrarArchivosYLiberarMemoria(vectorArchivos, cantArchCargados);
        return 1;
    }


    //Proceso de merge
    archivoFinal= mergeGenMult(vectorArchivos,cantArchCargados,sizeof(alumno),cmpFechaIns);
    if(!archivoFinal){
        printf("Error al crear el archivo de mejores alumnos.\n");
        cerrarArchivosYLiberarMemoria(vectorArchivos,cantArchCargados);
    }

    //Normalizar nombres
    archivoFinal = normalizarArchivo(archivoFinal,"listadoTotalAlumnos.dat");
    if(!archivoFinal){
        printf("Error al normalizar los archivos.\n");
        cerrarArchivosYLiberarMemoria(vectorArchivos,cantArchCargados);
    }

    //Chequeo el merge
    archivoFinal = freopen("listadoTotalAlumnos.dat","rb",archivoFinal);
    printf("\nARCHIVO MERGE RESULTANTE: \n");
    fseek(archivoFinal, 0, SEEK_SET);
    while(fread(&buffer,sizeof(alumno),1,archivoFinal)){
        printf("%I64d %I64d %s %.2f \n",
               buffer.dni,
               buffer.fechaDeInscripcion,
               buffer.nombreYApellido,
               buffer.promedio);
    }
    printf("\n\n");


    //Carga vectror alumnos
    fseek(archivoFinal, 0, SEEK_SET);
    mejoresAlumnos = fopen("MejoresAlumnos.csv","wt");
    if (!mejoresAlumnos){
        printf("Error al crear el archivo de mejores alumnos.\n");
        fclose(archivoFinal);
        return(1);
    }

    //Insertamos alumnos en el vector
    while(fread(&buffer, sizeof(alumno), 1, archivoFinal) == 1){
        insOrd(top5Alum,&buffer,&ce,TAM,sizeof(alumno),myrProm);
    }


    //Guardo en el CSV e imprimo para chequear
    printf("TOP 5 ALUMNOS:\n");
    for (aux = top5Alum ; aux < top5Alum + TAM; aux++) {
        // Convierto a fecha el dato
        tiempo =(time_t)(aux->fechaDeInscripcion/1000); //Se divide entre 1000 porque esta en MS y lo necesito en SG. Sino no entra en time_t
        // Lo paso a estructura
        tiempoLocal = gmtime(&tiempo);

        // Chequeo que no haya error
        if (tiempoLocal == NULL) {
            printf("Error al convertir el tiempo para el alumno %s.\n",aux->nombreYApellido);
        } else {
            // Escribo en el archivo CSV
            fprintf(mejoresAlumnos, "%I64d,%02d/%02d/%d,%02d:%02d:%02d,%s,%.2f\n",
                aux->dni,
                tiempoLocal->tm_mday,
                tiempoLocal->tm_mon + 1,
                tiempoLocal->tm_year + 1900,
                tiempoLocal->tm_hour,
                tiempoLocal->tm_min,
                tiempoLocal->tm_sec,
                aux->nombreYApellido,
                aux->promedio);

            // Imprimo para chequear
            printf("%I64d,%02d/%02d/%d %02d:%02d:%02d,%s,%.2f\n",
                aux->dni,
                tiempoLocal->tm_mday,
                tiempoLocal->tm_mon + 1,
                tiempoLocal->tm_year + 1900,
                tiempoLocal->tm_hour,
                tiempoLocal->tm_min,
                tiempoLocal->tm_sec,
                aux->nombreYApellido,
                aux->promedio);
        }
    }
    /// FIN BLOQUE MAIN

    /// Cierre de archivos
    fclose(mejoresAlumnos);
    fclose(archivoFinal);
    return 0;
}

