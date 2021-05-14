#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>
#include <fstream>
#include <cstring>

struct header{
    std::string key;
    std::string type;
    double val;
};

struct sequence{
    int id = 0;
    std::string seqString;
    int seqlen;
};

struct link{
    int firstId = 0;
    int secondId = 0;
    std::string cigar;
};

class GFA{

    private:
        header head;
        std::ofstream MyFile;
        std::vector<std::tuple<int, std::string>> path;
        std::vector<link> links;
    public:
        std::vector<sequence> sequences;
        GFA(std::string file, double version){
            MyFile.open(file);
            head.key = "VZ";
            head.type = "Z";
            head.val = version;
        }

        void headerLine(){
            MyFile << "H\t" << head.key << ":" << head.type <<  ":" << head.val << "\n";
        }

        void segmentLine(){
            for(sequence s : sequences){
                MyFile << "S\t" << s.id << "\t" << s.seqString << "\n";
            }
        }

        void linkLine(){
            for(link l : links){
                MyFile << "L\t" << l.firstId << "\t+\t" << l.secondId << "\t+\t" << l.cigar << "\n";
            }
        }

        void pathLine(int seq_id){
            MyFile << "P\t" << seq_id << "\t";
            for(std::vector<sequence>::iterator it = sequences.begin(); it != sequences.end(); it++){
                if(it == sequences.begin()){
                    MyFile << (*it).id << "+";
                }else{
                   MyFile << "," << (*it).id << "+"; 
                }
            }
            MyFile << "\t";
            for(std::vector<link>::iterator it = links.begin(); it != links.end(); it++){
                if(it == links.begin()){
                    MyFile << (*it).cigar;
                }else{
                   MyFile << "," << (*it).cigar; 
                }
            }
            MyFile << "\n";
        }

        void printPath(){
            for(std::tuple<int, std::string> s : path){
                MyFile << std::get<0>(s) << " " << std::get<1>(s) << "\n";
            }
        }

        void findPath(Graph *graph){
            std::string novi = "";
            for(sequence s : sequences){
                bool yes = false;
                novi = "";
                for(Node* node : (*graph).nodes){
                    for(auto seeq : node->sequence){
                        if(std::get<0>(seeq) == s.id){
                            novi += node->value;
                            yes = true;
                        }
                    }
                    if(!yes){
                        novi += "-";
                        yes = false;
                    }
                }
                if(novi.length() < (*graph).graph_length){
                    for(int i = 0; i < (*graph).graph_length - novi.length(); i++){
                        novi += "-";
                    }
                }
                path.push_back(std::make_tuple(s.id, novi));
            }
        }

        void addSequence(const char* seq, int seqlen, int seq_id){
            std::string ss = "";
            for(int i = 0; i < seqlen; i++){
                ss += seq[i];
            }
            sequence s;
            s.id = seq_id;
            s.seqlen = seqlen;
            s.seqString = ss;
            sequences.push_back(s);
        }

        void addPath(std::tuple<int, std::string> s){
            path.push_back(s);
        }

        void addLink(int seq1_id, int seq2_id, std::string cigar){
            link l;
            l.firstId = seq1_id;
            l.secondId = seq2_id;
            l.cigar = cigar;
            links.push_back(l);
        }

        void close(){
            MyFile.close();
        }

        void calculateConsensus(unsigned int lenght, int seq_number){
            std::string cons = "";
            std::string consensusSequence = "";
            char char_array[seq_number];
            int max = 0;
            char result;
            for(int i = 0; i < lenght; i++){
                for(std::vector<std::tuple<int, std::string>>::iterator it = path.begin(); it != path.end(); it++){
                    cons += std::get<1>(*it).at(i);
                }
                char count[256] = {0};
                max = 0;
                strcpy(char_array, cons.c_str());
                for (int i = 0; i < seq_number; i++) {
                    count[char_array[i]]++;
                    if (max < count[char_array[i]]) {
                        max = count[char_array[i]];
                        result = char_array[i];
                    }
                }
                consensusSequence += result;
                cons = "";
            }
            //add consensus sequence to path
            path.push_back(make_tuple(seq_number + 1, consensusSequence)); 
        }
};