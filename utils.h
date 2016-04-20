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
#include <deque>
#include <set>
using namespace std;

#define     DEFAULT_GRAMMER_FILENAME    "grammer.txt"
#define     DEFAULT_STRING_FILENAME     "input.txt"
#define     PARAMETER_GRAMMER_FILENAME  "-g"
#define     PARAMETER_STRING_FILENAME   "-f"
#define     PARAMETER_HELP              "-h"
#define     MESSAGE_HELP                "help> ./run -g grammer_filename(default: grammer.txt) -f input_string_filename(default: input.txt) "

typedef unsigned int uint;
typedef pair<string,deque<string> > svs;
typedef deque<string> Words;

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
    void print(string message,deque<string> vs) {
        printf("%s\n",message.c_str());
        for ( int i = 0 ; i < (int)vs.size() ; i++ ) 
            printf("%s ",vs[i].c_str());
        puts("");
    }
    void print(string message,vector<deque<string> > vvs) {
        printf("%s\n",message.c_str());
        for ( int i = 0 ; i < (int)vvs.size() ; i++ ) {
            printf("%d: ",i);
            for ( int j = 0 ; j < (int)vvs[i].size() ; j++ ) 
                printf("%s ",vvs[i][j].c_str());
            puts("");
        }
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
    multimap<string,deque<string> > g;
    multimap<deque<string>,string> bg;
    Grammer() {}
    void insert(svs curGrammer) {
        g.insert(curGrammer);
        bg.insert(pair<deque<string>, string>(curGrammer.second,curGrammer.first));
    }
    void printG() {
        for ( map<string,deque<string> >::iterator it=g.begin();it!=g.end();it++ ) {
            fprintf(stdout,"%s -> ",it->first.c_str());
            for ( int i = 0 ; i < (int)it->second.size() ; i++ ) {
                fprintf(stdout,"%s ",it->second[i].c_str());
            }
            fprintf(stdout,"\n");
        }
    }
    void printBg() {
        for ( multimap<deque<string>,string>::iterator it=bg.begin();it!=bg.end();it++ ) {
            for ( int i = 0 ; i < (int)it->first.size() ; i++ ) 
                fprintf(stdout,"%s ",it->first[i].c_str());
            fprintf(stdout,"-> ");
            fprintf(stdout,"%s\n",it->second.c_str());
        }
    }
    vector<deque<string> > findRhsUsingLhs(string lhs) {
        vector<deque<string> > ret;
        pair<multimap<string,deque<string> >::iterator,multimap<string,deque<string> >::iterator> foundGrammer = g.equal_range(lhs);
        for ( multimap<string,deque<string> >::iterator it=foundGrammer.first;it!=foundGrammer.second;it++ ) 
            ret.push_back(it->second);
        return ret;
    }
    deque<string> findLhsUsingRhs(Words rhs) {
        deque<string> ret;
        pair<multimap<deque<string>,string>::iterator,multimap<deque<string>,string>::iterator> foundGrammer = bg.equal_range(rhs);
        for ( multimap<deque<string>,string>::iterator it=foundGrammer.first;it!=foundGrammer.second;it++ ) 
            ret.push_back(it->second);
        return ret;
    }
};
svs parseGrammer(char *s) {
    deque<string> vs;
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
    int start;
    int end;
    string constituent;
    Words found;
    Words next;
    State *parent;
    State(){}
    State(State* state):
        start (state->start), end (state->end), constituent (state->constituent), found (state->found), next (state->next), parent (state->parent) {}
    State(int _start,int _end,string _constituent,Words _found,Words _next):
        start (_start), end (_end), constituent (_constituent), found (_found), next (_next), parent (NULL){}
    State(int _start,int _end,string _constituent,Words _found,Words _next,State *_parent):
        start (_start), end (_end), constituent (_constituent), found (_found), next (_next), parent (_parent){}
    void printState(Words words) {
        for ( int i = 0 ; i < (int)words.size() ; i++ ) {
            if ( i == end ) printf("+ ");
            printf("%s ",words[i].c_str());
        }
        printf("%s\n",end==(int)words.size()?"+":"");
        printf("[");
        printf("%d, %d, [%s], [",start,end,constituent.c_str());
        for ( int i = 0 ; i < (int)found.size() ; i++ ) 
            printf("%s%s",i==0?"":" ",found[i].c_str());
        printf("], [");
        for ( int i = 0 ; i < (int)next.size() ; i++ ) 
            printf("%s%s",i==0?"":" ",next[i].c_str());
        printf("] ");
        printf("]\n");
    }
};
bool isCompleteState(State* state) {
    return state->next.empty();
}
bool isActiveState(State* state) {
    return !isCompleteState(state);
}
void printPendingChart(deque<State*>& pendingChart,Words words) {
    for ( int i = 0 ; i < (int)pendingChart.size() ; i++ ) {
        pendingChart[i]->printState(words);
    }
}
void printCompleteChart(vector<State*>& completeChart,Words words) {
    for ( int i = 0 ; i < (int)completeChart.size() ; i++ ) {
        completeChart[i]->printState(words);
    }
}
