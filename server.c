#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#define BUFSZ 1024
#define RACKS 4
#define SWITCHS 3

//Switchs
struct sw
{
    char tipo[2];
};

//Racks
struct sw racks[RACKS][SWITCHS];

void execComando(const char* buf){
    int i = 0;
    char comando[3] = ''; //Obtem o comando
    memset(comando, 0, 3);

    while(buf[i] != ' '){
        comando[i] = buf[i];
        i++;
    }

    printf("[DEBUG] Comando obtido = %s\n", comando);

    if(strcmp(comando,"add") == 0){
        //Instalar 
        memset(buf, 0, BUFSZ);
        sprintf(buf, "Instalar switch\n");
    }
    else if(strcmp(comando,"rm") == 0){
        //Desinstalar
        memset(buf, 0, BUFSZ);
        sprintf(buf, "Desinstalar Switch\n");
    }
    else if(strcmp(comando,"get") == 0){
        //Ler dados de Switch
        memset(buf, 0, BUFSZ);
        sprintf(buf, "Obter dados do switch\n");
    }
    else if(strcmp(comando,"ls") == 0){
        //Listar switchs em rack
        memset(buf, 0, BUFSZ);
        sprintf(buf, "Listar switchs\n");
    }
    else{
        //Nenhum comando identificado
        memset(buf, 0, BUFSZ);
        sprintf(buf, "Comando não reconhecido\n");
    }
}

void usage(int argc, char **argv){
    printf("usage\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char **argv){
    if(argc < 3) usage(argc, argv); //Verificar chamada correta

    char buf[BUFSZ];
    size_t count;

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
        if(strcmp(buf,"Exit\n") == 0) {
            break;
        }

        execComando(buf);

        printf("[msg]Servidor > %s", buf);
        count = send(csock, buf, strlen(buf)+1, 0);
        if(count != strlen(buf)+1) logexit("send");
    }

    sprintf(buf, "Conexao Encerrada\n");
    printf("[log]%s",buf);
    send(csock, buf, strlen(buf)+1, 0);
    close(csock);

    exit(EXIT_SUCCESS);
    return 0;
}