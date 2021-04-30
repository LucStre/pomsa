#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <tuple>
#include "../include/graph/graph.hpp"

using namespace std;

enum AlignmentType{
    global,
    local,
    semiglobal
};
enum ComingFromDirection{
    fromNone,
    fromLeft,
    fromTop,
    fromDiagonal
};

int MATCH = 0, INSERTION = 0, DELETION = 0;
Graph graph, graphNew;

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
    int k = 0;
    if (type == global){
        V[0][0].value = 0;
        V[0][0].iIndex = 0;
        V[0][0].iIndex = 0;
        for (int i = 1; i <= graph.nodes_number; i++){
            V[i][0].value = gap * i;
            V[i][0].comingFrom = &V[i - 1][0];
            V[i][0].iIndex = i;
            V[i][0].iIndex = 0;
        }
        for (int j = 1; j <= target_len; j++){
            V[0][j].value = gap * j;
            V[0][j].comingFrom = &V[0][j - 1];
            V[0][j].iIndex = 0;
            V[0][j].iIndex = j;
        }
        k++;
    }
    for (int i = k; i <= graph.nodes_number; i++){
        for (int j = k; j <= target_len; j++){
            V[i][j].value = 0;
            if (j == 0){
                V[i][j].comingFrom = &V[i - 1][j];
                V[i][j].iIndex = j;
                V[i][j].jIndex = j;
            }
            if (i == 0){
                V[i][j].comingFrom = &V[i][j - 1];
                V[i][j].iIndex = i;
                V[i][j].jIndex = j - 1;
            }
        }
    }
    int number = 0;
    Cell *cell;
    for (int j = 1; j <= target_len; j++){
        number = 1;
        for (vector<Node*>::iterator it = graph.nodes.begin(); it != graph.nodes.end(); it++, number++)
        {
            cell = &V[number - 1][j];
            cell->value = V[number - 1][j].value;
            cell->iIndex = number - 1;
            cell->jIndex = j;
            V[number][j].predecessors.push_back(cell);

            cell = &V[number - 1][j - 1];
            cell->value = V[number - 1][j - 1].value;
            cell->iIndex = number - 1;
            cell->jIndex = j - 1;
            V[number][j].predecessors.push_back(cell);
        }
    }
}

