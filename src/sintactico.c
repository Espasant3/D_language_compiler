
#include "sintactico.h"
#include "lexico.h"

void pedirComponentes(tablaSimbolos *TS){
    tipoelem comp_pedido;
    while(1){
        comp_pedido = sig_comp_lexico(TS);
        if(comp_pedido.value == EOF){ //Finalización en el EOF
            free(comp_pedido.comp_lexico);
            break;
        }
        printf("\t< %-10s , %3d >\n", comp_pedido.comp_lexico ,comp_pedido.value );

        if(!es_miembro(*TS, comp_pedido)){ //Todo componente recibido que no está en la tabla de símbolos tiene que
            // liberar la memoria
            free(comp_pedido.comp_lexico);
        }
    }
} 