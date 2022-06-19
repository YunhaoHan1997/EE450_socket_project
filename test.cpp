//
// Created by Yunhao on 6/18/22.
//
#include <iostream>
#include "string"
using namespace std;
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
int main(){
    string a = "Fklqpdb";
    cout << decrypt(a);

}