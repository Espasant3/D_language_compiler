
#include <string.h>
#include "tabla_de_simbolos.h"
#include "definiciones.h"

struct celda {
    tipoelem info;
    struct celda *izq, *der;
};

tipoclave _clave_elem(tipoelem *E) {
    return E->comp_lexico;
}

int _comparar_claves(tipoclave cl1, tipoclave cl2) {
    return strcmp(cl1,cl2) == 0 ?  0 : strcmp (cl1,cl2) > 0 ? 1 : -1;
}

void crear(tablaSimbolos *TS) {
    *TS = NULL;
}

void destruir(tablaSimbolos *TS) {
    if (*TS != NULL) {
        destruir(&(*TS)->izq);
        destruir(&(*TS)->der);
        free((*TS)->info.comp_lexico);
        free(*TS);
        *TS = NULL;
    }
}

unsigned es_vacio(tablaSimbolos TS) {
    return TS == NULL;
}

tablaSimbolos izq(tablaSimbolos TS) {
    return TS->izq;
}

tablaSimbolos der(tablaSimbolos TS) {
    return TS->der;
}

void leer(tablaSimbolos TS, tipoelem *E) {
    *E = TS->info;
}
// Función privada para comparar las claves

int _comparar_clave_elem(tipoclave cl, tipoelem E) {
    return _comparar_claves(cl, _clave_elem(&E));
}
//Función privada para informar si una clave está en el árbol

unsigned _es_miembro_clave(tablaSimbolos TS, tipoclave cl) {
    if (es_vacio(TS)) {
        return 0;
    }
    int comp = _comparar_clave_elem(cl, TS->info);

    if (comp == 0) { //cl == TS->info
        return 1;
    }
    if (comp > 0) { //cl > TS->info
        return _es_miembro_clave(der(TS), cl);
    }
    //cl < TS->info
    return _es_miembro_clave(izq(TS), cl);
}

//Funciones públicas

unsigned es_miembro(tablaSimbolos TS, tipoelem E) {
    return _es_miembro_clave(TS, _clave_elem(&E));
}

void buscar_nodo(tablaSimbolos TS, tipoclave cl, tipoelem *nodo) {
    if (es_vacio(TS)) {
        return;
    }
    int comp = _comparar_clave_elem(cl, TS->info);

    if (comp == 0) { // cl == TS->info
        *nodo = TS->info;
    } else if (comp < 0) { // cl < TS->info
        buscar_nodo(TS->izq, cl, nodo);
    } else { // cl > TS->info
        buscar_nodo(TS->der, cl, nodo);
    }
}

void insertar(tablaSimbolos *TS, tipoelem E) {
    if (es_vacio(*TS)) {
        *TS = (tablaSimbolos ) malloc(sizeof (struct celda));
        (*TS)->info = E;
        (*TS)->izq = NULL;
        (*TS)->der = NULL;
        return;
    }
    tipoclave cl = _clave_elem(&E);
    int comp = _comparar_clave_elem(cl, (*TS)->info);
    if (comp > 0) {
        insertar(&(*TS)->der, E);
    } else {
        insertar(&(*TS)->izq, E);
    }
}

tipoelem _suprimir_min(tablaSimbolos * TS) {//Se devuelve el elemento más a la izquierda
    tablaSimbolos aux;
    tipoelem ele;
    if (es_vacio((*TS)->izq)) {//Si izquierda vacía, se devuelve valor nodo actual A
        ele = (*TS)->info;
        aux = *TS;
        *TS = (*TS)->der;
        free(aux);
        return ele;
    } else {
        return _suprimir_min(&(*TS)->izq); //se vuelve a buscar mínimo rama izquierda
    }
}

/* Funcion que permite eliminar un nodo del arbol */
void suprimir(tablaSimbolos *TS, tipoelem E) {
    tablaSimbolos aux;
    if (es_vacio(*TS)) {
        return;
    }

    tipoclave cl = _clave_elem(&E);
    int comp = _comparar_clave_elem(cl, (*TS)->info);
    if (comp < 0) { //if (E < (*A)->info) {
        suprimir(&(*TS)->izq, E);
    } else if (comp > 0) { //(E > (*A)->info) {
        suprimir(&(*TS)->der, E);
    } else if (es_vacio((*TS)->izq) && es_vacio((*TS)->der)) {
        //_destruir_elem(&((*A)->info));
        free(*TS);
        *TS = NULL;
    } else if (es_vacio((*TS)->izq)) { // pero no es vacio derecha
        aux = *TS;
        *TS = (*TS)->der;
        //_destruir_elem(&aux->info);
        free(aux);
    } else if (es_vacio((*TS)->der)) { //pero no es vacio izquierda
        aux = *TS;
        *TS = (*TS)->izq;
        //_destruir_elem(&aux->info);
        free(aux);
    } else { //ni derecha ni izquierda esta vacio, busco mínimo subárbol derecho
        //_destruir_elem(&(*A)->info); //elimino la info pero no libero el nodo,
        //pues en su sitio voy a poner el mínimo del subárbol derecho
        (*TS)->info = _suprimir_min(&(*TS)->der);
    }
}


tipoelem crearTipoelem(int value, char * comp_lexico) {
    tipoelem elemento;
    elemento.comp_lexico = (char *) malloc((strlen(comp_lexico) + 1)*sizeof(char)); // Aumentar en uno el tamaño
    if (elemento.comp_lexico == NULL) {
        // Manejar el error de memoria
        printf("Error: no se pudo asignar memoria\n");
        exit(1);
    }
    strcpy(elemento.comp_lexico, comp_lexico);
    elemento.comp_lexico[strlen(comp_lexico)] = '\0'; // Añadir el carácter nulo al final
    elemento.value = value;

    return elemento;
}



void initialize(tablaSimbolos *TS){

    crear(TS);

    insertar(TS, crearTipoelem(IMPORT, "import"));
    insertar(TS, crearTipoelem(DOUBLE, "double"));
    insertar(TS, crearTipoelem(WHILE, "while"));
    insertar(TS, crearTipoelem(FOREACH, "foreach"));
    insertar(TS, crearTipoelem(RETURN, "return"));
    insertar(TS, crearTipoelem(VOID, "void"));
    insertar(TS, crearTipoelem(INT, "int"));
    insertar(TS, crearTipoelem(IF, "if"));
    insertar(TS, crearTipoelem(ELSE, "else"));
    insertar(TS, crearTipoelem(DO, "do"));
    insertar(TS, crearTipoelem(FOR, "for"));
    insertar(TS, crearTipoelem(TRUE, "true"));
    insertar(TS, crearTipoelem(FALSE, "false"));
    insertar(TS,crearTipoelem(EQUALEQUAL, "=="));
    insertar(TS, crearTipoelem(PLUSEQUAL, "+="));
    insertar(TS, crearTipoelem(PLUSPLUS, "++"));



}


