#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <tuple>
#include <fstream>
#include "graph.hpp"
#include "GFA.hpp"

using namespace std;

enum AlignmentType{
    global,
    semiglobal
};
enum ComingFromDirection{
    fromNone,
    fromLeft,
    fromTop,
    fromDiagonal
};

int MATCH = 0, INSERTION = 0, DELETION = 0;
Graph *graph = new Graph;

struct Cell
{
    int value;
    int iIndex, jIndex;
    vector<Cell *> predecessors;
    Cell *comingFrom = nullptr;

    bool operator<(const Cell &other) const{
        return value < other.value;
    };

    bool operator>(const Cell &other) const{
        return value > other.value;
    };

    bool operator==(const Cell &other) const{
        return value == other.value;
    };
};

Cell **V;

void initialization(AlignmentType type, unsigned int target_len, int gap)
{
    V[0][0].value = 0;
    V[0][0].iIndex = 0;
    V[0][0].jIndex = 0;
    for (int i = 1; i <= (*graph).nodes_number; i++){
        if(type == global){
            V[i][0].value = gap * i;
        }else{
            V[i][0].value = 0; 
        }
        V[i][0].comingFrom = &V[i - 1][0];
        V[i][0].iIndex = i;
        V[i][0].jIndex = 0;
    }
    for (int j = 1; j <= target_len; j++){
        if(type == global){
            V[0][j].value = gap * j;
        }else{
            V[0][j].value = 0; 
        }
        V[0][j].comingFrom = &V[0][j - 1];
        V[0][j].iIndex = 0;
        V[0][j].jIndex = j;
    }
    for (int i = 1; i <= (*graph).nodes_number; i++){
        for (int j = 1; j <= target_len; j++){
            V[i][j].value = 0;
            V[i][j].iIndex = i;
            V[i][j].jIndex = j;
        }
    }
    Cell *cell;
    for (int j = 1; j <= target_len; j++){
        for (vector<Node*>::iterator it = (*graph).nodes.begin(); it != (*graph).nodes.end(); it++){
            if(!(*it)->predecessors.empty()){
                for(vector<Node*>::iterator iter = (*it)->predecessors.begin(); iter != (*it)->predecessors.end(); iter++){
                    cell = &V[(*iter)->id + 1][j];
                    cell->value = V[(*iter)->id + 1][j].value;
                    cell->iIndex = (*iter)->id + 1;
                    cell->jIndex = j;
                    V[(*it)->id + 1][j].predecessors.push_back(cell);

                    cell = &V[(*iter)->id + 1][j - 1];
                    cell->value = V[(*iter)->id + 1][j - 1].value;
                    cell->iIndex = (*iter)->id + 1;
                    cell->jIndex = j - 1;
                    V[(*it)->id + 1][j].predecessors.push_back(cell);
                }
            }else{
                cell = &V[0][j];
                cell->value = V[0][j].value;
                cell->iIndex = 0;
                cell->jIndex = j;
                V[(*it)->id + 1][j].predecessors.push_back(cell);

                cell = &V[0][j - 1];
                cell->value = V[0][j - 1].value;
                cell->iIndex = 0;
                cell->jIndex = j - 1;
                V[(*it)->id + 1][j].predecessors.push_back(cell);
            }
        }
    }
}

