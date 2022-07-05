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
    srand((unsigned)time(NULL));
    int random = (rand() % (3-1+1))+1;
    cout<<random;

}
