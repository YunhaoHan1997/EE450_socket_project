//
// Created by Yunhao on 6/17/22.
//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iomanip>
#include <ctype.h>
#include <vector>

using namespace std;

#define LOCAL_HOST "127.0.0.1" // Host address
#define SERVER_PORT 25161 // TCP Port # of AWS server
#define BUFLEN 1000 // Length of socket stream buffer

struct sockaddr_in m_tcp;
int m_tcp_sockfd;

struct sockaddr_in client;
int client_sockfd;

void init_TCP(){
    // *** CREATE SOCKET ***

    m_tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // *** CONNECT SOCKET ***

    // specify Server address

    m_tcp.sin_family = AF_INET;
    //AWS Port #
    m_tcp.sin_port = htons(SERVER_PORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    //m_tcp.sin_addr.s_addr = htonl(INADDR_LOOPBACK) ;
    m_tcp.sin_addr.s_addr = inet_addr(LOCAL_HOST);

    /*
     //DEBUG: check IP
     char *ip = inet_ntoa(m_tcp.sin_addr);
     printf("Debug: Ip Address -> %s\n", ip);
     */

    if ( connect(m_tcp_sockfd, (struct sockaddr *) &m_tcp, sizeof(m_tcp)) == -1){
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }
}

void sendCheckInfo(char *name){
    if (send(m_tcp_sockfd, name, strlen(name), 0) == -1){
        perror("Error sending data to server socket");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
}

void recvFromM(){

}

int main(int argc, char * argv[]){
    init_TCP();
    if(argc == 1){
        sendCheckInfo(argv[0]);
    }
    return 0;
}