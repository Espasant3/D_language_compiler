#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "sintactico.h"
#include "tabla_de_simbolos.h"
#include <string.h>

int main(int argc, char** argv) {

    tablaSimbolos ts;

    initialize(&ts);

    pedirComponentes(&ts);

    destruir(&ts);

    exit(EXIT_SUCCESS);

}
