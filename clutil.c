#include <stdio.h>
#include<stdlib.h>


/**
 * Funcao responsavel por transformar .txt em string, facilitando edicao
 * dos programas.
**/
void readProgram(char * path, char ** str, int * tam){
    FILE * f = fopen (path, "rb");
    char * buffer = 0;
    int length;

    if (f){
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        (*str) = (char *) malloc (length);
    if ((*str)){
        fread ((*str), 1, length, f);
    }
    
    fclose (f);
    (*tam) = length;
    }
}