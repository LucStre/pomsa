#include <iostream>
#include <getopt.h>
#include "../pomsa_build/PomsaConfig.h"
#include "../include/alignToGraph.hpp"

using namespace std;

static int help_flag;
static int version_flag;
static int align_flag;
static int match_flag;
static int mismatch_flag;
static int gap_flag;

void printHelpMessage(){
	char help[] = {
	#include "help.txt"
	};
	for(int i=0;true;i++){
		if(help[i] == '\0') break;
		cerr << help[i];
	}
  	exit(0);
}

int main(int argc, char **argv){
    int c;
    int type;
    int match = 5, mismatch = -4, gap = -8;
    while(1){
        static struct option long_options[] =
            {
                {"help", no_argument, &help_flag, 1},
                {"version", no_argument, &version_flag, 1},
                {"align", required_argument, &align_flag, 1},
                {"match", required_argument, &match_flag, 1},
                {"mismatch", required_argument, &mismatch_flag, 1},
                {"gap", required_argument, &gap_flag, 1},
                {0, 0, 0, 0}
            };
        int option_index = 0;
        c = getopt_long(argc, argv, "vx:hx:a:m:n:g:", long_options, &option_index);
        if(c == -1){
            break;
        }
        switch (c)
        {
        case 0:
            break;
        case 'v':
            version_flag = 1;
            break;
        case 'h':
            help_flag = 1;
            break;
        case 'a':
            align_flag = 1;
            type = atoi(optarg);
            break;
        case 'm':
            match = atoi(optarg);
            break;
        case 'n':
            mismatch = atoi(optarg);
            break;
        case 'g':
            gap = atoi(optarg);
            break;
        case '?':
            break;
        default:
            abort();
        }
    }
    if(help_flag) printHelpMessage();
    if(version_flag){
        cerr << "v" << pomsa_VERSION_MAJOR << "." << pomsa_VERSION_MINOR << "." << pomsa_VERSION_PATCH << endl;
        exit(0);
    }

    if(align_flag){
        vector<string> sequences = {"GTA", "AGCA", "GTAT", "GTAC"};
        
        for(vector<string>::iterator it = sequences.begin(); it != sequences.end(); it++){
            if(it == sequences.begin()){
                start((*it), (*it).length(), match, mismatch, gap, type);
            }else{
                alignSequence((*it), (*it).length());
            }
        }
        printGFA();
    }   
    return 0;
}