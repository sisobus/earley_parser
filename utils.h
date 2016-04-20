#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <map>
#include <set>
using namespace std;

#define     DEFAULT_GRAMMER_FILENAME    "grammer.txt"
#define     DEFAULT_STRING_FILENAME     "input.txt"
#define     PARAMETER_GRAMMER_FILENAME  "-g"
#define     PARAMETER_STRING_FILENAME   "-f"
#define     PARAMETER_HELP              "-h"
#define     MESSAGE_HELP                "help> ./run -g grammer_filename(default: grammer.txt) -f input_string_filename(default: input.txt) "

typedef unsigned int uint;
typedef pair<string,vector<string> > svs;
typedef vector<string> Words;

struct Printer {
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
struct Option {
    string  grammerFilename;
    string  inputFilename;
    Option():
        grammerFilename         (DEFAULT_GRAMMER_FILENAME),
        inputFilename    (DEFAULT_STRING_FILENAME) {}
    void getOption(int argc,char *argv[]) {
        Printer printer;
        if ( argc == 1 ) {
            return;
        }
        for ( int i = 0 ; i < argc ; i++ ) {
            if ( string(argv[i]) == PARAMETER_GRAMMER_FILENAME ) {
                char buf[64]={};
                sscanf(argv[i+1],"%s",buf);
                this->grammerFilename = string(buf);
                i++;
            }
            if ( string(argv[i]) == PARAMETER_STRING_FILENAME ) {
                char buf[64]={};
                sscanf(argv[i+1],"%s",buf);
                this->inputFilename = string(buf);
                i++;
            }
            if ( string(argv[i]) == PARAMETER_HELP ) {
                printer.print(MESSAGE_HELP);
                exit(-1);
            }
        }
    }
};
/*
 * grammer parsing
 */
struct Grammer { 
    multimap<string,vector<string> > g;
    multimap<vector<string>,string> bg;
    Grammer() {}
    void insert(svs curGrammer) {
        g.insert(curGrammer);
        bg.insert(pair<vector<string>, string>(curGrammer.second,curGrammer.first));
    }
    void printG() {
        for ( map<string,vector<string> >::iterator it=g.begin();it!=g.end();it++ ) {
            fprintf(stdout,"%s -> ",it->first.c_str());
            for ( int i = 0 ; i < (int)it->second.size() ; i++ ) {
                fprintf(stdout,"%s ",it->second[i].c_str());
            }
            fprintf(stdout,"\n");
        }
    }
    void printBg() {
        for ( multimap<vector<string>,string>::iterator it=bg.begin();it!=bg.end();it++ ) {
            for ( int i = 0 ; i < (int)it->first.size() ; i++ ) 
                fprintf(stdout,"%s ",it->first[i].c_str());
            fprintf(stdout,"-> ");
            fprintf(stdout,"%s\n",it->second.c_str());
        }
    }
};
svs parseGrammer(char *s) {
    vector<string> vs;
    for ( char *p = strtok(s," \t") ; p ; p=strtok(NULL," \t") ) {
        if ( string(p) == "->" ) continue;
        vs.push_back(string(p));
    }
    svs ret;
    ret.first = vs[0];
    for ( int i = 1 ; i < (int)vs.size() ; i++ ) 
        ret.second.push_back(vs[i]);
    return ret;
}
Grammer initGrammer(string grammerFilename) {
    FILE *fp = fopen(grammerFilename.c_str(),"r");
    Grammer grammer;
    for ( char buf[1024] ; ~fscanf(fp,"%[^\n]\n",buf) ; ) {
        svs curParsedGrammer = parseGrammer(buf);
        grammer.insert(curParsedGrammer);
    }
    fclose(fp);
    return grammer;
}
Words parseString(char *s) {
    Words ret;
    for ( char *p = strtok(s," \t") ; p ; p=strtok(NULL," \t" ) )
        ret.push_back(string(p));
    return ret;
}
vector<Words> initInputString(string inputFilename) {
    FILE *fp = fopen(inputFilename.c_str(),"r");
    vector<Words> ret;
    for ( char buf[1024] ; ~fscanf(fp,"%[^\n]\n",buf) ; ) {
        for ( int i = 0 ; buf[i] ; i++ ) 
            if ( buf[i] == '.' ) buf[i] = 0;
        Words cur = parseString(buf);
        ret.push_back(cur);
    }
    fclose(fp);
    return ret;
}
void printParsedInputString(FILE* fp,Words& parsedInputString) {
    for ( int i = 0 ; i < (int)parsedInputString.size() ; i++ ) 
        fprintf(fp,"%s ",parsedInputString[i].c_str());
    fprintf(fp,"\n");
}
string getOriginalString(Words parsedString) {
    string ret = "";
    for ( int i = 0 ; i < (int)parsedString.size() ; i++ ) 
        ret = ret + parsedString[i] + " ";
    return ret;
}
struct State {
    int le;
    int ri;
};
