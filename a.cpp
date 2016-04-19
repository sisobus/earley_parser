#include "utils.h"
#define DEBUG
int main(int argc,char *argv[]) {
    Printer printer;
    Option  option;
    Grammer grammer;
    vector<ParsedString> parsedStrings;

    option.getOption(argc,argv);
    grammer = initGrammer(option.grammerFilename);
    parsedStrings = initInputString(option.inputFilename);

#ifdef DEBUG
    grammer.printG();
#endif
    /*
     * psi = parsed string index
     */
    for ( int psi = 0 ; psi < (int)parsedStrings.size() ; psi++ ) {
        ParsedString    curParsedString = parsedStrings[psi];
        string          curOriginalString = getOriginalString(curParsedString);
#ifdef DEBUG
        printParsedInputString(stdout,curParsedString);
        printer.print("current original string :",curOriginalString);
#endif
    }
    return 0;
}
