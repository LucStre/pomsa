#include <iostream>
#include "../pomsa_build/PomsaConfig.h"
#include "../include/align/alignToGraph.hpp"

using namespace std;

int main(int argc, char **argv){
    if(argc < 2){
        cout << argv[0] << " Version " <<  pomsa_VERSION_MAJOR << "." << pomsa_VERSION_MINOR << "." << pomsa_VERSION_PATCH << endl;
    }

    char seq1[] = {'A', 'G', 'C', 'T', 'G', 'C', 'A', 'T'};
    char seq2[] = {'G', 'C', 'A', 'G', 'C', 'G'};
    //char seq1[] = {'C', 'C', 'G', 'C', 'T', 'T', 'T', 'T', 'C', 'C', 'G', 'C'};
    //char seq2[] = {'C', 'C', 'G', 'C', 'A', 'A', 'A', 'A', 'C', 'C', 'G', 'C'};
    start(seq1, sizeof(seq1), seq2, sizeof(seq2));
    
    return 0;
}