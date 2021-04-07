#include <iostream>
#include "../pomsa_build/PomsaConfig.h"

using namespace std;

int main(int argc, char **argv){
    if(argc < 2){
        cout << argv[0] << "Version " <<  pomsa_VERSION_MAJOR << "." << pomsa_VERSION_MINOR << "." << pomsa_VERSION_PATCH << endl;
    }
    return 0;
}