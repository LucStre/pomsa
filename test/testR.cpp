#include <iostream>
#include <fstream>
#include <chrono>
#include <dirent.h>
#include <string>
#include "../include/alignToGraph.hpp"

using namespace std;
using namespace std::chrono;

int match = 5, mismat = -4, gap = -8, i = 0, type = 0;

void list_dir(const char *path){
    struct dirent *entry;
    DIR *dir = opendir(path);
    
    if(dir == NULL){
        return;
    }
    int len[10] = {0};
    int i = 0;
    auto begin = high_resolution_clock::now();
    while((entry = readdir(dir)) != NULL){
        string ulaz, sequence = "";
        if(entry->d_name[0] != '.'){
            string file = (string) path + "/" + (string) entry->d_name;
            cout << file << endl;
            ifstream infile(file);
            while(getline(infile, ulaz)){
                if(ulaz.rfind(">", 0) != 0){
                    sequence += ulaz;
                    
                }
            }
            if(i == 0){
                start(sequence, sequence.length(), match, mismat, gap, type);
                len[i] = sequence.length();
                i++;
            }else{
                alignSequence(sequence, sequence.length());
                len[i] = sequence.length();
                i++;
            }
        }
        
    }
    auto end = high_resolution_clock::now();
    closedir(dir);
    auto duration = duration_cast<microseconds>(end-begin);
    cout << "Duration: " << duration.count() << " microseconds" << endl;
    for(int j = 0; j < 10; j++){
        cout << "Sequence lenght: " << len[j] << endl;
    }
}

int main(int argc, char **argv){
    list_dir(argv[1]);
    printGFA();
    return 0;
}