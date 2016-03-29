#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <cassert>
using namespace std;

#define     DEFAULT_GRAMMER_FILENAME    "grammer.txt"
#define     DEFAULT_STRING_FILENAME     "symbol.txt"
#define     PARAMETER_GRAMMER_FILENAME  "-g"
#define     PARAMETER_STRING_FILENAME   "-f"
#define     PARAMETER_HELP              "-h"
#define     MESSAGE_HELP                "help> ./run -g grammer_filename(grammer.txt) -f symbol_string_filename(symbol.txt) "

typedef unsigned int uint;

class Printer {
public:
    void print(string message) {
        printf("%s\n",message.c_str());
    }
    void print(string message,int a) {
        printf("%s %d\n",message.c_str(),a);
    }
    void print(string message,uint a) {
        printf("%s %u\n",message.c_str(),a);
    }
    void print(string message,string s) {
        printf("%s %s\n",message.c_str(),s.c_str());
    }
    void print(string message,double d) {
        printf("%s %lf\n",message.c_str(),d);
    }
    void print(string message,float f) {
        printf("%s %f\n",message.c_str(),f);
    }
    void print(string message,char *s) {
        printf("%s %s\n",message.c_str(),s);
    }
};
class Option {
    string  grammerFileName;
    string  symbolStringFileName;
    Option():
        grammerFileName         (DEFAULT_GRAMMER_FILENAME),
        symbolStringFileName    (DEFAULT_STRING_FILENAME) {}
    void getOption(int argc,char *argv[]) {
        Printer printer;
        if ( argc == 1 ) {
            printer.print(MESSAGE_HELP);
            return;
        }
        for ( int i = 0 ; i < argc ; i++ ) {
            if ( string(argv[i]) == PARAMETER_GRAMMER_FILENAME ) {
                char buf[64]={};
                sscanf(argv[i+1],"%s",buf);
                this->grammerFileName = string(buf);
                i++;
            }
            if ( string(argv[i]) == PARAMETER_STRING_FILENAME ) {
            
            }
        }
    }
};
