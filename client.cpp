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
#define BUFLEN 1024 // Length of socket stream buffer

struct sockaddr_in m_tcp;
int m_tcp_sockfd;

struct sockaddr_in client;
int client_sockfd;
char msg[BUFLEN];
char money[BUFLEN];
char flag[BUFLEN];
int recvLen1;
map<string, string> wallets;

struct RequestInfo{
    char name1[BUFLEN];
    char name2[BUFLEN];
    int amount;
};

struct ResponseInfo{
    int flag;
    int amount;
};

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

void sendFlag(int f){
    sprintf(flag,"%d",f);
    if (send(m_tcp_sockfd, flag, strlen(flag), 0) == -1){
        perror("Error sending data to server socket");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
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

void recvFlagFromM(){
    char buf[BUFLEN];
    if ( (recvLen1 = recv(m_tcp_sockfd, buf, BUFLEN, 0)) == -1){
        perror("Error receiving message from AWS");
        close(client_sockfd);
        exit(EXIT_FAILURE);
    }
    buf[recvLen1] = '\0';
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

void show(RequestInfo &requestInfo,ResponseInfo &responseInfo){
    switch(responseInfo.flag){
        case 0:
            cout<<"The current balance of "<<requestInfo.name1<< " is : "<< responseInfo.amount<<" txcoins."<<endl;
            break;
        case 1:
            cout<<"Unable to proceed with checking balance as "<<requestInfo.name1<<" is not in network"<<endl;
            break;
        case 2:
        {
            cout<< requestInfo.name1<<" successfully transferred "<< requestInfo.amount<<" txcoins to "<<requestInfo.name2<<endl;
            cout<<"The current balance of "<<requestInfo.name1<< " is : "<< responseInfo.amount<<" txcoins."<<endl;
            break;
        }
        default:
            cout<<"haha"<<endl;
            break;
    }
}

int main(int argc, char * argv[]){
    if(argc != 2 and argc != 4){
        cout << "Please check the input format" << endl;
        exit(EXIT_FAILURE);
    }

    init_TCP();
    cout << "The client is up and running."<<endl;

//    if(argc == 2){
//        // check if Username is letter
//        if (!isalpha(*argv[1])){
//            cout << "Username must be a letter" << endl;
//            exit(EXIT_FAILURE);
//        }
//
//        //send 0 means check
//        sendFlag(0);
//        recvFlagFromM();
//        for(int i = 0; i < 10000000; i++){}
//        sendCheckInfo(argv[1]);
//
//        recvMsgFromM();
//
//        if(msg[0] == '2'){
//            cout<<"Unable to proceed with checking balance as "<<argv[1]<<" is not in network"<<endl;
//        }else{
//            for(int i = 0; i < 100000000; i++){}
//            recvMoneyFromM();
//            cout<<"The current balance of "<<argv[1]<< " is : "<< money<<" txcoins."<<endl;
//        }
//
//    }

//    if(argc == 4){
//        // check if Username is letter
//        if (!isalpha(*argv[1])){
//            cout << "Sender must be a letter" << endl;
//            exit(EXIT_FAILURE);
//        }
//        // check if Username is letter
//        if (!isalpha(*argv[2])){
//            cout << "Receiver must be a letter" << endl;
//            exit(EXIT_FAILURE);
//        }
//        // check if transaction amount is digit
//        if (!isdigit(*argv[3])){
//            cout << "Transaction amount must be a letter" << endl;
//            exit(EXIT_FAILURE);
//        }
//
//        //send 1 means transfer
//        sendFlag(1);
//        recvFlagFromM();
//        for(int i = 0; i < 100000000; i++){}
//        sendTransaction(argv[1], argv[2], argv[3]);
//        cout<< argv[1]<<" has requested to transfer "<< argv[3]<<" txcoins to "<<argv[2]<<endl;
//        for(int i = 0; i < 10000000; i++){}
//        recvMsgFromM();
//        for(int i = 0; i < 100000000; i++){}
//        //if name1 and name2 exist in transacitons
//        if(msg[0] == '1'){
//            for(int i = 0; i < 10000000; i++){}
//            recvMoneyFromM();
//            cout<< argv[1]<<" successfully transferred "<< argv[3]<<" txcoins to "<<argv[2]<<endl;
//            cout<<"The current balance of "<<argv[1]<< " is : "<< money<<" txcoins."<<endl;
//        }
//        // msg = 2 means name1 exist, name2 not
//        else if(msg[0] == '2'){
//            cout << "Unable to proceed with the transaction as "<<argv[2]<<" is not part of the network."<< endl;
//        //msg = 3 means name2 exist, name1 not
//        }else if(msg[0] == '3'){
//            cout << "Unable to proceed with the transaction as "<<argv[1]<<" is not part of the network."<< endl;
//        }else if(msg[0] == '4'){
//            cout << "Unable to proceed with the transaction as "<<argv[1]<<" and "<< argv[2]<<" are not part of the network."<< endl;
//        }else{
//            //if msg == 0
//            // insufficient
//            for(int i = 0; i < 1000000; i++){}
//            cout<< argv[1]<<" was unable to transfer " <<argv[3]<< " txcoins to "<<argv[2]<<" because of insufficient balance."<<endl;
//            recvMoneyFromM();
//            cout<<"The current balance of "<<argv[1]<< " is : "<< money<<" txcoins."<<endl;
//        }
//    }
    RequestInfo request;
    RequestInfo temp_request;
    ResponseInfo response;
    if(argc == 2){
        strcpy(request.name1, argv[1]);
        request.name2[0] = '\0';
        request.amount = 0;
        char send_buf[1024];
        memset(send_buf, 0, 1024);
        memcpy(send_buf, &request, sizeof(request));
        memcpy(&temp_request, &request, sizeof (request));
        if(send(m_tcp_sockfd, send_buf, sizeof (send_buf), 0) == -1){
            perror("Message Send Error");
            close(m_tcp_sockfd);
            exit(1);
        }
        cout << request.name1 << " sent a balance enquiry request to the main server." << endl;
    } else if(argc == 4){
        //check if Username is letter
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
        strcpy(request.name1, argv[1]);
        strcpy(request.name2, argv[2]);
//        strncpy(request.name1, argv[1], sizeof (argv[1]));
//        strncpy(request.name2, argv[2], sizeof (argv[2]));
        request.amount = stoi(argv[3]);
//        char send_buf[1024];
//        memset(send_buf, 0, 1024);
//        memcpy(send_buf, &request, sizeof(send_buf));
//        memcpy(&temp_request, &request, sizeof (request));
        if(send(m_tcp_sockfd, &request, sizeof (request), 0) == -1){
            perror("Message Send Error");
            close(m_tcp_sockfd);
            exit(1);
        }
    }
    char recv_buf[1024];
    memset(recv_buf, 'z', 1024);
    if (recv(m_tcp_sockfd, recv_buf, sizeof recv_buf, 0) == -1) {
        perror("Receive Error");
        close(m_tcp_sockfd);
        exit(1);
    }
    memset(&response, 0, sizeof response);
    memcpy(&response, recv_buf, sizeof response);

    show(temp_request,response);
    close(client_sockfd);
    return 0;
}