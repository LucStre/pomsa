#include <iostream>
#include <vector>
#include <list>
#include <algorithm>

struct Node{
    int id;
    char value;
    int predCount = 0;
    std::vector<Node *> predecessors;
    //id of sequence and position index of each letter in sequence for pomsa
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
};

class Graph{

public:
    std::vector<Node*> nodes;
    std::list<Edge*> edges;
    unsigned int nodes_number = 0;
    int ID = 0;

    Graph() {}

    void createGraph(const char *seq, unsigned int seq_len){
        nodes_number = seq_len;
        Node *current;
        Node *prev;
        Edge *edge;
        for (int i = 0; i < seq_len; i++){
            current = new Node;
            current->id = ID++;
            current->value = seq[i];
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

    void addPredecessors(){
        for(Edge *e : edges){
            e->dest->predecessors.push_back(e->src);
        }
    }

    void topologicalSort(){
        std::vector<Node*> sortedNodes;
        std::list<Node*> setNodes;
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
    }

    Node* createNode(char value){
        Node *node = new Node;
        node->id = ID++;
        node->value = value;
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

    void addNode(Node *node){
        nodes.push_back(node);
    }

    void reverseNodes(){
        std::reverse(nodes.begin(), nodes.end());
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
    }
};