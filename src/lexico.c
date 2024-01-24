
#include "lexico.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "tabla_de_simbolos.h"
#include "errores.h"
#include "definiciones.h"
#include "sistemaEntrada.h"

const int comment = -50; //Constante empleada para evitar mandar los comentarios al analizados sintáctico
int line = 0; //Contador del número de línea actual para mandar en caso de error

void asign_file(char* file_name){
    set_file(file_name);
}

typedef enum {
    START_BINARY,
    INTEGER,
    FLOAT,
    EXPONENT,
    BINARY,
    EXPONENT_SIGN,
    EXPONENT_NO_SIGN,
    HEXADECIMAL,
} state_number; //Tipo enumerado que tiene como fin simular los estados del autómata de números

/*
 * Función que crea un tipoelem con el valor y con el componente léxico introducidos
 */
tipoelem createTipoelem(int value, char * comp_lexico) {
    tipoelem elemento;
    elemento.comp_lexico = (char *) malloc((strlen(comp_lexico) + 1 )*sizeof(char));
    strcpy(elemento.comp_lexico, comp_lexico);
    elemento.comp_lexico [strlen(comp_lexico)] = '\0';
    elemento.value = value;
    return elemento;
}
/*
 * Función auxiliar para comprobar si los caracteres son binarios
 */
bool _is_binary_digit(char c) {
    return c == '0' || c == '1';
}
/*
 * Función auxiliar para comprobar si los caracteres son letras hexadecimales válidas
 */
bool _is_hex_letter(char c){
    return c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F'
    || c == 'a' || c == 'b' || c == 'c' || c == 'd' || c == 'e' || c == 'f';
}
/*
 * Función auxiliar para comprobar si el caracter es de aceptación en el autómata alfanumérico
 */
bool _is_accepted_alfanum(char c){
    return c == '.' || c == '(' || c == ')' || c == '{' || c == '}' ||
           c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == ',' ||
           c == '<' || c == '>' || c == '=' || c == '[' || c == ']' || c == ';';
}
/*
 * Función auxiliar que comprueba si el componente léxico es de tipo individual, es decir, independientemente de lo que
 * tenga a continuación
 */
bool _is_comp_lex_invidual(char c){
    return c == '.' || c == '(' || c == ')' || c == '{' || c == '}' ||
           c == ',' || c == '[' || c == ']' || c == ';';
}
/*
 * Autómata de lexemas alfanuméricos
 */
tipoelem autom_alfanum(tablaSimbolos *ts){

    char letra;
    tipoelem elemento;
    elemento.value = 0;
    char * word;
    while (1){
        letra = sig_carac();

        if(isspace(letra)){ //Aceptación
            if(letra == '\n'){
                line++;
            }
            prev_carac();
            break;
        } else if(_is_accepted_alfanum(letra)) { //Acepta con retroceso
            prev_carac();
            break;
        } else if(isalnum(letra) || letra == '_'){ //Comportamiento normal

        }
    }
    word = get_word();
    buscar_nodo(*ts,word,&elemento); //Se busca el elemento en la tabla de símbolos

    if(elemento.value == 0){ //El elemento no existe en la tabla de símbolos

        elemento = createTipoelem(ID,word); //Se crea un elemento nuevo con valor ID para la palabra
        insertar(ts,elemento);
    }
    return elemento;
}
/*
 * Autómata de lexemas que son números
 */
