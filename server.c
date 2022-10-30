#include "common.h"
#include "racks.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>

#define BUFSZ 1024
#define RACKS 4
#define SWITCHS 3

void setRack(racks vet[RACKS]){
    for(int i=0;i<RACKS;i++){
        vet[i] = criaRack(i+1);
    }
}

int mensagemIncorreta(const char* buf){
    memset(buf, 0, BUFSZ);
    sprintf(buf, "unknow command, ending connection\n");
    return 1;
}

//Função para conferir se string passada é digito, retorna 0 se não for
int isDigit(const char * str){
    if(!str) return 0; //Se string for NULL
    if(!(*str)) return 0; //Se string for vazia
    if(str[0] != '0') return 0; //Garantindo que possui zero a esquerda

    while(*str)
        if(!isdigit(*str++))
            return 0;
    return 1;
}

int execComando(const char* buf, racks rack[RACKS]){
    int i = 0;
    int sw[3] = {0, 0, 0};
    int rackNum = 0;

    char *tokens;

    tokens = strtok(buf, " ");

    if(strcmp(tokens,"add") == 0){
        tokens = strtok(NULL, " ");
        if(strcmp(tokens,"sw") == 0)
        {
            //Instalar
            tokens = strtok(NULL, " ");
            while (strcmp(tokens,"in") != 0)
            {
                //Mais de 3 switchs na mensagem, ou nenhum digito
                if(isDigit(tokens) == 0) return mensagemIncorreta(buf);
                if(i>=3){
                    memset(buf, 0, BUFSZ);
                    sprintf(buf, "error rack limit exceeded\n");
                    return 0;
                }
                sw[i] = atoi(tokens);
                tokens = strtok(NULL, " ");
                i++;
            }
            tokens = strtok(NULL, "\n"); //Pega final da mensagem
            if (isDigit(tokens) == 1) //Obter rack
                rackNum = atoi(tokens);
            else return mensagemIncorreta(buf);
            
            adiconarSwitch(buf, sw, rackNum, i, rack);
            return 0;    
        }
        return mensagemIncorreta(buf);
        
    }
    else if(strcmp(tokens,"rm") == 0){
        //Desinstalar
        tokens = strtok(NULL, " ");
        if(strcmp(tokens,"sw") == 0)
        {
            tokens = strtok(NULL, " ");
            while (strcmp(tokens,"in") != 0)
            {
                //Mais de 1 switchs na mensagem, ou nenhum digito
                if(i>=1 || isDigit(tokens) == 0) return mensagemIncorreta(buf);
                sw[i] = atoi(tokens);
                tokens = strtok(NULL, " ");
                i++;
            }
            tokens = strtok(NULL, "\n"); //Pega final da mensagem
            if (isDigit(tokens) == 1) //Obter rack
                rackNum = atoi(tokens);
            else return mensagemIncorreta(buf);
            
            rmSwitch(buf, sw[0], rackNum, rack);
            return 0;    
        }
        return mensagemIncorreta(buf);
    }
    else if(strcmp(tokens,"get") == 0){
        //Ler dados de Switch
        tokens = strtok(NULL, " ");
        while (strcmp(tokens,"in") != 0)
        {
            //Mais de 2 switchs na mensagem, ou nenhum digito
            if(i>=2 || isDigit(tokens) == 0) return mensagemIncorreta(buf);
            sw[i] = atoi(tokens);
            tokens = strtok(NULL, " ");
            i++;
        }
        tokens = strtok(NULL, "\n"); //Pega final da mensagem
        if (isDigit(tokens) == 1) //Obter rack
            rackNum = atoi(tokens);
        else return mensagemIncorreta(buf);
        
        getSwitchs(buf, sw, i, rackNum, rack);
        return 0;
    }
    else if(strcmp(tokens,"ls") == 0){
        //Listar switchs em rack
        tokens = strtok(NULL, "\n");
        if (isDigit(tokens) == 1) //Obter rack
            rackNum = atoi(tokens);
        else return mensagemIncorreta(buf);

        lsSwitch(buf, rackNum, rack);
        return 0;
    }
    else{
        //Nenhum comando identificado
        return mensagemIncorreta(buf);
    }
}

