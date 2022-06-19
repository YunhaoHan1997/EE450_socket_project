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
#define BUFLEN 1024


char name1[BUFLEN];
char name2[BUFLEN];
char transaction_amount[BUFLEN];
int recvLen1;

struct transaction{
    string id;
    string from;
    string to;
};
vector<transaction> transactions;
map<string, int> wallets;


struct sockaddr_in m_tcp_client,m_tcp_monitor, m_udp, client,monitor, serverA, serverB, serverC;
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
    if ( (new_tcp_client_fd = accept(m_tcp_client_fd,(struct sockaddr *) &m_tcp_client, &clientLen)) == -1){
        perror("Error accepting socket");
        exit(EXIT_FAILURE);
    }
}

void acceptFromMonitor(){
    socklen_t clientLen = sizeof(monitor);
    //int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    if ( (new_tcp_monitor_fd = accept(m_tcp_monitor_fd,(struct sockaddr *) &m_tcp_monitor, &clientLen)) == -1){
        perror("Error accepting socket");
        exit(EXIT_FAILURE);
    }
}

void recvFromClient(){
    recvLen1 = recv(new_tcp_client_fd, name1, BUFLEN, 0);
    if (recvLen1 == -1){
        perror("Error receiving message from client");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    name1[recvLen1] = '\0';

    recvLen1 = recv(new_tcp_client_fd, name2, BUFLEN, 0);
    if (recvLen1 == -1){
        perror("Error receiving message from client");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    name2[recvLen1] = '\0';

    recvLen1 = recv(new_tcp_client_fd, transaction_amount, BUFLEN, 0);
    if (recvLen1 == -1){
        perror("Error receiving message from client");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    transaction_amount[recvLen1] = '\0';
}

void setServerABC(){
//    Server A
    serverA.sin_family = AF_INET;
    //AWS Port #
    serverA.sin_port = htons(ServerA_PORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    serverA.sin_addr.s_addr = inet_addr(LOCAL_HOST);

//    Server B
    serverB.sin_family = AF_INET;
    //AWS Port #
    serverB.sin_port = htons(ServerB_PORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    serverB.sin_addr.s_addr = inet_addr(LOCAL_HOST);

    // Server c
    serverC.sin_family = AF_INET;
    //AWS Port #
    serverC.sin_port = htons(ServerC_PORT);
    //AWS IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    serverC.sin_addr.s_addr = inet_addr(LOCAL_HOST);
}

void receiveABC(){

}

int main(){
    init_ClientTCP();
    init_MonitorTCP();
    cout << "The MServer is up and running." << endl;
    init_UDP();
    while(1){
        acceptFromClient();
        cout << "acceptFromClient succ"<<endl;
        recvFromClient();
        cout << "recevfrom client succ"<<endl;
        cout << name1 << endl;
        cout << name2 << endl;
        cout << transaction_amount << endl;

        close(new_tcp_client_fd);
        close(new_tcp_monitor_fd);
    }

}