tipoelem autom_num( char recepcion){

    char letra;
    char * word;
    state_number estadoActual;
    bool was_number = false; //Variable empleada para verificar si el número binario tiene algún número después del prefijo

    if(recepcion == '0'){
        estadoActual = START_BINARY;
    } else {
        estadoActual = INTEGER;
    }

    while (1){

        letra = sig_carac();
        switch (estadoActual) {

            case START_BINARY: // En este estado no puede haber ningún número Integer
                if(isdigit(letra)){ // Salta error
                    errorOcurred(ERROR_NUM_START_0, line);
                    word = get_word();
                    return createTipoelem(INT_NUM, word);
                } else if(letra == '.'){
                    estadoActual = FLOAT;
                } else if(letra == 'b' || letra == 'B'){
                    estadoActual = BINARY;
                } else if(letra == 'x' || letra == 'X'){
                    estadoActual = HEXADECIMAL;
                } else if(isspace(letra)){
                    // Aceptación
                    if(letra == '\n'){
                        line++;
                    }
                    prev_carac();
                    word = get_word();
                    return createTipoelem(INT_NUM, word);

                } else if(isalpha(letra)){ // Salta error
                    errorOcurred(ERROR_NUM_CHAR, line);
                    word = get_word();
                    return createTipoelem(INT_NUM, word);
                } else{
                    //Aceptación con retroceso
                    prev_carac();
                    word = get_word();
                    return createTipoelem(INT_NUM, word);
                }
                break;

            case INTEGER: //Estado dedicado a lo números enteros
                if(isdigit(letra)){ //Funcionamiento normal

                } else if(letra == '.'){
                    estadoActual = FLOAT;

                } else if(letra == 'e' || letra == 'E'){
                    estadoActual = EXPONENT;

                }else if(letra == '_'){ //Funcionamiento correcto

                }else if(isspace(letra)){
                    //Aceptación
                    if(letra == '\n'){
                        line++;
                    }
                    prev_carac();
                    word = get_word();
                    return createTipoelem(INT_NUM, word);

                } else if(isalpha(letra)){ // Salta error
                    errorOcurred(ERROR_NUM_CHAR, line);
                    word = get_word();
                    return createTipoelem(INT_NUM, word);
                } else {
                    //Aceptación con retroceso
                    prev_carac();
                    word = get_word();
                    return createTipoelem(INT_NUM, word);
                }
                break;

            case FLOAT: // Estado dedicado a los números float (o double)
                if(isdigit(letra)){ //Funcionamiento normal

                } else if(letra == '.'){ // Salta error

                    errorOcurred(ERROR_NUM_FLOAT, line);
                    word = get_word();
                    return createTipoelem(FLOAT_NUMBER, word);

                } else if(letra == 'e' || letra == 'E'){
                    estadoActual = EXPONENT;

                }else if(letra == '_'){ //Funciona correctamente

                }else if(isspace(letra)){
                    //Aceptación
                    if(letra == '\n'){
                        line++;
                    }
                    prev_carac();
                    word = get_word();
                    return createTipoelem(FLOAT_NUMBER, word);

                } else if(isalpha(letra)){ // Salta error
                    errorOcurred(ERROR_NUM_CHAR, line);
                    return createTipoelem(FLOAT_NUMBER, "numero_float");
                } else {
                    //Aceptación con retroceso
                    prev_carac();
                    word = get_word();
                    return createTipoelem(FLOAT_NUMBER, word);
                }
                break;

            case BINARY: //Estado dedicado a los números binarios
                if(_is_binary_digit(letra)){ //Funcionamiento normal

                    was_number = true; //Una vez recibido el primer caracter después del prefijo se puede aceptar la cadena
                } else if(isdigit(letra)){ // Salta error
                    errorOcurred(ERROR_NUM_BIN, line);
                    word = get_word();
                    return createTipoelem(BINARY_NUM, word);

                } else if(isalpha(letra)){ // Salta error
                    errorOcurred(ERROR_NUM_CHAR, line);
                    word = get_word();
                    return createTipoelem(BINARY_NUM, word);

                } else if(isspace(letra)){
                    //Aceptación
                    if(letra == '\n'){
                        line++;
                    }
                    prev_carac();
                    word = get_word();
                    return createTipoelem(BINARY_NUM, word);

                } else if(letra == '_' && was_number){ //Funcionamiento correcto

                } else{
                    //Aceptación con retroceso
                    prev_carac();
                    word = get_word();
                    return createTipoelem(BINARY_NUM, word);
                }
                break;
            case HEXADECIMAL: // Estado dedicado a los números hexadecimales
                if(isdigit(letra) || _is_hex_letter(letra)){
                    was_number = true;
                } else if(isspace(letra)){
                    //Aceptación
                    if(letra == '\n'){
                        line++;
                    }
                    prev_carac();
                    word = get_word();
                    return createTipoelem(HEX_NUMBER, word);

                } else if(isalpha(letra)){ // Salta error

                    errorOcurred(ERROR_NUM_CHAR, line);
                    word = get_word();
                    return createTipoelem(HEX_NUMBER, word);

                } else{
                    //Aceptación con retroceso
                    prev_carac();
                    word = get_word();
                    return createTipoelem(HEX_NUMBER, word);
                }
                break;

            case EXPONENT: // Estado intermedio para los números exponenciales
                if(letra == '+' || letra == '-'){
                    estadoActual = EXPONENT_SIGN;
                } else if(isdigit(letra)){
                    estadoActual = EXPONENT_NO_SIGN;
                } else { // Salta error
                    errorOcurred(ERROR_EXP_POW, line);
                    word = get_word();
                    return createTipoelem(EXP_NUMBER, word);
                }
                break;

            case EXPONENT_SIGN: //Estado transitorio previo a la posible aceptación
                if(isdigit(letra)) {
                    estadoActual = EXPONENT_NO_SIGN;
                } else { // Salta error
                    errorOcurred(ERROR_EXP_POW, line);
                    word = get_word();
                    return createTipoelem(EXP_NUMBER, word);
                }

            case EXPONENT_NO_SIGN: // Estado dedicado al exponente de los números exponenciales

                if(isdigit(letra) || letra == '_'){ //Funcionamiento normal

                } else if(isspace(letra)){
                    //Aceptación
                    if(letra == '\n'){
                        line++;
                    }
                    prev_carac();
                    word = get_word();
                    return createTipoelem(EXP_NUMBER, word);

                } else if(isalpha(letra)){ // Salta error
                    errorOcurred(ERROR_NUM_CHAR, line);
                    word = get_word();
                    return createTipoelem(EXP_NUMBER, word);

                } else {
                    //Aceptación con retroceso
                    prev_carac();
                    word = get_word();
                    return createTipoelem(EXP_NUMBER, word);
                }
                break;
        }
    }
}
/*
 * Autómata dedicado a las cadenas de tipo string que están delimitadas por \"
 */
