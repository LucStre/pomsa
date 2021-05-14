#include <iostream>
#include "../pomsa_build/PomsaConfig.h"
#include "../include/alignToGraph.hpp"

using namespace std;

int main(int argc, char **argv){
    if(argc < 2){
        cout << argv[0] << " Version " <<  pomsa_VERSION_MAJOR << "." << pomsa_VERSION_MINOR << "." << pomsa_VERSION_PATCH << endl;
    }

    /* char seq1[] = {'A', 'G', 'C', 'T', 'G', 'C', 'A', 'T'};
    char seq2[] = {'G', 'C', 'A', 'G', 'C', 'G'}; */
    /* char seq1[] = {'C', 'C', 'G', 'C', 'T', 'T', 'T', 'T', 'C', 'C', 'G', 'C'};
    char seq2[] = {'C', 'C', 'G', 'C', 'A', 'A', 'A', 'A', 'C', 'C', 'G', 'C'}; */

    vector<string> sequences = {"GTA", "AGCA", "GTAT", "GTAC"};

    for(vector<string>::iterator it = sequences.begin(); it != sequences.end(); it++){
        if(it == sequences.begin()){
            start((*it), (*it).length());
        }else{
            alignSequence((*it), (*it).length());
        }
    }
    printGFA();    
    return 0;
}