void adiconarSwitch(const char* buf, int sw[], int rackNum, int quant, racks rack[RACKS]){
    int i = 0;
    char str[BUFSZ];
    memset(buf, 0, BUFSZ);

    //Checagem de existência de rack
    if(rackNum>4 || rackNum<1){
        sprintf(buf, "error rack doesn't exist\n");
        return;
    }

    //Confere se todos os switchs para instalação existem 
    for(i=0;i<quant;i++){
        if(sw[i] > 4 || sw[i] < 1){
            sprintf(buf, "error switch type unknown\n");
            return;
        }
    }

    //Checagem do limite de switchs para instalação
    if(quantidadeSwitchs(&rack[rackNum-1]) + quant > 3){
        sprintf(buf, "error rack limit exceeded\n");
        return;
    }

    //Verificar se switch já foi instalado no rack
    for(i=0;i<quant;i++){
        if(existeSwitch(&rack[rackNum-1],sw[i]) == 1){
            sprintf(buf, "error switch 0%d already installed in 0%d\n",sw[i],rackNum);
            return;
        }
    }

    //Instala os switchs
    sprintf(buf, "switch ");
    for(i=0;i<quant;i++){
        addSwitch(&rack[rackNum-1],sw[i]);
        sprintf(str, "0%d ",sw[i]);
        strcat(buf,str);
    }
    strcat(buf, "installed\n");
    return;
}

void rmSwitch(const char* buf, int sw, int rackNum, racks rack[RACKS]){
    int i = 0;
    memset(buf, 0, BUFSZ);

    //Checagem de existência de rack
    if(rackNum>4 || rackNum<1){
        sprintf(buf, "error rack doesn't exist\n");
        return;
    }

    //Checagem se existem switchs para remoção
    if(existeSwitch(&rack[rackNum-1],sw) == 0){
        sprintf(buf, "error switch doesn't exist\n");
        return;
    }

    //Remove Switch
    removerSwitch(&rack[rackNum-1],sw);
    sprintf(buf, "switch 0%d removed from 0%d\n", sw, rackNum);
    return;
}

void getSwitchs(const char* buf, int sw[], int quant, int rackNum, racks rack[RACKS]){
    int i = 0;
    char str[BUFSZ];
    memset(buf, 0, BUFSZ);

    //Checagem de existência de rack
    if(rackNum>4 || rackNum<1){
        sprintf(buf, "error rack doesn't exist\n");
        return;
    }

    //Conferir se switchs existem
    for(i=0;i<quant;i++){
        if(existeSwitch(&rack[rackNum-1],sw[i]) == 0){
            sprintf(buf, "error switch doesn't exist\n");
            return;
        }
    }

    //Obtem dados do switch
    srand(time(NULL));
    for(i=0;i<quant;i++){
        sprintf(str, "%d Kbs ",(rand() % 10000000)+1);
        strcat(buf,str);
    }
    strcat(buf,"\n");
    return;
}

void lsSwitch(const char* buf, int rackNum, racks rack[RACKS]){
    memset(buf, 0, BUFSZ);

    //Checagem de existência de rack
    if(rackNum>4 || rackNum<1){
        sprintf(buf, "error rack doesn't exist\n");
        return;
    }

    if(quantidadeSwitchs(&rack[rackNum-1]) == 0){
        sprintf(buf, "empty rack\n");
        return; 
    }

    //Obter switchs
    imprimeSwitchs(buf, &rack[rackNum-1]);
    return;
}

void usage(int argc, char **argv){
    printf("usage\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char **argv){
    if(argc < 3) usage(argc, argv); //Verificar chamada correta

    char buf[BUFSZ];
    size_t count;
    racks rack[RACKS];
    setRack(rack);

    //Chamada do connect
    struct sockaddr_storage storage;
    if(server_sockaddr_init(argv[1], argv[2], &storage) !=0) usage(argc, argv);

    //Criar Socket
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if(s == -1) logexit("socket");

    int enable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(bind(s, addr, sizeof(storage))!=0) logexit("bind");

    if(listen(s, 10)!=0) logexit("listen");

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("[log]Conectado a %s, aguardando conexões\n", addrstr);
    
    char caddrstr[BUFSZ];
    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    int csock = accept(s, caddr, &caddrlen);
    if(csock == -1) logexit("accept");

    addrtostr(caddr, caddrstr, BUFSZ);
    printf("[log]Conexão de %s\n", caddrstr);
    while(1){
        memset(buf, 0, BUFSZ);
        count = recv(csock, buf, BUFSZ, 0);
        printf("[msg]Cliente > %s", buf);
        
        //Encerra conexão
        if(strcmp(buf,"exit\n") == 0) {
            sprintf(buf, "Conexao Encerrada\n");
            break;
        }

        int acao = execComando(buf,rack);

        if(acao != 0)
            break;

        printf("[msg]Servidor > %s", buf);
        count = send(csock, buf, strlen(buf)+1, 0);
        if(count != strlen(buf)+1) logexit("send");
    }

    send(csock, buf, strlen(buf)+1, 0);
    printf("[log]%s",buf);
    close(csock);

    exit(EXIT_SUCCESS);
    return 0;
}