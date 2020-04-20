#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
    int i = 1;
    for(;i < 256; i++){
        printf("\e[38;5;%dm%d\e[0m\t",i,i);
        if(i && 0 == i%10){
            printf("\n");
        }
    }
    printf("\n");
    return 0;
}
