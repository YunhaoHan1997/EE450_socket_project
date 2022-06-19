//
// Created by Yunhao on 6/19/22.
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
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <map>
#include <climits>

using namespace std;
#define LOCAL_HOST "127.0.0.1" // Host address
#define ServerA_PORT 21161
#define BUFLEN 1000 // Length of socket stream buffer

char name1[BUFLEN];
char name2[BUFLEN];
char transaction_amount[BUFLEN];

struct transaction{
    char *name1;
    char *name2;
    char *amount;
};
vector<transaction> transactions;
struct sockaddr_in m_udp, serverA;
int serverA_sockfd;

void init_UDP(){
    // *** 1. CREATE SOCKET ***
    if ( (serverA_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
        perror("Error creating UDP socket");
        exit(EXIT_FAILURE);
    }

    // specify ServerA address

    serverA.sin_family = AF_INET;
    //AWS Port #
    serverA.sin_port = htons(ServerA_PORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    serverA.sin_addr.s_addr = inet_addr(LOCAL_HOST);

    // *** 2. BIND SOCKET ***

    if (::bind(serverA_sockfd, (struct sockaddr *) &serverA, sizeof(serverA)) == -1 ){
        close(serverA_sockfd);
        perror("Error binding UDP socket");
        exit(EXIT_FAILURE);
    }
}

void constructTransactions(){
    std::ifstream fileInput("block1.txt")
}