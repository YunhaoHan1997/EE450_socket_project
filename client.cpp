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
#include <map>

using namespace std;

#define LOCAL_HOST "127.0.0.1" // Host address
#define SERVER_PORT 25161 // TCP Port # of AWS server
#define BUFLEN 1000 // Length of socket stream buffer

struct sockaddr_in m_tcp;
int m_tcp_sockfd;

struct sockaddr_in client;
int client_sockfd;
char msg[BUFLEN];
char money[BUFLEN];
int recvLen1;
map<string, string> wallets;


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

    // get dynamic client port number
    int client_portNum;
    socklen_t len = sizeof(client);
    if (getsockname(m_tcp_sockfd, (struct sockaddr *)&client, &len) == -1){
        perror("Error getting client port number");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
    else
        client_portNum = ntohs(client.sin_port);
    cout << name << " sent a balance enquiry request to the main server." << endl;
}

void sendTransaction(char *sender, char *receiver, char *amount){
    if (send(m_tcp_sockfd, sender, strlen(sender), 0) == -1){
        perror("Error sending data to server socket");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 10000000; i++){}
    if (send(m_tcp_sockfd, receiver, strlen(receiver), 0) == -1){
        perror("Error sending data to server socket");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 10000000; i++){}
    if (send(m_tcp_sockfd, amount, strlen(amount), 0) == -1){
        perror("Error sending data to server socket");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
    // get dynamic client port number
    int client_portNum;
    socklen_t len = sizeof(client);
    if (getsockname(m_tcp_sockfd, (struct sockaddr *)&client, &len) == -1){
        perror("Error getting client port number");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
    else
        client_portNum = ntohs(client.sin_port);

}


void recvMsgFromM(){
    if ( (recvLen1 = recv(m_tcp_sockfd, msg, BUFLEN, 0)) == -1){
        perror("Error receiving message from AWS");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
    msg[recvLen1] = '\0';

}

void recvMoneyFromM(){
    if ( (recvLen1 = recv(m_tcp_sockfd, money, BUFLEN, 0)) == -1){
            perror("Error receiving message from M");
            close(client_sockfd);
            exit(EXIT_FAILURE);
        }
    money[recvLen1] = '\0';
}

int main(int argc, char * argv[]){
    if(argc != 2 and argc != 4){
        cout << "Please check the input format" << endl;
        exit(EXIT_FAILURE);
    }

    init_TCP();
    cout << "The client is up and running."<<endl;

    if(argc == 2){
        // check if Username is letter
        if (!isalpha(*argv[1])){
            cout << "Username must be a letter" << endl;
            exit(EXIT_FAILURE);
        }
        sendCheckInfo(argv[1]);
        recvMsgFromM();
        if(msg[1] == '2'){
            cout<<*argv[1]<<" is not in network"<<endl;
        }else{
            recvMoneyFromM();
            cout<<"The current balance of "<<argv[1]<< " is : "<< money<<" txcoins."<<endl;
        }

    }

    if(argc == 4){
        // check if Username is letter
        if (!isalpha(*argv[1])){
            cout << "Sender must be a letter" << endl;
            exit(EXIT_FAILURE);
        }
        // check if Username is letter
        if (!isalpha(*argv[2])){
            cout << "Receiver must be a letter" << endl;
            exit(EXIT_FAILURE);
        }
        // check if transaction amount is digit
        if (!isdigit(*argv[3])){
            cout << "Transaction amount must be a letter" << endl;
            exit(EXIT_FAILURE);
        }

        sendTransaction(argv[1], argv[2], argv[3]);
        cout<< argv[1]<<" has requested to transfer "<< argv[3]<<" txcoins to "<<argv[2]<<endl;
        recvMsgFromM();
        //if name1 and name2 exist in transacitons
        if(msg[0] == '1'){
            for(int i = 0; i < 10000000; i++){}
            recvMoneyFromM();
            cout<< argv[1]<<" successfully transferred "<< argv[3]<<" txcoins to "<<argv[2]<<endl;
            cout<<"The current balance of "<<argv[1]<< " is : "<< money<<" txcoins."<<endl;
        }
        // msg = 2 means name1 exist, name2 not
        else if(msg[0] == '2'){
            cout << "Unable to proceed with the transaction as "<<argv[2]<<" is not part of the network."<< endl;
        //msg = 3 means name2 exist, name1 not
        }else if(msg[0] == '3'){
            cout << "Unable to proceed with the transaction as "<<argv[1]<<" is not part of the network."<< endl;
        }else if(msg[0] == '4'){
            cout << "Unable to proceed with the transaction as "<<argv[1]<<" and "<< argv[2]<<" are not part of the network."<< endl;
        }else{
            //if msg == 0
            // insufficient
            for(int i = 0; i < 10000000; i++){}
            cout<< argv[1]<<" was unable to transfer " <<argv[3]<< " txcoins to "<<argv[2]<<" because of insufficient balance."<<endl;
            recvMoneyFromM();
            cout<<"The current balance of "<<argv[1]<< " is : "<< money<<" txcoins."<<endl;
        }
    }

    return 0;
}