void AlignToGraph(const char *target, unsigned int target_len, AlignmentType type, int match, int mismatch, int gap)
{
    V = new Cell *[graph.nodes_number + 1];
    for (unsigned int i = 0; i <= graph.nodes_number; i++){
        V[i] = new Cell[target_len + 1];
    }

    initialization(type, target_len, gap);

    int maxMatch = 0, maxInsert = 0;
    Cell *matchCell, *insertionCell, *deletionCell;
    
    for (int i = 1; i <= graph.nodes_number; i++){
        for (int j = 1; j <= target_len; j++){
            bool inserted = false, matched = false;
            for (vector<Cell *>::iterator it = V[i][j].predecessors.begin(); it != V[i][j].predecessors.end(); it++){
                if ((*it)->jIndex == j - 1){
                    maxMatch = (*it)->value + ((graph.nodes[i - 1]->value == target[j - 1]) ? match : mismatch);
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
    if (type == local){
        for (int i = 1; i <= graph.nodes_number; i++){
            for (int j = 1; j <= target_len; j++){
                if (V[i][j].value > alignScore->value){
                    alignScore = &V[i - 1][j - 1];
                    alignScore->iIndex = i - 1;
                    alignScore->jIndex = j - 1;
                }
            }
        }
    }else if (type == semiglobal){
        for (int i = 1; i < graph.nodes_number; i++){
            if (V[i][target_len].value > alignScore->value){
                alignScore = &V[i][target_len];
                alignScore->iIndex = i;
                alignScore->jIndex = target_len;
            }
        }
        for (int j = 1; j < target_len; j++){
            if (V[graph.nodes_number][j].value > alignScore->value){
                alignScore = &V[graph.nodes_number][j];
                alignScore->iIndex = graph.nodes_number;
                alignScore->jIndex = j;
            }
        }
    }else{
        alignScore = &V[graph.nodes_number][target_len];
        alignScore->iIndex = graph.nodes_number;
        alignScore->jIndex = target_len;
    }
    
    list<tuple<Node *, int>> succ;
    vector<tuple<Node *, int>> nodes;
    list<tuple<Node *, int>> removed;
    while (alignScore->comingFrom != nullptr){
        if (alignScore->comingFrom->iIndex == alignScore->iIndex && alignScore->comingFrom->jIndex < alignScore->jIndex){
            //fromLeft
            nodes.push_back(make_tuple(graphNew.createNode(target[alignScore->jIndex - 1]), false));
            if(!succ.empty()){
                for(tuple<Node*, int> s : succ){
                    for(tuple<Node*, int> n : nodes){
                        if(get<1>(s) == 0){
                            graphNew.createEdge(get<0>(n), get<0>(s));
                        }
                        if(alignScore->comingFrom->comingFrom == nullptr){
                            graphNew.createEdge(get<0>(n), get<0>(s));
                        }
                    }
                }
                for(tuple<Node*, int> s : succ){
                    if(get<1>(s) == 0){
                        removed.push_back(s);
                    }
                }
            }
            for(tuple<Node *, int> r : removed){
                succ.remove(r);
            }
            removed.clear();
            for(tuple<Node*, int> n : nodes){
                succ.push_back(n);
            }
            nodes.clear();
        }
        if (alignScore->comingFrom->jIndex == alignScore->jIndex && alignScore->comingFrom->iIndex < alignScore->iIndex){
            //fromTop
            nodes.push_back(make_tuple(graphNew.createNode(graph.nodes[alignScore->iIndex - 1]->value), false));
            if(!succ.empty()){
                for(tuple<Node*, int> s : succ){
                    for(tuple<Node*, int> n : nodes){
                        if(get<1>(s) == 1){
                            graphNew.createEdge(get<0>(n), get<0>(s));
                        }
                        if(alignScore->comingFrom->comingFrom == nullptr){
                            graphNew.createEdge(get<0>(n), get<0>(s));
                        }
                    }
                }
                for(tuple<Node*, int> s : succ){
                    if(get<1>(s) == 1){
                        removed.push_back(s);
                    }
                }
            }
            for(tuple<Node *, int> r : removed){
                succ.remove(r);
            }
            removed.clear();
            for(tuple<Node*, int> n : nodes){
                succ.push_back(n);
            }
            nodes.clear();
        }
        if (alignScore->comingFrom->jIndex < alignScore->jIndex && alignScore->comingFrom->iIndex < alignScore->iIndex){
            //fromDiagonal
            if(graph.nodes[alignScore->iIndex - 1]->value == target[alignScore->jIndex - 1]){
                //match
                //dodajem samo jedan čvor
                nodes.push_back(make_tuple(graphNew.createNode(graph.nodes[alignScore->iIndex - 1]->value), 2));
                if(!succ.empty()){
                    for(tuple<Node*, int> s : succ){
                        for(tuple<Node*, int> n : nodes){
                            graphNew.createEdge(get<0>(n), get<0>(s));
                        }
                    }
                    succ.clear();
                }
                for(tuple<Node*, int> n : nodes){
                    succ.push_back(n);
                }
                nodes.clear();
            }else{
                //mismatch
                //dodajem dva čvora koji imaju istog prethodnika
                nodes.push_back(make_tuple(graphNew.createNode(graph.nodes[alignScore->iIndex - 1]->value), true));
                nodes.push_back(make_tuple(graphNew.createNode(target[alignScore->jIndex - 1]), false));
                if(!succ.empty()){
                    for(tuple<Node*, int> s : succ){
                        for(tuple<Node*, int> n : nodes){
                            if(get<1>(s) == 2){
                                graphNew.createEdge(get<0>(n), get<0>(s));
                            }else{
                                if(get<1>(s) == get<1>(n)){
                                    graphNew.createEdge(get<0>(n), get<0>(s));
                                }
                            }
                        }
                    }
                    succ.clear();
                }
                for(tuple<Node*, int> n : nodes){
                    succ.push_back(n);
                }
                nodes.clear();
            }
        }
        alignScore = alignScore->comingFrom;
    }
    cout << endl;
    graphNew.reverseNodes();
    graphNew.addPredecessors();
    graphNew.topologicalSort();
    graphNew.print();
}
void start(const char *seq1, unsigned int seq1_len, const char *seq2, unsigned int seq2_len){
    graph.createGraph(seq1, sizeof(seq1));
    graph.topologicalSort();
    //graph.print();
    AlignToGraph(seq2, sizeof(seq2), global, 1, -1, -2);
    cout << endl;
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