#ifndef AVLTREE_H
#define AVLTREE_H

#include <vector>
#include <utility>

class AVLTree {
private:
    struct Node {
        int bookId;
        int count;
        Node* left;
        Node* right;
        int height;
        Node(int id) : bookId(id), count(1), left(nullptr), right(nullptr), height(1) {}
    };
    Node* root;

    Node* insert(Node* node, int bookId);
    Node* find(Node* node, int bookId);
    int getHeight(Node* node);
    int getBalance(Node* node);
    Node* rotateRight(Node* y);
    Node* rotateLeft(Node* x);
    void inOrder(Node* node, std::vector<std::pair<int, int>>& result);
    void topN(Node* node, std::vector<std::pair<int, int>>& result, int N);
    void destroy(Node* node);

public:
    AVLTree();
    ~AVLTree();
    void increment(int bookId);
    int getCount(int bookId);
    std::vector<std::pair<int, int>> getAll();
    std::vector<std::pair<int, int>> getTopN(int N);
};

#endif