void AlignToGraph(const char *target, unsigned int target_len, int target_id, AlignmentType type, int match, int mismatch, int gap, string *cigar = nullptr)
{
    V = new Cell *[(*graph).nodes_number + 1];
    for (unsigned int i = 0; i <= (*graph).nodes_number; i++){
        V[i] = new Cell[target_len + 1];
    }

    initialization(type, target_len, gap);

    int maxMatch = 0, maxInsert = 0;
    Cell *matchCell, *insertionCell, *deletionCell;
    
    for (int i = 1; i <= (*graph).nodes_number; i++){
        for (int j = 1; j <= target_len; j++){
            bool inserted = false, matched = false;
            for (vector<Cell *>::iterator it = V[i][j].predecessors.begin(); it != V[i][j].predecessors.end(); it++){
                if ((*it)->jIndex == j - 1){
                    maxMatch = (*it)->value + (((*graph).nodes[i - 1]->value == target[j - 1]) ? match : mismatch);
                    if (!matched){
                        MATCH = maxMatch;
                        matchCell = *it;
                        matched = true;
                    }
                }else if ((*it)->jIndex == j){
                    maxInsert = (*it)->value + gap;
                    if (!inserted){
                        INSERTION = maxInsert;
                        insertionCell = *it;
                        inserted = true;
                    }
                }
                if (maxMatch > MATCH){
                    MATCH = maxMatch;
                    matchCell = *it;
                }
                if (maxInsert > INSERTION){
                    INSERTION = maxInsert;
                    insertionCell = *it;
                }
            }
            DELETION = V[i][j - 1].value + gap;
            deletionCell = &(V[i][j - 1]);
            if(MATCH >= INSERTION && MATCH >= DELETION){
                V[i][j].comingFrom = matchCell;
                V[i][j].value = MATCH;
            }
            else if(INSERTION > MATCH && INSERTION >= DELETION){
                V[i][j].comingFrom = insertionCell;
                V[i][j].value = INSERTION;
            }
            else{
                V[i][j].comingFrom = deletionCell;
                V[i][j].value = DELETION;
            }
        }
    }

    Cell *alignScore;
    
    alignScore = &V[(*graph).nodes_number][target_len];
    alignScore->iIndex = (*graph).nodes_number;
    alignScore->jIndex = target_len;
    int cellNumber = (*graph).nodes_number;
    
    Node* newNode;
    Node* insertionNode = new Node;
    Node* succNode = new Node;
    Node* currentNode = new Node;
    bool mm = false;
    string tmp = "";
    while (alignScore->comingFrom != nullptr){
        if (alignScore->comingFrom->iIndex == alignScore->iIndex && alignScore->comingFrom->jIndex < alignScore->jIndex){
            //fromLeft
            tmp = tmp + 'I';
            newNode = (*graph).createNode(target[alignScore->jIndex - 1], target_id, alignScore->jIndex - 1);
            (*graph).graph_length++;
            if(currentNode->id >= 0){
                (*graph).createEdge(newNode, currentNode);
                currentNode->predecessors.push_back(newNode);
                currentNode = new Node;
            }
            if(insertionNode->id >= 0){
                (*graph).createEdge(newNode, insertionNode);
                insertionNode->predecessors.push_back(newNode);
            }
            insertionNode = newNode;
        }
        else if (alignScore->comingFrom->jIndex == alignScore->jIndex && alignScore->comingFrom->iIndex < alignScore->iIndex){
            //fromTop
            tmp = tmp + 'D';
            currentNode = (*graph).nodes[alignScore->iIndex - 1];
            list <Edge *> delEdges;
            for(Edge* e : (*graph).edges){
                if(e->src == currentNode && e->dest == newNode){
                    for(Node* n : e->src->predecessors){
                        (*graph).createEdge(n, newNode);
                        newNode->predecessors.push_back(n);
                    }
                    for(Node* n : e->dest->predecessors){
                        if(n == currentNode){
                            (*graph).deletePredecessor(e->dest, n);
                        }
                        
                    }
                    delEdges.push_back(e);
                }
            }
            for(Edge* e : delEdges){
                (*graph).deleteEdge(e);
            }
        }
        else if (alignScore->comingFrom->jIndex < alignScore->jIndex && alignScore->comingFrom->iIndex < alignScore->iIndex){
            //fromDiagonal
            tmp = tmp + 'M';
            if((*graph).nodes[alignScore->iIndex - 1]->value == target[alignScore->jIndex - 1]){
                //match
                currentNode = (*graph).nodes[alignScore->iIndex - 1];
                currentNode->sequence.push_back(make_tuple(target_id, alignScore->jIndex - 1));
                if(insertionNode->id >= 0){
                    (*graph).createEdge((*graph).nodes[alignScore->iIndex - 1], insertionNode);
                    insertionNode->predecessors.push_back((*graph).nodes[alignScore->iIndex - 1]);
                    insertionNode = new Node;
                }
                if(mm){
                    for(Node* n : (*graph).nodes){
                        if(n->id == alignScore->iIndex - 1){
                            for(Edge* e : (*graph).edges){
                                if(e->src == n && succNode->id >= 0){
                                    (*graph).createEdge(n, newNode);
                                    newNode->predecessors.push_back(n);
                                    delete succNode;
                                    succNode = new Node;
                                }
                            }
                        }
                    }
                    mm = false;
                }
            }else{
                //mismatch
                //add new node with same predecessor as node that he mismatches with
                newNode = (*graph).createNode(target[alignScore->jIndex - 1], target_id, alignScore->jIndex - 1);
                if(insertionNode->id >= 0){
                    (*graph).createEdge(newNode, insertionNode);
                    insertionNode->predecessors.push_back(newNode);
                    insertionNode = new Node;
                }
                mm = true;
                if(succNode->id >= 0){
                    for(Edge* e : (*graph).edges){
                        if(e->src->id == succNode->id){
                            (*graph).createEdge(newNode, e->src);
                            e->src->predecessors.push_back(newNode);
                        }
                    }
                    *succNode = *newNode;
                }else{
                    for(Node* n : (*graph).nodes){
                        if(n->id == alignScore->iIndex - 1){
                            for(Edge* e : (*graph).edges){
                                if(e->src == n){
                                    (*graph).createEdge(newNode, e->dest);
                                    e->dest->predecessors.push_back(newNode);
                                    *succNode = *newNode;
                                }else if(e->dest == n){
                                    bool add = true;
                                    for(Edge* e : (*graph).edges){
                                        if(e->src == n){
                                            add = false;
                                        }
                                    }
                                    if(add){
                                        (*graph).createEdge(e->src, newNode);
                                        newNode->predecessors.push_back(e->src);
                                    }
                                }
                            }
                        }
                    }
                }  
            }
        }
        alignScore = alignScore->comingFrom;
    }
    (*graph).topologicalSort();
    //(*graph).print();
    reverse(tmp.begin(), tmp.end());
    if(cigar != nullptr){
        *cigar = "";
        char c = tmp.at(0);
        int n = 1;
        for(int i = 1; i < tmp.size(); i++){
            if(tmp.at(i) == c){
                n++;
            }else{
                *cigar = *cigar + to_string(n) + c;
                n = 1;
                c = tmp.at(i);
            }
        }
        *cigar = *cigar + to_string(n) + c;
    }
    for (int i = 0; i <= cellNumber; i++) {
        delete[] V[i];
    }
    delete[] V;
}
void start(const char *seq11, unsigned int seq11_len, const char *seq22, unsigned int seq22_len){
    /* (*graph).createGraph(seq1, sizeof(seq1), 1);
    (*graph).topologicalSort();
    string cigar = "";
    AlignToGraph(seq2, seq2_len, 2, global, 1, -1, -2, &cigar);
    cout << cigar << endl; */
    int seq_id = 0;
    char seq1[] = {'G', 'T', 'A'};
    (*graph).createGraph(seq1, sizeof(seq1), ++seq_id);
    (*graph).topologicalSort();

    char seq2[] = {'A', 'G', 'C', 'A'};
    string cigar1 = "";
    AlignToGraph(seq2, sizeof(seq2), ++seq_id, global, 1, -1, -2, &cigar1);
    //cout << cigar1 << endl;
    //(*graph).print();
    
    char seq3[] = {'G', 'T', 'A', 'T'};
    string cigar2 = "";
    AlignToGraph(seq3, sizeof(seq3), ++seq_id, global, 1, -1, -2, &cigar2);
    //cout << cigar2 << endl;

    char seq4[] = {'G', 'T', 'A', 'C'};
    string cigar3 = "";
    AlignToGraph(seq4, sizeof(seq4), ++seq_id, global, 1, -1, -2, &cigar3);
    //cout << cigar3 << endl;

    std::ofstream file("text.gfa");
    file << "Napisano" << std::endl;
    file.close();

    //create GFA file
    GFA* gfa = new GFA("text.gfa", 1.0);
    (*gfa).headerLine();
    (*gfa).addSequence(seq1, sizeof(seq1), 1);
    (*gfa).addSequence(seq2, sizeof(seq2), 2);
    (*gfa).addSequence(seq3, sizeof(seq3), 3);
    (*gfa).addSequence(seq4, sizeof(seq4), 4);
    (*gfa).segmentLine();
    (*gfa).addLink(1, 2, cigar1);
    (*gfa).addLink(12, 3, cigar2);
    (*gfa).addLink(123, 4, cigar3);
    (*gfa).linkLine();
    (*gfa).pathLine(seq_id + 1);
    (*gfa).findPath(graph);
    (*gfa).calculateConsensus((*graph).graph_length, seq_id);
    (*gfa).printPath();
    (*gfa).close();
    delete gfa;
}
/* int main(){
    char seq1[] = {'A', 'G', 'C', 'T', 'G', 'C', 'A', 'T'};
    char seq2[] = {'G', 'C', 'A', 'G', 'C', 'G'};
    //char seq1[] = {'C', 'C', 'G', 'C', 'T', 'T', 'T', 'T', 'C', 'C', 'G', 'C'};
    //char seq2[] = {'C', 'C', 'G', 'C', 'A', 'A', 'A', 'A', 'C', 'C', 'G', 'C'};
    graph.createGraph(seq1, sizeof(seq1));
    graph.topologicalSort();
    //graph.print();
    AlignToGraph(seq2, sizeof(seq2), global, 1, -1, -2);
    cout << endl;
    return 0;
} */