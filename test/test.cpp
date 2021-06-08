#include <iostream>
#include <fstream>
#include <chrono>
#include "../include/alignToGraph.hpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char **argv){
    string ulaz;
    ifstream infile(argv[1]);
    int match = 5, mismatch = -4, gap = -8, i = 0, type = 0;
    int len;
    auto begin = high_resolution_clock::now();
    while(getline(infile, ulaz)){
        if(i == 0){
            start(ulaz, ulaz.length(), match, mismatch, gap, type);
            i++;
            len = ulaz.length();
        }else{
            alignSequence(ulaz, ulaz.length());
        }
    }
    auto end = high_resolution_clock::now();
    printGFA();
    auto duration = duration_cast<microseconds>(end-begin);
    cout << "Length of sequences: " << len << endl;
    cout << "Duration: " << duration.count() << " microseconds" << endl;
    return 0;
}