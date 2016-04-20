#include "utils.h"
#define DEBUG
int main(int argc,char *argv[]) {
    Printer printer;
    Option  option;
    Grammer grammer;
    vector<Words> words;

    option.getOption(argc,argv);
    grammer = initGrammer(option.grammerFilename);
    words = initInputString(option.inputFilename);

#ifdef DEBUG
    grammer.printG();
#endif
    /*
     * psi = parsed string index
     */
    for ( int psi = 0 ; psi < (int)words.size() ; psi++ ) {
        Words           curWords = words[psi];
        string          curOriginalString = getOriginalString(curWords);
#ifdef DEBUG
        printParsedInputString(stdout,curWords);
        printer.print("current original string :",curOriginalString);
#endif
    }
    return 0;
}
