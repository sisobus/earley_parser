#include "utils.h"
//#define DEBUG
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
            curState->printState(curWords);
#ifdef DEBUG
            printer.print("\n$$ cur state print $$");
            curState->printState(curWords);
            if ( curState->parent ) curState->parent->printState(curWords);
#endif
            if ( isCompleteState(curState) ) {
                /*
                 * process 1
                 *
                 * Take a pending edge e1, [i,j,p1,[c1,...,cn],[u1,...,um]] from PC
                 * if [u1,...,um] is empty, i.e., the edge e1 is a complete edge
                 * then do the process 1(e1)
                 *
                 * process1(e1): where e1 = [i,j,p1,[c1,...,cn],[]] a complete edge
                 * for each active edge of the form [k,i,x,[w1,...,wn],[p1,p2,...,pn]],
                 *      generate a new pending edge [k,j,x,[w1,...,wn,p1],[p2,...,pn]]
                 * put e1 as a completed edge
                 *
                 */
                for ( int i = 0 ; i < (int)completeChart.size() ; i++ ) {
                    if ( isActiveState(completeChart[i]) ) {
                        if ( completeChart[i]->end == curState->start &&
                                completeChart[i]->next[0] == curState->constituent ) {
                            Words nextFound = completeChart[i]->found;
                            nextFound.push_back(curState->constituent);
                            Words nextNext = completeChart[i]->next;
                            nextNext.pop_front();
                            vector<State*> newChild = completeChart[i]->child;
                            newChild.push_back(curState);
                            pendingChart.push_back(new State(completeChart[i]->start,curState->end,
                                        completeChart[i]->constituent,nextFound,nextNext,
                                        newChild));
                        }
                    }
                }
                completeChart.push_back(curState);
            } else if ( isActiveState(curState) ) {
                /*
                 * process 2
                 *
                 * Take a pending edge e1, [i,j,p1,[c1,...,cn],[u1,...,um]] from PC
                 * if [u1,...,um] is not empty, i.e., the edge e1 is not a complete edge
                 * check if there is any edges of the form, [j,_,u1,_,_] in either chart
                 *
                 * if there is (i.e., the constituent u1 had aleady been or to be tried)
                 *      if there is any completed edge e2, [j,k,u1,[_],[]] do process2(e1,e2)
                 *      put e1 as an active edge (wait to see if there is any pending edge for e1)
                 *
                 * process2(e1,e2):
                 *      e1 = [i,j,p1,[c1,...,cn],[u1,...,um]] and e2 = [j,k,u1,[_],[]],
                 *      generate a pending edge [i,k,p1,[c1,...,cn,u1],[u2,...,um]
                 * 
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
                            vector<State*> newChild = curState->child;
                            newChild.push_back(completeChart[i]);
                            pendingChart.push_back(new State(curState->start,completeChart[i]->end,
                                        curState->constituent,nextFound,nextNext,
                                        newChild));
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
                            vector<State*> newChild = curState->child;
                            newChild.push_back(pendingChart[i]);
                            pendingChart.push_back(new State(curState->start,pendingChart[i]->end,
                                        curState->constituent,nextFound,nextNext,
                                        newChild));
                        }
                    }
                }
                /*
                 * if there is any edges of the form [j,_,u1,_,_] in either chart
                 * put e1 as an active edge
                 *
                 * otherwise, take all grammer rules with LHS as u1, (i.e., u1->x1+x2+...+xn)
                 * for each rule,
                 *      generate edges [j,j,u1,[],[x1,x2,...,xn]] into pending chart and 
                 *      put e1 as an active edge.
                 */
                if ( foundSuccess ) {
                    completeChart.push_back(curState);
                } else if ( !foundSuccess ) {
                    rhs = grammer.findRhsUsingLhs(curState->next[0]);
                    for ( int i = 0 ; i < (int)rhs.size() ; i++ ) 
                        pendingChart.push_back(new State(curState->end,curState->end,
                                    curState->next[0],Words(),rhs[i],vector<State*>()));
                    completeChart.push_back(curState);
                }
            }
        }
        /*
         * answer print
         */
        for ( int i = 0 ; i < (int)completeChart.size() ; i++ ) {
            if ( isCompleteState(completeChart[i]) ) {
                if ( completeChart[i]->start == 0 && completeChart[i]->end == (int)curWords.size() 
                        && completeChart[i]->constituent == "S" ) {
                    printParseTree(completeChart[i],curWords);
                }
            }
        }
    }
    return 0;
}
