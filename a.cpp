#include "utils.h"
#define DEBUG
int main(int argc,char *argv[]) {
    Printer printer;
    Option  option;
    Grammer grammer;
    vector<Words> sentences;

    option.getOption(argc,argv);
    grammer     = initGrammer(option.grammerFilename);
    sentences   = initInputString(option.inputFilename);

#ifdef DEBUG
    printer.print("\n$$ grammer print $$");
    grammer.printG();
#endif
    /*
     * psi = parsed string index
     */
    for ( int psi = 0 ; psi < (int)sentences.size() ; psi++ ) {
        Words           curWords = sentences[psi];
        string          curOriginalString = getOriginalString(curWords);
#ifdef DEBUG
        printer.print("\n$$ parsed input string print $$");
        printParsedInputString(stdout,curWords);
        printer.print("",curOriginalString);
#endif
        deque<State *> pendingChart;
        vector<State *> completeChart;

        Words lhs;
        vector<Words> rhs;

        /*
         * The parser put an initial edge, [0,0,s,[],[NP,VP]] into the PC
         */
        rhs = grammer.findRhsUsingLhs("S");
        for ( int i = 0 ; i < (int)rhs.size() ; i++ ) 
            pendingChart.push_back(new State(0,0,"S",Words(),rhs[i]));
        /*
         * put trivial completed edges such as [0,1,noun,[time],[]] into the CC
         */
        for ( int i = 0 ; i < (int)curWords.size() ; i++ ) {
            Words tmp;
            tmp.push_back(curWords[i]);
            Words lhs = grammer.findLhsUsingRhs(tmp);
            for ( int j = 0 ; j < (int)lhs.size() ; j++ ) 
                completeChart.push_back(new State(i,i+1,lhs[j],tmp,Words()));
        }
#ifdef DEBUG
        printer.print("\n$$ initial complete chart print $$");
        for ( int i = 0 ; i < (int)completeChart.size() ; i++ ) 
            completeChart[i]->printState(curWords);
#endif

        /*
         * Recursively do the following until there is no pending edges in PC
         */
        while ( !pendingChart.empty() ) {
            State *curState = pendingChart.front();pendingChart.pop_front();
#ifdef DEBUG
            printer.print("\n$$ cur state print $$");
            curState->printState(curWords);
#endif
            if ( isCompleteState(curState) ) {
                /*
                 * process 1
                 */
                for ( int i = 0 ; i < (int)completeChart.size() ; i++ ) {
                    if ( isActiveState(completeChart[i]) ) {
                        if ( completeChart[i]->end == curState->start &&
                                completeChart[i]->next[0] == curState->constituent ) {
                            Words nextFound = completeChart[i]->found;
                            nextFound.push_back(curState->constituent);
                            Words nextNext = completeChart[i]->next;
                            nextNext.pop_front();
                            pendingChart.push_back(new State(completeChart[i]->start,curState->end,
                                        completeChart[i]->constituent,nextFound,nextNext,
                                        completeChart[i]));
                        }
                    }
                }
                completeChart.push_back(new State(curState));
            } else if ( isActiveState(curState) ) {
                /*
                 * process 2
                 */
                bool foundSuccess = false;
                for ( int i = 0 ; i < (int)completeChart.size() ; i++ ) {
                    if ( completeChart[i]->start == curState->end &&
                            completeChart[i]->constituent == curState->next[0] ) {
                        foundSuccess = true;
                        if ( isCompleteState(completeChart[i]) ) {
                            Words nextFound = curState->found;
                            nextFound.push_back(curState->next[0]);
                            Words nextNext = curState->next;
                            nextNext.pop_front();
                            pendingChart.push_back(new State(curState->start,completeChart[i]->end,
                                        curState->constituent,nextFound,nextNext,
                                        completeChart[i]));
                            completeChart.push_back(new State(curState));
                        }
                    }
                }
                for ( int i = 0 ; i < (int)pendingChart.size() ; i++ ) {
                    if ( pendingChart[i]->start == curState->end &&
                            pendingChart[i]->constituent == curState->next[0] ) {
                        foundSuccess = true;
                        if ( isCompleteState(pendingChart[i]) ) {
                            Words nextFound = curState->found;
                            nextFound.push_back(curState->next[0]);
                            Words nextNext = curState->next;
                            nextNext.pop_front();
                            pendingChart.push_back(new State(curState->start,pendingChart[i]->end,
                                        curState->constituent,nextFound,nextNext,
                                        pendingChart[i]));
                            completeChart.push_back(new State(curState));
                        }
                    }
                }
                if ( !foundSuccess ) {
                    rhs = grammer.findRhsUsingLhs(curState->next[0]);
                    for ( int i = 0 ; i < (int)rhs.size() ; i++ ) 
                        pendingChart.push_back(new State(curState->end,curState->end,
                                    curState->next[0],Words(),rhs[i],curState));
                    completeChart.push_back(new State(curState));
                }
            }
        }
    }
    return 0;
}
