#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#define BUFSZ 1024

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

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if(bind(s, addr, sizeof(storage))!=0) logexit("bind");

    if(listen(s, 10)!=0) logexit("listen");

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("Conectado a %s\n", addrstr);
    

    char caddrstr[BUFSZ];
    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);

    int csock = accept(s, caddr, sizeof(cstorage));
    if(csock == -1) logexit("accept");

    addrtostr(caddr, caddrstr, BUFSZ);
    printf("[log]Conexão de %s\n", caddrstr);

    while(1){
        memset(buf, 0, BUFSZ);
        count = recv(csock, buf, BUFSZ, 0);
        printf("[msg] %s: %s\n", caddrstr,buf);
        
        //Encerra conexão
        if(strncmp(buf,"Exit", 4)) break;

        sprintf(buf, "Mesagem recebida\n");
        count = send(csock, buf, strlen(buf)+1, 0);
        if(count != strlen(buf)+1) logexit("send");
    }

    sprintf(buf, "Conexão encerrada\n");
    send(csock, buf, strlen(buf)+1, 0);
    close(csock);

    exit(EXIT_SUCCESS);
    return 0;
}