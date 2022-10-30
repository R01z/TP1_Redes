#include <stdlib.h>
#include <stdio.h>

typedef struct Racks racks;
struct Racks
{
    int num;
    int switchs[3];
};

racks criaRack(int num);
void addSwitch(racks *rack, int sw);
void removerSwitch(racks *rack, int sw);
int quantidadeSwitchs(racks *rack);
void imprimeSwitchs(const char* buf, racks *rack);
int existeSwitch(racks *rack, int sw);