//
// Created by Yunhao on 7/4/22.
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
#include <map>
#include <fstream>

using namespace std;

#define LOCAL_HOST "127.0.0.1" // Host address
#define SERVER_PORT 26161 // TCP Port # of M server
#define BUFLEN 1000 // Length of socket stream buffer
struct sockaddr_in m_tcp;
int m_tcp_sockfd;

struct sockaddr_in monitor;
int monitor_sockfd;

struct transaction{
    string id;
    string from;
    string to;
    string amount;
};

vector<transaction> transactions;

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

void sendInfo(char *name){
    if (send(m_tcp_sockfd, name, strlen(name), 0) == -1){
        perror("Error sending data to server socket");
        close(monitor_sockfd);
        exit(EXIT_FAILURE);
    }

    // get dynamic client port number
    int client_portNum;
    socklen_t len = sizeof(monitor);
    if (getsockname(m_tcp_sockfd, (struct sockaddr *)&monitor, &len) == -1){
        perror("Error getting client port number");
        close(monitor_sockfd);
        exit(EXIT_FAILURE);
    }
    else
        client_portNum = ntohs(monitor.sin_port);
    cout << "Monitor sent a sorted list request to the main server." << endl;
}

void buildtxt(vector<transaction> &trans){
    ofstream write;
    ofstream file_writer("txchain.txt", ios_base::out);
    write.open("txchain.txt",ios::app);
    for(transaction transaction: trans){
        write<< transaction.id << " " << transaction.from << " " << transaction.to << " "<< transaction.amount<<endl;
    }

}

void receiveTransaction(){
    char id[BUFLEN];
    char from[BUFLEN];
    char to[BUFLEN];
    char amount[BUFLEN];
    int recvLen1;
    int recvDone = 0;
    while(!recvDone){
        if ( (recvLen1 = recv(m_tcp_sockfd, id, BUFLEN, 0)) == -1){
            perror("Error receiving message from M");
            close(m_tcp_sockfd);
            exit(EXIT_FAILURE);
        }
        id[recvLen1] = '\0';

        if(id[0] != '\0'){
            if ( (recvLen1 = recv(m_tcp_sockfd, from, BUFLEN, 0)) == -1){
                perror("Error receiving message from M");
                close(m_tcp_sockfd);
                exit(EXIT_FAILURE);
            }
            from[recvLen1] = '\0';

            if ( (recvLen1 = recv(m_tcp_sockfd, to, BUFLEN, 0)) == -1){
                perror("Error receiving message from M");
                close(m_tcp_sockfd);
                exit(EXIT_FAILURE);
            }
            to[recvLen1] = '\0';

            if ( (recvLen1 = recv(m_tcp_sockfd, amount, BUFLEN, 0)) == -1){
                perror("Error receiving message from AWS");
                close(m_tcp_sockfd);
                exit(EXIT_FAILURE);
            }
            amount[recvLen1] = '\0';
            transactions.push_back({id,from,to,amount});

        }
        else{
            recvDone = 1;
        }
    }
    cout<<"recv succ"<<endl;
    buildtxt(transactions);
}

int main(int argc, char * argv[]){
    if(argc != 2){
        cout << "Please check the input format" << endl;
        exit(EXIT_FAILURE);
    }
    init_TCP();
    cout<<"The monitor is up and running."<<endl;
    if(strcmp(argv[1], "TXLIST") != 0){
        cout << "Please check the input format" << endl;
        exit(EXIT_FAILURE);
    }
    sendInfo(argv[1]);
    receiveTransaction();
    close(monitor_sockfd);

}