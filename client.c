#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//Windows não possui "sys/socket.h", esse programa rodará apenas no linux

#define BUFSZ 1024

void usage(int argc, char **argv){
    printf("ERRO NA CHAMADA\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv){
    if (argc < 3) {
		usage(argc, argv);
	}

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		usage(argc, argv);
	}

	int s;
	s = socket(storage.ss_family, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))) {
		logexit("connect");
	}

	char addrstr[BUFSZ];
	addrtostr(addr, addrstr, BUFSZ);

	printf("connected to %s\n", addrstr);

    //Comunicação cliente-servidor
    char buf[BUFSZ];
    unsigned total = 0;
    size_t count = 0;
    while(1){
        memset(buf, 0, BUFSZ);
        printf("[msg]Cliente > ");
        fgets(buf, BUFSZ-1, stdin);
        count = send(s, buf, strlen(buf)+1, 0);
        if(count != strlen(buf)+1) logexit("send");

        memset(buf, 0, BUFSZ);
        total = 0;
        //Loop para receber a mensagem
        //while(1){
        count = recv(s, buf + total, BUFSZ - total, 0);
            //Caso Mensagem encerrada
            //if(count == 0) break;

            //total += count;
        //}
        printf("[msg]Server > %s",buf);

        //Encerra conexão
        if(strncmp(buf,"Conexao Encerrada", 17) == 0){
            break;
        }
    }

    printf("[log] Conexão Encerrada\n");
    close(s);

    exit(EXIT_SUCCESS);
    return 0;
}