#include <time.h>
#include <fstream>
#include <queue>
#include <iostream>
#include "parser.h"
#include "fmDM.h"


//---------------------------------------------------------------------------------------------------------------------//
int main(int argc, char *argv[]){
    //variable declaration
    clock_t start,end;
    start = clock();
    srand( time(NULL) );
    int repeat = 100,minCut = 999999;
    double time_limit = 28.0;

    parser_C* pParser = new parser_C();
    pParser->parse(argv[1]);
    string outfileName = argv[2];

    int count_not_better = 0;
    for(int i=0;i<repeat;i++){
        cout << "iter " << i << "\n";
		fmPart_C *fmPart = new fmPart_C(pParser);
        fmPart->initPart();
        bool in_time_limit = fmPart->runFM(minCut,start,time_limit);
		
        if( fmPart->getMinCut() < minCut){
            minCut = fmPart->getMinCut();
            fmPart->writeBestPart(outfileName);
            count_not_better = 0;
        }
        else{
            count_not_better++;
        }
        if(count_not_better > 6 || !in_time_limit) break;
    }
    
    end = clock();
    printf("Execution Time: %f seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
    delete pParser;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------//
