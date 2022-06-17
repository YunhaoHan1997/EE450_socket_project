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

struct sockaddr_in m_tcp_client,m_tcp_monitor, m_udp, client, serverA, serverB, serverC;
int m_tcp_client_fd,m_tcp_monitor_fd, m_udp_fd, new_tcp_client_fd, new_tcp_monitor_fd;

// initialize tcp of the Main Server
void init_ClientTCP(){

    // *** 1. CREATE SOCKET ***
    if ( (m_tcp_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating TCP socket");
        exit(EXIT_FAILURE);
    }
    // specify AWS address
    m_tcp_client.sin_family = AF_INET;
    //Server Tcp Port #
    m_tcp_client.sin_port = htons(M_Client_TCP_PORT);
    //Server IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    m_tcp_client.sin_addr.s_addr = inet_addr(LOCAL_HOST);

    // *** 2. BIND SOCKET ***

    if (::bind(m_tcp_client_fd, (struct sockaddr *) &m_tcp_client, sizeof(m_tcp_client)) == -1 ){
        close(m_tcp_client_fd);
        perror("Error binding TCP socket");
        exit(EXIT_FAILURE);
    }
    // *** 3. LISTEN FOR CONNECTIONS ***

    if (listen(m_tcp_client_fd, BACKLOG) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
}
void init_MonitorTCP(){

    // *** 1. CREATE SOCKET ***
    if ( (m_tcp_monitor_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating TCP socket");
        exit(EXIT_FAILURE);
    }
    // specify Server address
    m_tcp_monitor.sin_family = AF_INET;
    //Server Tcp Port #
    m_tcp_monitor.sin_port = htons(M_Monitor_TCP_PORT);
    //Server IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    m_tcp_monitor.sin_addr.s_addr = inet_addr(LOCAL_HOST);

    // *** 2. BIND SOCKET ***

    if (::bind(m_tcp_monitor_fd, (struct sockaddr *) &m_tcp_monitor, sizeof(m_tcp_monitor)) == -1 ){
        close(m_tcp_monitor_fd);
        perror("Error binding TCP socket");
        exit(EXIT_FAILURE);
    }
    // *** 3. LISTEN FOR CONNECTIONS ***

    if (listen(m_tcp_monitor_fd, BACKLOG) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
}
void init_UDP(){
    // *** 1. CREATE SOCKET ***
    if ( (m_udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
        perror("Error creating UDP socket");
        exit(EXIT_FAILURE);
    }

    // specify AWS address

    m_udp.sin_family = AF_INET;
    //AWS Port #
    m_udp.sin_port = htons(M_UDP_PORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    m_udp.sin_addr.s_addr = inet_addr(LOCAL_HOST);

    // *** 2. BIND SOCKET ***

    if (::bind(m_udp_fd, (struct sockaddr *) &m_udp, sizeof(m_udp)) == -1 ){
        close(m_udp_fd);
        perror("Error binding UDP socket");
        exit(EXIT_FAILURE);
    }
    //note: udp does not have listen
}

void acceptFromClient(){
    //clientlen from CMU class notes
    socklen_t clientLen = sizeof(client);
    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    if ( (new_tcp_client_fd = accept(aws_TCP_sockfd,(struct sockaddr *) &clientAddr, &clientLen)) == -1){
        perror("Error accepting socket");
        exit(EXIT_FAILURE);
    }
}

int main(){
    init_ClientTCP();
    init_MonitorTCP();
    init_UDP();

}