#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
//Windows não possui "sys/socket.h", esse programa rodará apenas no linux

#define BUFSZ 1024

void usage(int argc, char **argv){
    printf("ERRO NA CHAMADA\n");
    exit(EXIT_FAILURE);
}

void logexit(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    if(argc < 3) usage(argc, argv); //Verificar chamada correta

    int count=0;

    //Criar Socket
    int s;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1) logexit("socket");

    //Chamada do connect
    struct sockaddr_storage storage;
    if(addrparse(argv[1], argv[2], &storage) !=0) usage(argc, argv);

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(connect(s, addr, sizeof(storage)) != 0) logexit("connect");

    //Imprimir endereço conectado
    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("Conectado a %s\n");

    //Comunicação cliente-servidor
    char buf[BUFSZ];
    unsigned total = 0;
    while(1){
        memset(buf, 0, BUFSZ);
        printf("Cliente > ");
        fgets(buf, BUFSZ-1, stdin);
        count = send(s, buf, strlen(buf)+1, 0);
        if(count != strlen(buf)+1) logexit("send");

        //Loop para receber a mensagem
        while(1){
            count = recv(s, buf + total, BUFSZ - total, 0);
            //Caso Mensagem encerrada
            if(count == 0) break;

            total += count;
        }
        total = 0;
        puts(buf);

        //Encerra conexão
        if(strncmp(buf,"Conexao Encerrada", 17)) break;;
    }

    close(s);

    exit(EXIT_SUCCESS);
    return 0;
}