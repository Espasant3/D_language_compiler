#include <stdio.h>
#include <stdlib.h>
#include "errores.h"

void errorOcurred(int code, int line){


    switch (code) {
        case ERROR_NUM_START_0:
            printf("The number from line %d starts with 0 and is integer\n", line);
            break;
        case ERROR_NUM_CHAR:
            printf("The number in line %d contains character\n", line);
            break;
        case ERROR_NUM_BIN:
            printf("The binary number from line %d contains non binary numbers\n", line);
            break;
        case ERROR_EXP_POW:
            printf("The exponential number of the %d line has the wrong power format\n", line);
            break;
        case ERROR_NUM_FLOAT:
            printf("The float number of the %d line is in a wrong format\n", line);
            break;
        case ERROR_TOO_LONG_LEXEMA:
            printf("The current lexema is too long\n");
            break;
        default:
            printf("Error indeterminado en la línea %d\n",line);
            break;
    }
}

