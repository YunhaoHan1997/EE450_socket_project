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
#include <vector>
#include <iomanip>
#include <map>

using namespace std;

#define LOCAL_HOST "127.0.0.1" // Host address
#define M_UDP_PORT 24161 // AWS port number
#define M_Client_TCP_PORT 25161 // AWS port number
#define M_Monitor_TCP_PORT 26161 // AWS port number
#define ServerA_PORT 21161
#define ServerB_PORT 22161
#define ServerC_PORT 23161
#define MAXDATASIZE 1024 // max number of bytes we can get at once
#define BACKLOG 10 // max number of connections allowed on the incoming queue
#define FAIL -1


struct transaction{
    string id;
    string from;
    string to;
};
vector<transaction> transactions;
map<string, int> wallets;

struct sockaddr_in MAddrTCPClient,MAddrTCPMonitor, MAddrUDP, clientAddr, serverAAddr, serverBAddr, serverCAddr;
int M_TCPClient_sockfd,M_TCPMonitor_sockfd, M_UDP_sockfd, new_aws_TCP_sockfd;

// initialize tcp of the Main Server
void init_ClientTCP(){

    // *** 1. CREATE SOCKET ***
    if ( (M_TCPClient_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating TCP socket");
        exit(EXIT_FAILURE);
    }
    // specify AWS address
    MAddrTCPClient.sin_family = AF_INET;
    //Server Tcp Port #
    MAddrTCPClient.sin_port = htons(M_Client_TCP_PORT);
    //Server IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    MAddrTCPClient.sin_addr.s_addr = inet_addr(LOCAL_HOST);

    // *** 2. BIND SOCKET ***

    if (::bind(M_TCPClient_sockfd, (struct sockaddr *) &MAddrTCPClient, sizeof(MAddrTCPClient)) == -1 ){
        close(M_TCPClient_sockfd);
        perror("Error binding TCP socket");
        exit(EXIT_FAILURE);
    }
    // *** 3. LISTEN FOR CONNECTIONS ***

    if (listen(M_TCPClient_sockfd, BACKLOG) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
}
void init_MonitorTCP(){

    // *** 1. CREATE SOCKET ***
    if ( (M_TCPMonitor_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating TCP socket");
        exit(EXIT_FAILURE);
    }
    // specify Server address
    MAddrTCPMonitor.sin_family = AF_INET;
    //Server Tcp Port #
    MAddrTCPMonitor.sin_port = htons(M_Monitor_TCP_PORT);
    //Server IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    MAddrTCPMonitor.sin_addr.s_addr = inet_addr(LOCAL_HOST);

    // *** 2. BIND SOCKET ***

    if (::bind(M_TCPClient_sockfd, (struct sockaddr *) &MAddrTCPClient, sizeof(MAddrTCPClient)) == -1 ){
        close(M_TCPClient_sockfd);
        perror("Error binding TCP socket");
        exit(EXIT_FAILURE);
    }
    // *** 3. LISTEN FOR CONNECTIONS ***

    if (listen(M_TCPClient_sockfd, BACKLOG) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

int main(){
    void init_TCP();
}