tipoelem autom_string(){

    char letra;
    char *word;
    enum {NORMAL, SPECIAL} estado = NORMAL;
    while(1){

        letra = sig_carac();
        switch (estado) {
            case NORMAL:
                if(letra == '\"'){ //Cuando se recibe una " se acepta el string
                    word = get_word();
                    free(word);
                    return createTipoelem(STRING, "string");
                }
                if(letra == '\\'){ // Si se recibe un caracter de escape si a continuación aparece una " esta no aceptará el string
                    estado = SPECIAL;
                }
                break;
            case SPECIAL: //Estado especial que sirve para no aceptar las posibles comillas dobles que aparezcan a continuación
                estado = NORMAL;
                break;
        }
    }
}
/*
 * Autómata de comentarios de línea
 */
void autom_coment_line(){
    char letra;
    char *word;
    while(1){
        letra = sig_carac();
        if(letra == '\n'){ //Aceptación al cambiar de línea
            line++;
            // Se fuerza el desplazamiento del puntero de fin para que el próximo lexema no contenga el comentario
            word = get_word();
            free(word);
            return;
        }
    }
}
/*
 * Autómata de comentarios anidados
 */
void autom_coment_nested(){ //Comentarios anidados del tipo /++/
    //El anidamiento se maneja con la pila del sistema mediante llamadas recursivas

    char letra;
    char * word;
    enum {NORMAL, END, NEW} estado = NORMAL;
    while (1){
        letra = sig_carac();
        if(letra == '\n'){
            line++;
        }
        switch (estado) {
            case NORMAL: //Estado de funcionamiento normal
                if(letra == '+'){
                    estado = END;
                }else if(letra == '/'){
                    estado = NEW;
                }
                break;
            case END: //Estado previo a una posible aceptación
                if(letra == '/'){
                    word = get_word();
                    free(word);
                    return;
                } else if(letra != '+'){
                    estado = NORMAL;
                }
                break;
            case NEW: //Estado previo al comienzo de un nuevo comentario anidado
                if(letra == '+'){
                    autom_coment_nested();
                } else if(letra != '/'){
                    estado = NORMAL;
                }
                break;
        }
    }
}
/*
 * Autómata de comentarios multilínea
 */
void autom_coment_multiline(){ //Comentarios del tipo /**/
    char letra;
    char * word;
    bool astar = false;
    while (1){
        letra = sig_carac();

        if(astar && letra == '/'){
            word = get_word();
            free(word);
            return;
        }
        if(letra == '\n'){
            line++;
        }
        if(letra == '*'){
            astar = true;
        } else{
            astar = false;
        }
    }
}
/*
 * Autómata dedicado a los operadores
 */
