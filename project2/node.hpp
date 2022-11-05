#include <string>
#include <iostream>
#include <vector>
#include <functional>

enum class Node_TYPE {
    LINE,
    NAME,
    STRING,
    CHAR,
    INT,
    FLOAT,
    NOTHING
};
using namespace std;
class Node {
public:
    bool is_key;
    string name;
    string value;
    vector<Node *> sub_nodes;
    int line_num;
    bool is_terminal  = true;
    bool is_empty = false;

    Node(bool is_key, string name, string value)
    {
        this->is_key = is_key;
        this->name = name;
        this->value = value;
    }
    Node(string name, int line_num)
    {
        this->is_terminal = false;
        this->name = name;
        this->line_num = line_num;
    }
    Node()
    {
        this->is_empty = true;
    }
    ~Node() = default;

    void add_sub(Node *sub_node) {
        this->sub_nodes.push_back(sub_node);
    }
};