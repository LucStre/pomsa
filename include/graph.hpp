#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <tuple>
#include <string>

struct Node{
    int id = -1;
    char value;
    int predCount = 0;
    int weight = 0;
    std::vector<Node *> predecessors;
    std::vector<std::tuple<int, int>> sequence;
    bool operator<(const Node &other) const{
        return value < other.value;
    };

    bool operator>(const Node &other) const{
        return value > other.value;
    };

    bool operator==(const Node &other) const{
        return id == other.id;
    };
};

struct Edge{
    Node *src, *dest;
    int weight = 0;
};

class Graph{

private:
    unsigned int ID = 0;

public:
    std::vector<Node*> nodes;
    std::list<Edge*> edges;
    unsigned int nodes_number = 0;
    unsigned int graph_length = 0;
    

    Graph() {}

    void createGraph(const char *seq, unsigned int seq_len, int seq_id){
        nodes_number = seq_len;
        Node *current;
        Node *prev;
        Edge *edge;
        for (int i = 0; i < seq_len; i++){
            current = new Node;
            current->id = ID++;
            current->value = seq[i];
            current->sequence.push_back(std::make_tuple(seq_id, i));
            graph_length++;
            if (i > 0){
                edge = new Edge;
                edge->src = prev;
                edge->dest = current;
                edges.push_back(edge);
            }
            prev = current;
            nodes.push_back(current);
        }
        addPredecessors();
    }

    std::list<Edge*> sortedEdges;
    std::vector<Node*> sortedNodes;
    std::list<Node*> setNodes;

    void addPredecessors(){
        for(Edge *e : edges){
            e->dest->predecessors.push_back(e->src);
        }
    }

    void deletePredecessor(Node *src, Node *pred){
        for(std::vector<Node*>::iterator it = src->predecessors.begin(); it != src->predecessors.end(); it++){
            if((*it) == pred){
                src->predecessors.erase(it);
            }
        }
        
    }

    void topologicalSort(){
        Node *n;
        bool visited[nodes_number] = {false};
        for (Node *node : nodes){
            if(node->predecessors.size() == 0){
                setNodes.push_back(node);
                visited[node->id] = true;
            }
        }
        while (!setNodes.empty()){
            n = setNodes.front();
            setNodes.pop_front();
            sortedNodes.push_back(n);
            for (Edge *e : edges){
                if (e->src->id == n->id){
                    e->dest->predCount++;
                    if (e->dest->predCount == e->dest->predecessors.size()){
                        setNodes.push_back(e->dest);
                        visited[e->dest->id] = true;
                    }
                }
            }
        }
        ID = 0;
        for(Node *node : sortedNodes){
            node->id = ID++;
            node->predCount = 0;
        }
        nodes = sortedNodes;
        sortedNodes.clear();
        setNodes.clear();
        sortEdges();
    }

    Node* createNode(char value, int seq_id, int position){
        Node *node = new Node;
        node->id = ID++;
        node->value = value;
        node->sequence.push_back(std::make_tuple(seq_id, position));
        nodes.push_back(node);
        nodes_number++;
        return node;
    }

    void createEdge(Node *src, Node *dest){
        Edge *edge = new Edge;
        edge->src = src;
        edge->dest = dest;
        edges.push_back(edge);
    }

    void deleteEdge(Edge* e){
        edges.remove(e);
    }

    void addNode(Node *node){
        nodes.push_back(node);
    }

    void reverseNodes(){
        std::reverse(nodes.begin(), nodes.end());
    }

    void sortEdges(){
        for(Node *n : nodes){
            for(Edge *e : edges){
                if(n == e->src){
                    sortedEdges.push_back(e);
                }
            }
        }
        edges = sortedEdges;
        sortedEdges.clear();
    }

    void calculateConsensus(){
        for(Edge *e : edges){
            e->weight = 0;
            e->src->predCount = 1;
        }
        for(Node *n : nodes){
            n->weight = 0;
            for(Edge *e : edges){
                if(n == e->src){
                    e->weight += n->sequence.size();
                }
            }
        }
        int max = -1;
        for(Edge *e1 : edges){
            for(Edge *e2 : edges){
                if(e1->src == e2->dest){
                    if(e1->src->predecessors.size() == e1->src->predCount){
                        e1->weight += max == -1 ? e2->weight : max;
                        e1->dest->weight = e1->weight + e1->src->weight;
                        max = -1;
                    }else{
                        e1->src->predCount++;
                        if(e2->weight > max){
                            max = e2->weight;
                        }
                    }
                }else if(e2->src->predecessors.size() == 0){
                    e2->dest->weight = e2->weight;
                }
            }
        }
        max = -1;
        Node *maxNode;
        for(Node *e : nodes){
            e->predCount = 1;
            if(e->weight > max){
                max = e->weight;
                maxNode = e;
            }
        }
        std::string consensusSequence = "";
        max = -1;
        Node *tmp = nullptr;
        for(Edge *e : edges){
            if(maxNode == e->src || tmp == e->src){
                for(Node *n: nodes){
                    if(n != maxNode){
                        n->weight = -1;
                    }
                }
                tmp = e->dest;
                tmp->weight = e->src->weight + e->weight;
                consensusSequence += tmp->value;
            }
        }
        while(maxNode->predecessors.size() != 0){
            for(Edge *e : edges){
                if(maxNode == e->dest){
                    if(maxNode->predecessors.size() == maxNode->predCount){
                        consensusSequence += maxNode->value;
                        maxNode = tmp == nullptr ? e->src : tmp;
                        tmp = nullptr;
                    }else{
                        maxNode->predCount++;
                        if(max < e->weight){
                            max = e->weight;
                            tmp = e->src;
                        }
                    }
                    
                }
            }
        }
        consensusSequence += maxNode->value;
        reverse(consensusSequence.begin(), consensusSequence.end());
        std::cout << "Consensus sequence: " << consensusSequence << std::endl;
    }

    void print(){
        for (Node *n : nodes){
            std::cout << n->value << "(" << n->id << ") ";
            for (std::vector<Node *>::iterator it = n->predecessors.begin(); it != n->predecessors.end(); ++it){
                std::cout << (*it)->value << "(" << (*it)->id << ") ";
            }
            std::cout << std::endl;
        }
        for (Edge *e : edges){
            std::cout << e->src->value << "(" << e->src->id << ")"
                                                             "->"
                      << e->dest->value << "(" << e->dest->id << ") ";
        }
        std::cout << std::endl;
        for (Node *n : nodes){
            std::cout << n->value << "(" << n->id << ") ";
            for (std::vector<std::tuple<int, int>>::iterator it = n->sequence.begin(); it != n->sequence.end(); it++){
                std::cout << "(" << std::get<0>(*it) << ", " << std::get<1>(*it) << ") ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};