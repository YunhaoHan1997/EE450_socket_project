// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
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

vector<transaction> transactions;

//void build_wallets(){
//    wallets.clear();
//    //clean wallets
//    for(auto transaction: transactions){
//        if(wallets.find(transaction.from) != wallets.end()){
//            auto it1 = wallets.find(transaction.from);
//            it1->second -= stoi(transaction.amount);
//        }else{
//            wallets.insert({transaction.from, 1000});
//        }
//        if(wallets.find(transaction.to) != wallets.end()){
//            auto it2 = wallets.find(transaction.to);
//            it2->second += stoi(transaction.amount);
//        }else{
//            wallets.insert({transaction.to, 1000});
//        }
//    }
//
//}
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
                i = char(int(i) + 26 - 3);
            }else{
                i = char(int(i) - 3);
            }
            // for a-z
        }else if(i >= 97 && i <= 122){
            if(int(i) >= 97 && int(i) <=99){
                i = char(int(i) + 26 - 3);
            }else{
                i = char(int(i) - 3);
            }
        }
    }
    return s;
}

string encrypt(string s){
    for(char & i : s){
        //for 0 - 9
        if(int(i) >= 48 && int(i) <= 57){
            // 7-9
            if(int(i) >= 55 && int(i) <= 57){
                i = char(int(i) - 10 + 3);
            }else{
                i = char(int(i) + 3);
            }
            //for A-Z
        }else if(i >= 65 && i <= 90){
            // X-Z
            if(int(i) >= 88 && int(i) <= 90){
                i = char(int(i) - 26 + 3);
            }else{
                i = char(int(i) + 3);
            }
            // for a-z
        }else if(i >= 97 && i <= 122){
            // x-z
            if(int(i) >= 120 && int(i) <=122){
                i = char(int(i) - 26 + 3);
            }else{
                i = char(int(i) + 3);
            }
        }
    }
    return s;
}
int main() {
    cout<<encrypt("Chinmay")<<endl;

    return 0;
}
