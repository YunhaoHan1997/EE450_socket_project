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
    char recv_buf1[1024];
    ComputeRequestInfo input_msg;
    input_msg.map_id = 1;
    input_msg.src_vertex_idx = 10;
    input_msg.file_size = (long long)100;
//    memset(recv_buf1, 0, 1024);
    recv_buf1[0] = '\0';
//    memcpy(recv_buf1, &input_msg, sizeof (input_msg));
    cout<<(recv_buf1[0] == '\0')<<endl;

}
