#include "clutil.c"

int main(){

    char * buffer;
    int tamanho; 

    readProgram("blur.cl", &buffer, &tamanho);
    printf("leu esta merda aqui");
    printf("tamanho: %d\n", tamanho);

    printf("conteúdo:\n %s\n", buffer);


}