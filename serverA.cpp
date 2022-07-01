//
// Created by Yunhao on 6/19/22.
//
#include <iostream>
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
#include <sstream>

using namespace std;
#define LOCAL_HOST "127.0.0.1" // Host address
#define ServerA_PORT 21161
#define BUFLEN 1000 // Length of socket stream buffer

char recv_id[BUFLEN];
char recv_name1[BUFLEN];
char recv_name2[BUFLEN];
char recv_amount[BUFLEN];
int recvLen1;

struct transaction{
    string id;
    string name1;
    string name2;
    string amount;
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

string decrypt(string s){
    for(char & i : s){
        //for 0 - 9
        if(int(i) >= 48 && int(i) <= 57){
            if(int(i) >= 48 && int(i) <= 50){
                i = char(int(i) + 10 - 3);
            }else{
                i = char(int(i) - 3);
            }
            //for A-Z
        }else if(i >= 65 && i <= 90){
            if(int(i) >= 65 && int(i) <= 67){
                i = char(int(i) + 10 - 3);
            }else{
                i = char(int(i) - 3);
            }
            // for a-z
        }else if(i >= 97 && i <= 122){
            if(int(i) >= 97 && int(i) <=99){
                i = char(int(i) + 10 - 3);
            }else{
                i = char(int(i) - 3);
            }
        }
    }
    return s;
}

void constructTransactions(){
    string line;
    fstream transactionFile;
    transactionFile.open("block1.txt", ios::in);
    if(transactionFile.is_open()){
        while(getline(transactionFile, line)){
            string id;
            string name1;
            string name2;
            string amount;
            stringstream ss;
            ss << line;
            ss >> id >>name1 >> name2 >> amount;
//            cout << name1 << name2 << amount;
            if(!id.empty() && !name1.empty() && !name2.empty() && !amount.empty()){
                name1 = decrypt(name1);
                name2 = decrypt(name2);
                amount = decrypt(amount);
                transactions.push_back(transaction{id, name1, name2, amount});
            }

        }
    }
    transactionFile.close();
}

void addTransaction(string &id, string &name1, string &name2, string &amount){
    transactions.push_back(transaction{id, name1, name2, amount});
    ofstream write;
    write.open("block1.txt",ios::app);
    write << id << " " <<name1 << " " << name2 << " "<< amount << endl;
    write.close();

}

void recvFromM(){
    socklen_t awsLen = sizeof(m_udp);
    //    recv trans ID
    if ((recvLen1 = recvfrom(serverA_sockfd, recv_id, BUFLEN, 0, (struct sockaddr *) &m_udp, &awsLen)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }

    //    recv name1
    if ((recvLen1 = recvfrom(serverA_sockfd, recv_name1, BUFLEN, 0, (struct sockaddr *) &m_udp, &awsLen)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }

    //recv name2
    if ((recvLen1 = recvfrom(serverA_sockfd, recv_name2, BUFLEN, 0, (struct sockaddr *) &m_udp, &awsLen)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }

    //recv amount
    if ((recvLen1 = recvfrom(serverA_sockfd, recv_amount, BUFLEN, 0, (struct sockaddr *) &m_udp, &awsLen)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }

    cout << "serverA receive from M succ" << endl;

//    cout << "The Server A has received input for finding shortest paths: starting vertex " << recvVertexIndex << " of map " << recvMapID << "." << endl;
}

//todo: send to M
void sendToM(){
    char flag[BUFLEN];

    int sendLen;
    memset(flag, '\0', sizeof(flag));
    for(transaction item: transactions){
        //send id
        if ( ( sendLen = sendto(serverA_sockfd, item.id.c_str(), strlen(item.id.c_str()), 0, (struct sockaddr *) &m_udp, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message1 to MServer from Server A");
            exit(EXIT_FAILURE);
        }

        //send name1
        if ( ( sendLen = sendto(serverA_sockfd, item.name1.c_str(), strlen(item.name1.c_str()), 0, (struct sockaddr *) &m_udp, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message2 to MServer from Server A");
            exit(EXIT_FAILURE);
        }

        // send name2
        if ( ( sendLen = sendto(serverA_sockfd, item.name2.c_str(), strlen(item.name2.c_str()), 0, (struct sockaddr *) &m_udp, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message3 to MServer from Server A");
            exit(EXIT_FAILURE);
        }

        //send amount
        if ( ( sendLen = sendto(serverA_sockfd, item.amount.c_str(), strlen(item.amount.c_str()), 0, (struct sockaddr *) &m_udp, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message4 to MServer from Server A");
            exit(EXIT_FAILURE);
        }
    }

    // Send NULL char to signify end of communication
    //todo here has error
//    memset(flag, '\0', sizeof(flag));
//    if ( ( sendLen = sendto(serverA_sockfd, flag, strlen(flag), 0, (struct sockaddr *) &m_udp, sizeof(struct sockaddr_in))) == -1) {
//        perror("Error sending UDP message5 to MServer from Server A");
//        exit(EXIT_FAILURE);
//    }
}

int main(){
    init_UDP();
    cout << "init udp succ" << endl;
    constructTransactions();
    cout << "transacitons succe" <<endl;
    while(1){
        recvFromM();
        sendToM();
    }

}