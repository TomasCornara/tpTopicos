#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

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
