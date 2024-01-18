

#ifndef PRACTICA_1_TABLA_DE_SIMBOLOS_H
#define PRACTICA_1_TABLA_DE_SIMBOLOS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef char* tipoclave;

typedef struct {
    int value;
    char * comp_lexico;
}tipoelem;

typedef struct celda * tablaSimbolos;//tipo opaco

typedef void *elem;

void crear(tablaSimbolos *TS);

void destruir(tablaSimbolos *TS);

unsigned es_vacio(tablaSimbolos TS);

tablaSimbolos izq(tablaSimbolos TS);

tablaSimbolos der(tablaSimbolos TS);

void leer(tablaSimbolos TS, tipoelem *E);

unsigned es_miembro(tablaSimbolos TS, tipoelem E);

void buscar_nodo(tablaSimbolos TS, tipoclave cl, tipoelem *nodo);

void insertar(tablaSimbolos *TS, tipoelem E);

void suprimir(tablaSimbolos *TS,tipoelem E);

void initialize(tablaSimbolos *TS);

/*
void crear(elem *elem1, int valor, char buffer[TBUFFER]);

unsigned isEmpty(elem el);

void info(elem elem1);

void liberar(elem *elem1);

*/
#endif //PRACTICA_1_TABLA_DE_SIMBOLOS_H
