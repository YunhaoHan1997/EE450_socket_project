#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctime>
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
#define MAX_NUM_OF_TRANSACTIONS_BIT 5

char id[BUFLEN];
char name1[BUFLEN];
char name2[BUFLEN];
char transaction_amount[BUFLEN];
int recvLen1, sendLen;

char receive_monitor[BUFLEN];
char flag2[BUFLEN];

fd_set readfds;
struct timeval tv;
int n;
int rv;

struct transaction{
    string id;
    string from;
    string to;
    string amount;
};

struct RequestInfo{
    char name1[BUFLEN];
    char name2[BUFLEN];
    int amount;
};

struct ResponseInfo{
    int flag;
    int amount;
};

vector<transaction> transactions;
map<string, int> wallets;
RequestInfo request;
ResponseInfo response;

struct sockaddr_in m_tcp_client,m_tcp_monitor, m_udp, client,monitor, serverA, serverB, serverC;
int m_tcp_client_fd,m_tcp_monitor_fd, m_udp_fd, new_tcp_client_fd, new_tcp_monitor_fd;

// initialize tcp of the Main Server
void init_ClientTCP(){

    // *** 1. CREATE SOCKET ***
    if ( (m_tcp_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating TCP socket");
        exit(EXIT_FAILURE);
    }
    memset(&m_tcp_client, 0, sizeof (m_tcp_client));
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
    memset(&m_tcp_monitor, 0, sizeof (m_tcp_monitor));
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
    memset(&m_udp, 0, sizeof (m_udp));
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

void recvFlagFromClient(){
    recvLen1 = recv(new_tcp_client_fd, flag2, BUFLEN, 0);
    if (recvLen1 == -1){
        perror("Error receiving message from client");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    flag2[recvLen1] = '\0';
}

void recvTransactionFromClient(){
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

void recvCheckWalletFromClient(){
    recvLen1 = recv(new_tcp_client_fd, name1, BUFLEN, 0);
    if (recvLen1 == -1){
        perror("Error receiving message from client");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    name1[recvLen1] = '\0';
}

void recvFromMonitor(){
    recvLen1 = recv(new_tcp_monitor_fd, receive_monitor, BUFLEN, 0);
    if (recvLen1 == -1){
        perror("Error receiving message from client");
        close(new_tcp_monitor_fd);
        exit(EXIT_FAILURE);
    }
    receive_monitor[recvLen1] = '\0';
    cout << "The main server received a sorted list request from the monitor using TCP over port 26161."<<endl;
}

void sendToA(char *id, char *from, char *to, char *amount){
    cout << "The main server sent a request to server A"<<endl;

    if ((sendLen = sendto(m_udp_fd, id, strlen(id), 0, (struct sockaddr *) &serverA, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server A from AWS");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    if ((sendLen = sendto(m_udp_fd, from, strlen(from), 0, (struct sockaddr *) &serverA, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server A from AWS");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    if ((sendLen = sendto(m_udp_fd, to, strlen(to), 0, (struct sockaddr *) &serverA, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server A from AWS");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    if ((sendLen = sendto(m_udp_fd, amount, strlen(amount), 0, (struct sockaddr *) &serverA, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server A from AWS");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }

    cout << "The main server received the feedback from server A using UDP over port 24161." << endl;
}

void sendToB(char *id, char *from, char *to, char *amount){

    cout << "The main server sent a request to server B"<<endl;

    if ((sendLen = sendto(m_udp_fd, id, strlen(id), 0, (struct sockaddr *) &serverB, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server B from M");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    if ((sendLen = sendto(m_udp_fd, from, strlen(from), 0, (struct sockaddr *) &serverB, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server B from M");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    if ((sendLen = sendto(m_udp_fd, to, strlen(to), 0, (struct sockaddr *) &serverB, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server B from M");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    if ((sendLen = sendto(m_udp_fd, amount, strlen(amount), 0, (struct sockaddr *) &serverB, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server B from M");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }

    cout << "The main server received the feedback from server B using UDP over port 24161." << endl;
}

void sendToC(char *id, char *from, char *to, char *amount){

    cout << "The main server sent a request to server C"<<endl;

    if ((sendLen = sendto(m_udp_fd, id, strlen(id), 0, (struct sockaddr *) &serverC, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server C from AWS");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    if ((sendLen = sendto(m_udp_fd, from, strlen(from), 0, (struct sockaddr *) &serverC, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server C from AWS");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    if ((sendLen = sendto(m_udp_fd, to, strlen(to), 0, (struct sockaddr *) &serverC, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server C from AWS");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
    if ((sendLen = sendto(m_udp_fd, amount, strlen(amount), 0, (struct sockaddr *) &serverC, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message to Server C from AWS");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }

    cout << "The main server received the feedback from server C using UDP over port 24161." << endl;
}

void receiveFromA(){

    char recv_id[BUFLEN];
    char recv_name1[BUFLEN];
    char recv_name2[BUFLEN];
    char recv_amount[BUFLEN];
    int recvDone = 0; // 0 = not finished receiving, 1 = finished receiving
    socklen_t serverALen = sizeof(serverA);
    cout << "The main server sent a request to server A."<<endl;
    while(! recvDone){

        struct transaction temp;
        if ((recvLen1 = recvfrom(m_udp_fd, recv_id, BUFLEN, 0, (struct sockaddr *) &serverA, &serverALen )) < 0){
            perror("Error receiving message from Server A");
            close(new_tcp_client_fd);
            exit(EXIT_FAILURE);
        }
        recv_id[recvLen1] = '\0';
        temp.id = string(recv_id);

        if (recv_id[0] != '\0'){
            if ((recvLen1 = recvfrom(m_udp_fd, recv_name1, BUFLEN, 0, (struct sockaddr *) &serverA, &serverALen )) < 0){
                perror("Error receiving message from Server A");
                close(new_tcp_client_fd);
                exit(EXIT_FAILURE);
            }
            recv_name1[recvLen1] = '\0';
            temp.from = string(recv_name1);

            if ((recvLen1 = recvfrom(m_udp_fd, recv_name2, BUFLEN, 0, (struct sockaddr *) &serverA, &serverALen )) < 0){
                perror("Error receiving message from Server A");
                close(new_tcp_client_fd);
                exit(EXIT_FAILURE);
            }
            recv_name2[recvLen1] = '\0';
            temp.to = string(recv_name2);

            //receive amount
            if ((recvLen1 = recvfrom(m_udp_fd,recv_amount , BUFLEN, 0, (struct sockaddr *) &serverA, &serverALen )) < 0){
                perror("Error receiving message from Server A");
                close(new_tcp_client_fd);
                exit(EXIT_FAILURE);
            }
            recv_amount[recvLen1] = '\0';
            temp.amount = string(recv_amount);
            transactions.push_back(temp);
        }else{
            //toggle recvDone
            recvDone = 1;
        }
    }

    cout << "The main server received transactions from Server A using UDP over port 21161." << endl;
}

void receiveFromB(){

    char recv_id[BUFLEN];
    char recv_name1[BUFLEN];
    char recv_name2[BUFLEN];
    char recv_amount[BUFLEN];
    int recvDone = 0; // 0 = not finished receiving, 1 = finished receiving
    socklen_t serverBLen = sizeof(serverB);
    cout << "The main server sent a request to server B."<<endl;
    while(! recvDone){

        struct transaction temp;
        if ((recvLen1 = recvfrom(m_udp_fd, recv_id, BUFLEN, 0, (struct sockaddr *) &serverB, &serverBLen )) < 0){
            perror("Error receiving message from Server A");
            close(new_tcp_client_fd);
            exit(EXIT_FAILURE);
        }
        recv_id[recvLen1] = '\0';
        temp.id = string(recv_id);

        if (recv_id[0] != '\0'){
            if ((recvLen1 = recvfrom(m_udp_fd, recv_name1, BUFLEN, 0, (struct sockaddr *) &serverB, &serverBLen )) < 0){
                perror("Error receiving message from Server B");
                close(new_tcp_client_fd);
                exit(EXIT_FAILURE);
            }
            recv_name1[recvLen1] = '\0';
            temp.from = string(recv_name1);

            if ((recvLen1 = recvfrom(m_udp_fd, recv_name2, BUFLEN, 0, (struct sockaddr *) &serverB, &serverBLen )) < 0){
                perror("Error receiving message from Server A");
                close(new_tcp_client_fd);
                exit(EXIT_FAILURE);
            }
            recv_name2[recvLen1] = '\0';
            temp.to = string(recv_name2);

            //receive amount
            if ((recvLen1 = recvfrom(m_udp_fd,recv_amount , BUFLEN, 0, (struct sockaddr *) &serverB, &serverBLen )) < 0){
                perror("Error receiving message from Server B");
                close(new_tcp_client_fd);
                exit(EXIT_FAILURE);
            }
            recv_amount[recvLen1] = '\0';
            temp.amount = string(recv_amount);
            transactions.push_back(temp);
        }else{
            //toggle recvDone
            recvDone = 1;
        }
    }

    cout << "The main server received transactions from Server B using UDP over port 22161." << endl;
}

void receiveFromC(){

    char recv_id[BUFLEN];
    char recv_name1[BUFLEN];
    char recv_name2[BUFLEN];
    char recv_amount[BUFLEN];
    int recvDone = 0; // 0 = not finished receiving, 1 = finished receiving
    socklen_t serverCLen = sizeof(serverC);
    cout << "The main server sent a request to server C."<<endl;
    while(! recvDone){

        struct transaction temp;
        if ((recvLen1 = recvfrom(m_udp_fd, recv_id, BUFLEN, 0, (struct sockaddr *) &serverC, &serverCLen )) < 0){
            perror("Error receiving message from Server C");
            close(new_tcp_client_fd);
            exit(EXIT_FAILURE);
        }
        recv_id[recvLen1] = '\0';
        temp.id = string(recv_id);

        if (recv_id[0] != '\0'){
            if ((recvLen1 = recvfrom(m_udp_fd, recv_name1, BUFLEN, 0, (struct sockaddr *) &serverC, &serverCLen )) < 0){
                perror("Error receiving message from Server C");
                close(new_tcp_client_fd);
                exit(EXIT_FAILURE);
            }
            recv_name1[recvLen1] = '\0';
            temp.from = string(recv_name1);

            if ((recvLen1 = recvfrom(m_udp_fd, recv_name2, BUFLEN, 0, (struct sockaddr *) &serverC, &serverCLen )) < 0){
                perror("Error receiving message from Server C");
                close(new_tcp_client_fd);
                exit(EXIT_FAILURE);
            }
            recv_name2[recvLen1] = '\0';
            temp.to = string(recv_name2);

            //receive amount
            if ((recvLen1 = recvfrom(m_udp_fd,recv_amount , BUFLEN, 0, (struct sockaddr *) &serverC, &serverCLen )) < 0){
                perror("Error receiving message from Server A");
                close(new_tcp_client_fd);
                exit(EXIT_FAILURE);
            }
            recv_amount[recvLen1] = '\0';
            temp.amount = string(recv_amount);
            transactions.push_back(temp);
        }else{
            //toggle recvDone
            recvDone = 1;
        }
    }

    cout << "The main server received transactions from Server C using UDP over port 23161." << endl;
}

void build_wallets(){
    //clean wallets
    wallets.clear();
    for(auto transaction: transactions){
        if(wallets.find(transaction.from) == wallets.end()){
            wallets.insert({transaction.from, 1000});
        }
        if(wallets.find(transaction.to) == wallets.end()){
            wallets.insert({transaction.to, 1000});
        }
    }

    for(const auto& transaction: transactions){
        auto it1 = wallets.find(transaction.from);
        it1->second -= stoi(transaction.amount);

        auto it2 = wallets.find(transaction.to);
        it2->second += stoi(transaction.amount);

    }
}

void sendFlagToClient(){
    if(send(new_tcp_client_fd, "1", strlen("1"), 0) == -1){
        perror("Error sending data to client");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
}

void sendWalletToClient(string target_name){
    char money[BUFLEN];
    sprintf(money, "%d", wallets.find(target_name)->second);
    if(send(new_tcp_client_fd, money, strlen(money), 0) == -1){
            perror("Error sending data to client");
            close(new_tcp_client_fd);
            exit(EXIT_FAILURE);
        }
}

void sendMsgToClient(int msg){
    char flag[BUFLEN];
    sprintf(flag,"%d", msg);
    if (send(new_tcp_client_fd, flag, strlen(flag), 0) == -1) {
        perror("Error sending data to client");
        close(new_tcp_client_fd);
        exit(EXIT_FAILURE);
    }
}

bool compareInterval(transaction i1, transaction i2)
{
    return (stoi(i1.id) < stoi(i2.id));
}

void sendTransactionsToMonitor(){
    char id[BUFLEN];
    char name1[BUFLEN];
    char name2[BUFLEN];
    char transaction_amount[BUFLEN];
    //sort transactions
    sort(transactions.begin(), transactions.end(), compareInterval);
    char buf[BUFLEN];
    for(auto transaction: transactions){
        //send transaction id
        for (int i = 0; i < 10000000; i++){}
        sprintf(id, "%s", transaction.id.c_str());
        if (send(new_tcp_monitor_fd, id, strlen(id), 0) == -1){
            perror("Error sending data to Monitor");
            close(new_tcp_monitor_fd);
            exit(EXIT_FAILURE);
        }

        //send transaction from
        for (int i = 0; i < 10000000; i++){}
        sprintf(name1, "%s", transaction.from.c_str());
        if (send(new_tcp_monitor_fd, name1, strlen(name1), 0) == -1){
            perror("Error sending data to Monitor");
            close(new_tcp_monitor_fd);
            exit(EXIT_FAILURE);
        }

        //send transaction to
        for (int i = 0; i < 10000000; i++){}
        sprintf(name2, "%s", transaction.to.c_str());
        if (send(new_tcp_monitor_fd, name2, strlen(name2), 0) == -1){
            perror("Error sending data to Monitor");
            close(new_tcp_monitor_fd);
            exit(EXIT_FAILURE);
        }

        //send transaction amount
        for (int i = 0; i < 10000000; i++){}
        sprintf(transaction_amount, "%s", transaction.amount.c_str());
        if (send(new_tcp_monitor_fd, transaction_amount, strlen(transaction_amount), 0) == -1){
            perror("Error sending data to Monitor");
            close(new_tcp_monitor_fd);
            exit(EXIT_FAILURE);
        }
    }
    memset(buf, '\0', sizeof(buf));
    for (int i = 0; i < 10000000; i++){}
    // Send NULL char to signify end of communication
    if (send(new_tcp_monitor_fd, buf, strlen(buf), 0) == -1){
        perror("Error sending data to Monitor");
        close(new_tcp_monitor_fd);
        exit(EXIT_FAILURE);
    }
    cout<<"The main server send a sorted list to the monitor using TCP over port 26161."<<endl;
}

void recvFromClient(){
    acceptFromClient();

//    char recv_buf[1024];
//    memset(recv_buf, 'z', 1024);
    memset(&request, 0, sizeof (request));
    int num_of_bytes_read = recv(new_tcp_client_fd, &request, sizeof (request), 0);
    if(num_of_bytes_read < 0) {
        perror("Reading Stream Message Error");
        exit(1);
    } else if (num_of_bytes_read == 0) {
        printf("Connection Has Ended\n");
    }
//    memset(&request, 0, sizeof request);
//    memcpy(&request, recv_buf, sizeof request);
}

void sendToClient(int flag, int amount){
    // 0 means check succ
    // 1 means check but not exist
    // 2 means transfer success
    // 3 means name1 not exist
    // 4 means name2 not exist
    // 5 means neither name1 nor name2 exist
    // 6 means name1 insufficient
    response.flag = flag;
    response.amount = amount;
    char send_buf1[1024];

    memset(send_buf1, 0, 1024);
    memcpy(send_buf1, &response, sizeof response);

    int sent = send(new_tcp_client_fd, send_buf1, sizeof (send_buf1), 0);
    // check whether send successfully
    if (sent == -1) {
        perror("Connection Error");
        close(new_tcp_client_fd);
        exit(1);
    }
    // print onscreen message
}


void select_Client(){
    recvFromClient();

    build_wallets();
    // only check
    if(!wallets.count(request.name2)){
        cout<<"The main server received input="<< request.name1 <<" from the client using TCP over port "<< M_Client_TCP_PORT<<endl;
        // check name1 if exist
        // if not
        if(!wallets.count(request.name1)){
            sendToClient(1, 0);
            cout << request.name1 << " is not in network"<<endl;
        }else{
            sendToClient(0, wallets.find(request.name1)->second);
            cout << "The main server sent the current balance to the client."<< endl;
        }
    //tansfer
    }else{
        cout << "The main server received from " << request.name1 << " to transfer "<<request.amount <<" coins to "<< request.name2<<" using TCP over port "<<M_Client_TCP_PORT <<endl;
        char now_id[MAX_NUM_OF_TRANSACTIONS_BIT];
        // convert id int to char[]
        sprintf(now_id, "%lu", transactions.size() + 1);

        // check name if exist or not
        // from name and to name both exist
        if(wallets.find(string(request.name1)) != wallets.end() && wallets.find(string(request.name2)) != wallets.end()){
            //insufficient
            if(wallets.find(request.name1)->second - request.amount < 0){
                sendToClient(6,wallets.find(request.name1)->second);
            }else{
                transactions.push_back({to_string(transactions.size() + 1), request.name1, request.name2, to_string(request.amount)});
                build_wallets();
                sendToClient(2,wallets.find(request.name1)->second);

                // randomly choose server to add new transaction in blockX.txt
                srand((unsigned)time(NULL));
                int random = (rand() % (3-1+1))+1;
                char amount_char[BUFLEN];
                sprintf(amount_char, "%d",request.amount);
                if(random == 1){
                    sendToA(now_id,request.name1,request.name2,amount_char);
                }else if(random == 2){
                    sendToB(now_id,request.name1,request.name2,amount_char);
                }else{
                    sendToC(now_id,request.name1,request.name2,amount_char);
                }
                cout<<"The main server sent the result of the transaction to the client."<<endl;
            }

        }
        // name1 exist, name2 not
        else if(wallets.find(string(name1)) != wallets.end() && wallets.find(string(name2)) == wallets.end()){
            sendToClient(3,0);
        }
        // name1 not exist, name2 exist
        else if(wallets.find(string(name1)) == wallets.end() && wallets.find(string(name2)) != wallets.end()){
            sendToClient(4,0);
      }else{
            //neither name1 nor name2 exist
            // send4
            sendToClient(5,0);
        }
    }

//    recvFlagFromClient();
//    sendFlagToClient();
//    // that means client want to transact
//    if(flag2[0] == '1'){
//
//        cout << "The main server received from " << name1 << " to transfer "<<transaction_amount <<" coins to "<< name2<<" using TCP over port "<<M_Client_TCP_PORT <<endl;
//        char now_id[MAX_NUM_OF_TRANSACTIONS_BIT];
//        // convert id int to char[]
//        sprintf(now_id, "%lu", transactions.size() + 1);
//        recvTransactionFromClient();
//        // check name if exist or not
//        // from name and to name both exist
//
//        if(wallets.find(string(name1)) != wallets.end() && wallets.find(string(name2)) != wallets.end()){
//            // 0 means both from and to are in the wallets, but name1 not sufficent to provide money to name2
//            if(wallets.find(name1)->second - atoi(transaction_amount) < 0){
//                sendMsgToClient(0);
//                //even insufficient, send the original balance
//                for(int i = 0; i < 10000000; i++){}
//                sendWalletToClient(name1);
//            }else{
//                sendMsgToClient(1);
//
//                transactions.push_back({to_string(transactions.size() + 1), string(name1), string(name2), string(transaction_amount)});
//                build_wallets();
//                for(int i = 0; i < 10000000; i++){}
//                sendWalletToClient(name1);
//
//                // randomly choose server to add new transaction in blockX.txt
//                srand((unsigned)time(NULL));
//                int random = (rand() % (3-1+1))+1;
//                if(random == 1){
//                    sendToA(now_id,name1,name2,transaction_amount);
//                }else if(random == 2){
//                    sendToB(now_id,name1,name2,transaction_amount);
//                }else{
//                    sendToC(now_id,name1,name2,transaction_amount);
//                }
//                cout<<"The main server sent the result of the transaction to the client."<<endl;
//            }
//
//        }
//        // name1 exist, name2 not
//        else if(wallets.find(string(name1)) != wallets.end() && wallets.find(string(name2)) == wallets.end()){
//            sendMsgToClient(2);
//            // name1 not exist, name2 exist
//        } else if(wallets.find(string(name1)) == wallets.end() && wallets.find(string(name2)) != wallets.end()){
//            sendMsgToClient(3);
//        }else{
//            //neither name1 nor name2 exist
//            // send4
//            sendMsgToClient(4);
//        }
//    }
//
//
//    // client only want to check balance
//    else{
//        recvCheckWalletFromClient();
//        cout<<"The main server received input="<< name1 <<" from the client using TCP over port "<< M_Client_TCP_PORT<<endl;
//        // check name1 if exist
//        // if not
//        if(!wallets.count(name1)){
//            sendMsgToClient(2);
//            cout << name1 << " is not in network"<<endl;
//        }else{
//            sendMsgToClient(1);
//            for(int i = 0; i < 10000000; i++){}
//            sendWalletToClient(name1);
//            cout << "The main server sent the current balance to the client."<< endl;
//        }
//
//    }
    close(new_tcp_client_fd);

}

void select_Monitor(){
    acceptFromMonitor();
    recvFromMonitor();
    sendTransactionsToMonitor();
    close(new_tcp_monitor_fd);
}

int main(){
    cout << "The main server is up and running."<<endl;
    init_ClientTCP();
    init_MonitorTCP();
    init_UDP();

    receiveFromA();
    receiveFromB();
    receiveFromC();

    while(1){
        FD_ZERO(&readfds);
        FD_SET(m_tcp_client_fd, &readfds);
        FD_SET(m_tcp_monitor_fd, &readfds);

        // since we got s2 second, it’s the "greater", so we use that for
        // the n param in select()
        n = m_tcp_monitor_fd + 1;
        // wait until either socket has data ready to be recv()d (timeout 10.5 secs)
        tv.tv_sec = 10;
        tv.tv_usec = 500000;
        rv = select(n, &readfds, NULL, NULL, &tv);
        if (rv == -1) {
            perror("select"); // error occurred in select()
        } else if (rv == 0) {
//            printf("Timeout occurred! No data after 10.5 seconds.\n");
        } else {
            if (FD_ISSET(m_tcp_client_fd, &readfds)) {
                select_Client();

            }
            if (FD_ISSET(m_tcp_monitor_fd, &readfds)) {
                select_Monitor();
            }
        }
    }
}