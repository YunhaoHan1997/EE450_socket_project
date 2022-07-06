// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include "random"
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
#include <cstdlib>
#include <ctime>
#define PORT	 8080
#define MAXLINE 1024
#include <map>
using namespace std;
// Driver code

map<string, int> wallets;
struct transaction{
    string id;
    string from;
    string to;
    string amount;
};
struct ComputeRequestInfo
{
    int			map_id;
    int			src_vertex_idx;
    long long	file_size;
};

vector<transaction> transactions;

int main() {
    string line;
    fstream transactionFile;
    transactionFile.open("block1.txt",ios::in);
    cout<<"get ifstream haha"<<endl;
    if(transactionFile.is_open()){
        while(getline(transactionFile, line)){
            cout<<"read haha"<<endl;
            string id;
            string name1;
            string name2;
            string amount;
            stringstream ss;
            ss << line;
            ss >> id >>name1 >> name2 >> amount;
            if(!id.empty() && !name1.empty() && !name2.empty() && !amount.empty()){
                cout<<id<< name1<<name2<<amount<<endl;
            }

        }
    }
    transactionFile.close();
}
