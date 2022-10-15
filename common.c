#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include <arpa/inet.h>

int addrparse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage){
    if(addrstr == NULL || portstr == NULL) return -1;

    uint16_t port = (unit16_t)atoi(portstr); //unsigned short
    if(port == 0) return -1;

    port = htons(port); //host to network short

    struct in_addr inaddr4; //32-bits endereço IPv4
    if(inet_pton(AF_INET, addrstr, &inaddr4)){
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AD_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in_addr inaddr6; //128-bits endereço IPv6
    if(inet_pton(AF_INET6, addrstr, &inaddr6)){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AD_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

void addrtostr(const struct sockaddr *addr, char *str, size_t strsize){
    int versao;
    uint16_t port;
    char addrstr[INET6_ADDRSTRLEN+1] = "";

    if(addr->sa_family == AF_INET){
        versao = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        if(!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSRTLEN+1))
            logexit("ntop");
        port = ntohs(addr4->sin_port);
    }
    else if(addr->sa_family == AF_INET6){
        versao = 6;
        struct sockaddr_in *addr6 = (struct sockaddr_in6 *)addr;
        if(!inet_ntop(AF_INET, &(addr6->sin6_addr), addrstr, INET6_ADDRSRTLEN+1))
            logexit("ntop");
        port = ntohs(addr6->sin6_port);
    }
    else logexit("Protocolo desconhecido\n");
    if(str)
        snprintf(str, strsize "IPv%d %s %hu", versao, addrstr, port);


}