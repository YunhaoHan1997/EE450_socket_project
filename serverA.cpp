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
#define ServerM_PORT 24161
#define BUFLEN 1000 // Length of socket stream buffer


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
    std::ifstream transactionFile("block1.txt");
    cout<<"open file succ"<<endl;
    if(transactionFile.is_open()){
        while(!transactionFile.eof() and getline(transactionFile, line)){
            cout<<"read file"<<endl;
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
}

void addTransaction(string id, string name1, string name2, string amount){
    transactions.push_back(transaction{id, name1, name2, amount});
    ofstream write;
    write.open("block1.txt",ios::app);
    write << id << " " <<name1 << " " << name2 << " "<< amount << endl;
    write.close();

}

void recvFromM(){
    char recv_id[BUFLEN];
    char recv_name1[BUFLEN];
    char recv_name2[BUFLEN];
    char recv_amount[BUFLEN];

    socklen_t m_len = sizeof(m_udp);
    cout<<"begin receive"<<endl;
    //    recv trans ID

    if ((recvLen1 = recvfrom(serverA_sockfd, recv_id, BUFLEN, 0, (struct sockaddr *) &m_udp, &m_len)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }
    recv_id[recvLen1] = '\0';
    cout<<"receive id"<<endl;

    //    recv name1
    if ((recvLen1 = recvfrom(serverA_sockfd, recv_name1, BUFLEN, 0, (struct sockaddr *) &m_udp, &m_len)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }
    cout<<"receive name1"<<endl;
    recv_name1[recvLen1] = '\0';

    //recv name2
    if ((recvLen1 = recvfrom(serverA_sockfd, recv_name2, BUFLEN, 0, (struct sockaddr *) &m_udp, &m_len)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }
    recv_name2[recvLen1] = '\0';
    cout<<"receive name2"<<endl;

    //recv amount
    if ((recvLen1 = recvfrom(serverA_sockfd, recv_amount, BUFLEN, 0, (struct sockaddr *) &m_udp, &m_len)) < 1){
        perror("Error receiving from AWS");
        exit(EXIT_FAILURE);
    }
    recv_amount[recvLen1] = '\0';
    cout<<"receive amount"<<endl;

    addTransaction(string(recv_id),string(recv_name1),string(recv_name2),string(recv_amount));

//    cout << "serverA receive from M succ" << endl;

//    cout << "The Server A has received input for finding shortest paths: starting vertex " << recvVertexIndex << " of map " << recvMapID << "." << endl;
}

//todo: send to M
void sendToM(){
    char flag[BUFLEN];
    char idBuf[BUFLEN];
    char name1Buf[BUFLEN];
    char name2Buf[BUFLEN];
    char amountBuf[BUFLEN];
    int sendLen;
    memset(flag, '\0', sizeof(flag));
    cout << "begin send to M" << endl;
    for(const transaction& item: transactions){
        sprintf(idBuf, "%s",item.id.c_str());
        sprintf(name1Buf, "%s", item.name1.c_str());
        sprintf(name2Buf, "%s", item.name2.c_str());
        sprintf(amountBuf, "%s", item.amount.c_str());
        //send id
        if ( ( sendLen = sendto(serverA_sockfd, idBuf, strlen(idBuf), 0, (struct sockaddr *) &m_udp, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message1 to MServer from Server A");
            exit(EXIT_FAILURE);
        }
        cout << "send id to M" << endl;
        // erase idBuf
        memset(idBuf,'\0', sizeof(idBuf));

        //send name1
        if ( ( sendLen = sendto(serverA_sockfd, name1Buf, strlen(name1Buf), 0, (struct sockaddr *) &m_udp, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message2 to MServer from Server A");
            exit(EXIT_FAILURE);
        }
        cout << "send name1 to M" << endl;
        // erase name1Buf
        memset(name1Buf,'\0', sizeof(name1Buf));

        // send name2
        if ( ( sendLen = sendto(serverA_sockfd, name2Buf, strlen(name2Buf), 0, (struct sockaddr *) &m_udp, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message3 to MServer from Server A");
            exit(EXIT_FAILURE);
        }
        cout << "send name2 to M" << endl;
        // erase name2Buf
        memset(name2Buf,'\0', sizeof(name2Buf));

        //send amount
        if ( ( sendLen = sendto(serverA_sockfd, amountBuf, strlen(amountBuf), 0, (struct sockaddr *) &m_udp, sizeof(struct sockaddr_in))) == -1) {
            perror("Error sending UDP message4 to MServer from Server A");
            exit(EXIT_FAILURE);
        }
        cout << "send amount to M" << endl;
        // erase amountBuf
        memset(amountBuf,'\0', sizeof(amountBuf));
    }

    // Send NULL char to signify end of communication
    memset(flag, '\0', sizeof(flag));
    if ( ( sendLen = sendto(serverA_sockfd, flag, strlen(flag), 0, (struct sockaddr *) &m_udp, sizeof(struct sockaddr_in))) == -1) {
        perror("Error sending UDP message5 to MServer from Server A");
        exit(EXIT_FAILURE);
    }
}

void setM(){
    m_udp.sin_family = AF_INET;
    //M Port #
    m_udp.sin_port = htons(ServerM_PORT);
    //M IP ADDR - INADDR_LOOPBACK refers to localhost ("127.0.0.1")
    m_udp.sin_addr.s_addr = inet_addr(LOCAL_HOST);
}

int main(){
    init_UDP();
    cout << "init udp succ" << endl;
    constructTransactions();
    cout << "The size of transactions is: " <<transactions.size()<<endl;
//    recvFromM();
    // in udp, if we want to send, we first set
    setM();
    sendToM();
//    recvFromM();
    while(1){
        recvFromM();
    }

}