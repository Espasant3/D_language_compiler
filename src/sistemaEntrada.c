#include "sistemaEntrada.h"
#include "errores.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define BUFFER_SIZE 160 //Tamaño total del buffer
#define BLOCK_SIZE BUFFER_SIZE-1 //Tamaño total utilizable del buffer

typedef struct {
    int bufferApuntado; // 1 será el buffer 1 y 2 será el buffer 2
    int position;       //La posición del puntero dentro del buffer
 } puntero;

puntero inicio;
puntero fin;

FILE *fichero;

char buffer1[BUFFER_SIZE];
char buffer2[BUFFER_SIZE];

bool retroceso = false;
/*
 * Función de apertura del fichero y de inicialización de los punteros, la inicialización solo se realizará una vez
 */
void open_fichero(){
    if(fichero == NULL){
        if((fichero = fopen("regression.d", "r"))){

            inicio.bufferApuntado = 1;
            inicio.position = 0;
            fin.bufferApuntado = 1;
            fin.position = 0;
            buffer1[BLOCK_SIZE] = EOF;
            buffer2[BLOCK_SIZE] = EOF;

            size_t escrito = fread(buffer1, 1, BLOCK_SIZE, fichero);

            if(escrito != BLOCK_SIZE){ //Si no se ha escrito todo quiere decir se ha llegado al fin de fichero
                buffer1[escrito] = EOF;
            }

            }else{
            printf("File doesn't exist\n");
            exit(EXIT_FAILURE);
        }
    }
}
/*
 * Función que cierra el fichero abierto
 */
void close_fichero(){
    fclose(fichero);
}
/*
 * Función auxiliar que devuelve la distancia entre punteros
 */
int _pointerDistance(){
    int distancia = 0;

    if(fin.bufferApuntado != inicio.bufferApuntado){
        distancia += (BLOCK_SIZE - inicio.position);
        distancia += fin.position;
    } else {
        distancia += (fin.position - inicio.position);
    }
    return distancia;
}
/*
 * Función que devuelve el buffer según el valor introducido, este valor debe ser el valor del buffer apuntado del puntero
 */
char * getBuffer(int pointedBuffer){
    if(pointedBuffer == 1){
        return buffer1;
    } else {
        return buffer2;
    }
}
/*
 * Función que indica si los punteros se han encontrado
 */
bool samePositionPointer(){
    return inicio.bufferApuntado == fin.bufferApuntado && inicio.position == fin.position;
}
/*
 * Función que llena el buffer contrario al que se le pasa, que deberá ser el buffer apuntado.
 */
void fillBuffer(int numBuf){
    size_t escrito = 0;
    if(numBuf == 1){ //Si el buffer cuyo final es el 1, se rellenará el buffer 2
        escrito = fread(buffer2, 1, BLOCK_SIZE, fichero);
        
        if(escrito != BLOCK_SIZE){ //Si no se ha escrito todo quiere decir se ha llegado al fin de fichero
            buffer2[escrito] = EOF;
        }
        //Como se está sustituyendo el buffer en el cual estaba el puntero de inicio, este pasa al siguiente buffer
        if(inicio.bufferApuntado == 2){
            errorOcurred(ERROR_TOO_LONG_LEXEMA, 0);
            inicio.bufferApuntado = 1;
            inicio.position = 0;
        }
    }
    else {
        escrito = fread(buffer1, 1, BLOCK_SIZE, fichero);

        if(escrito != BLOCK_SIZE){ //Si no se ha escrito todo quiere decir se ha llegado al fin de fichero
            buffer1[escrito] = EOF;
        }
        //Como se está sustituyendo el buffer en el cual estaba el puntero de inicio, este pasa al siguiente buffer
        if(inicio.bufferApuntado == 1){
            inicio.bufferApuntado = 2;
            inicio.position = 0;
        }
    }
}
/*
 * Función que cambia de buffer y actualiza el puntero de fin para que apunte al nuevo buffer.
 * Llenará el nuevo buffer si no ha habido retroceso.
 */
void changeBuffer(){

    if(!retroceso){
        fillBuffer(fin.bufferApuntado);
    }

    if(fin.bufferApuntado == 1){
        fin.bufferApuntado = 2;
        fin.position = 0;
    } else {
        fin.bufferApuntado = 1;
        fin.position = 0;
    }

}
/*
 * Función que actualiza la posición del puntero inicio a la posición siguiente a la actual
 */
void updateInicio(){
    inicio.position++;
    if(inicio.position > BLOCK_SIZE){ //Si se sobrepasa la última posición utilizable se cambia de buffer
        inicio.position = 0;
        if(inicio.bufferApuntado == 1){
            inicio.bufferApuntado = 2;
        } else {
            inicio.bufferApuntado = 1;
        }
    }
}
/*
 * Función que devuelve el caracter en la posición apuntada por el puntero de fin y lo actualiza a la siguiente posición
 */
char sig_carac(){
    char letra;
    open_fichero();
    letra = getBuffer(fin.bufferApuntado)[fin.position];
    if(letra != EOF){
        fin.position++;
    }else{ //Si se lee el EOF hay que valorar que tipo de EOF es
        if(fin.position < BLOCK_SIZE){ //Es el fin de fichero
            close_fichero();
            return EOF;
        }
        changeBuffer();

        letra = getBuffer(fin.bufferApuntado)[fin.position];
        fin.position++;
    }
    //Cada vez que se lee un caracter se desactiva el retroceso porque en caso de que lo hubiera habido ya no lo habría
    retroceso = false;
    return letra;
}
/*
 * Función que retrocede un caracter el puntero de fin y activa una variable que indica que ha habido retroceso para evitar
 * que se sustituya un buffer no recorrido
 */
void prev_carac(){

    retroceso = true;
    if(fin.position > 0){
        fin.position = fin.position -1;
    }else{
        fin.position = BLOCK_SIZE; //Se actualiza el puntero fin a la última posición del buffer anterior
        if(fin.bufferApuntado == 1){
            fin.bufferApuntado = 2;
        } else{
            fin.bufferApuntado = 1;
        }
    }
}
/*
 * Función que construye la palabra delimitada por los punteros inicio y fin
 */
char* build_word(){
    int tam = _pointerDistance();
    char* palabra;
    palabra = (char *) malloc((tam + 1) * sizeof (char ));
    int i = 0;

    while (!samePositionPointer()){

        if(inicio.position != BLOCK_SIZE){ //El EOF localizado en la posición BLOCK_SIZE no es parte de la palabra devuelta
            palabra[i] = getBuffer(inicio.bufferApuntado)[inicio.position];
        }

        updateInicio();
        i++;
    }
    palabra[tam] = '\0';
    return palabra;
}
/*
 * Función que devuelve una palabra del sistema de entrada
 */
char* get_word(){
    return build_word();
}
