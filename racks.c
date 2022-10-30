#include <stdlib.h>
#include <stdio.h>
#include "racks.h"

racks criaRack(int num){
    racks novoRack;
    novoRack.num = num;
    for(int i=0; i<3; i ++) novoRack.switchs[i] = 0;
    return novoRack;
}

void addSwitch(racks *rack, int sw){
    for(int i=0;i<3;i++){
        if(rack->switchs[i] == 0){
            rack->switchs[i] = sw;
            break;
        }
    }
}

void removerSwitch(racks *rack, int sw){
    for(int i=0;i<3;i++){
        if(rack->switchs[i] == sw){
            rack->switchs[i] = 0;
            break;
        }
    }
}

int quantidadeSwitchs(racks *rack){
    int quantidade = 0;
    for(int i=0;i<3;i++){
        if(rack->switchs[i] != 0) quantidade++;
    }
    return quantidade;
}

void imprimeSwitchs(const char* buf, racks *rack){
    char aux[5];
    for(int i=0;i<3;i++){
        if(rack->switchs[i] != 0){
            sprintf(aux,"0%d ", rack->switchs[i]);
            strcat(buf,aux);
        }
    }
    strcat(buf,"\n");
}

int existeSwitch(racks *rack, int sw){
    for(int i=0;i<3;i++){
        if(rack->switchs[i] == sw) return 1;
    }
    return 0;
}