tipoelem autom_operad(char recepcion){

    char letra;
    char * word;

    enum {NORMAL, BAR, PLUS, EQUALS} estadoActual = NORMAL;

    if(recepcion == '/'){
        estadoActual = BAR;
    } else if(recepcion == '='){
        estadoActual = EQUALS;
    } else if(recepcion == '+'){
        estadoActual = PLUS;
    }

    while (1){
        letra = sig_carac();
        switch (estadoActual) {

            case NORMAL:
                if(isspace(letra)){
                    //Aceptación
                    if(letra == '\n'){
                        line++;
                    }
                    prev_carac();
                    word = get_word();
                    return createTipoelem(recepcion, word);

                } else {
                    //Aceptación con retroceso
                    prev_carac();
                    word = get_word();
                    return createTipoelem(recepcion, word);
                }

            case BAR: //Estado especial que detecta posibles comentarios
                switch (letra) {
                    case '/':
                        autom_coment_line();
                        return createTipoelem(comment, "NULO");
                        break;
                    case '*':
                        autom_coment_multiline();
                        return createTipoelem(comment, "NULO");
                        break;
                    case '+':
                        autom_coment_nested();
                        return createTipoelem(comment, "NULO");
                        break;
                    default:
                        if(isspace(letra)){
                            //Aceptación
                            if(letra == '\n'){
                                line++;
                            }
                            prev_carac();
                            word = get_word();
                            return createTipoelem(recepcion, word);
                        } else{
                            //Aceptación con retroceso
                            prev_carac();
                            word = get_word();
                            return createTipoelem(recepcion, word);
                        }
                        break;
                }

            case PLUS: //Estado especial que detecta operadores especiales que comienzan por +
                if(letra == '+'){ //Aceptación especial
                    word = get_word();
                    return createTipoelem(PLUSPLUS, word); // "++"
                } else if(letra == '='){
                    //Aceptación especial
                    word = get_word();
                    return createTipoelem(PLUSEQUAL, word); // "+="
                } else if(isspace(letra)){
                    //Aceptación
                    if(letra == '\n'){
                        line++;
                    }
                    prev_carac();
                    word = get_word();
                    return createTipoelem('+', word); // "+"

                } else {
                    //Aceptación con retroceso
                    prev_carac();
                    word = get_word();
                    return createTipoelem('+', word);
                }
                break;
            case EQUALS: //Estado especial que detecta operadores especiales que comienzan por =
                if(letra == '='){
                    //Aceptación especial
                    word = get_word();
                    return createTipoelem(EQUALEQUAL, word); // "=="

                } else if(isspace(letra)){
                    //Aceptación
                    if(letra == '\n'){
                        line++;
                    }
                    prev_carac();
                    word = get_word();
                    return createTipoelem('=', word); // "="
                } else {
                    //Aceptación con retroceso
                    prev_carac();
                    word = get_word();
                    return createTipoelem('=', word); // "="
                }
                break;
        }
    }
}
/*
 * Función básica que es el motor de todo el analizador léxico
 */
tipoelem sig_comp_lexico(tablaSimbolos *TS){
    char letra;
    char * word;
    int type = 0;

    tipoelem comp_devuelto;

    while (1){
        letra = sig_carac();

        while (isspace(letra)){
            if(letra == '\n'){
                line++;
            }

            if(isspace(letra)){
                word = get_word(); //Desplazamos el puntero de inicio para que el lexema no contenga espacios o similares
                free(word);
            }
            letra = sig_carac();
        }

        if(letra == EOF){ //Si se recibe el fin de fichero se acaba
            return createTipoelem(EOF, "eof");
        }

        if(isalpha(letra) || letra == '_'){ //Empieza cadena alfanumérica
            type = 1;
        }
        if(type == 0 && isdigit(letra)){ //Empieza cadena numérica
            type = 2;
        }
        if(type == 0 && _is_comp_lex_invidual(letra)){ //Caracter que se aceptará solo
            type = 5;
        }

        //Si al terminar no ha variado el valor de type respecto al original entonces es un caracter no alfanumérico
        switch (type) {
            case 0:
                if(letra == '\"'){
                    comp_devuelto = autom_string();
                }else{
                    comp_devuelto = autom_operad(letra);
                }

                break;
            case 1:
                comp_devuelto = autom_alfanum(TS);
                break;
            case 2:
                comp_devuelto = autom_num(letra);
                break;
            case 5:
                word = get_word();
                comp_devuelto = createTipoelem(letra, word);
                break;
        }
        if(comp_devuelto.value != comment){
            break;
        }
    }

    return comp_devuelto;

}

