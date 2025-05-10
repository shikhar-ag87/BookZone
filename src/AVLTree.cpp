#include "AVLTree.h"
#include <algorithm>

AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() {
    destroy(root);
}

void AVLTree::destroy(Node* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

int AVLTree::getHeight(Node* node) {
    return node ? node->height : 0;
}

int AVLTree::getBalance(Node* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

AVLTree::Node* AVLTree::rotateRight(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    return x;
}

AVLTree::Node* AVLTree::rotateLeft(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    return y;
}

AVLTree::Node* AVLTree::insert(Node* node, int bookId) {
    if (!node) return new Node(bookId);
    if (bookId < node->bookId)
        node->left = insert(node->left, bookId);
    else if (bookId > node->bookId)
        node->right = insert(node->right, bookId);
    else {
        node->count++;
        return node;
    }

    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    // Left Left
    if (balance > 1 && bookId < node->left->bookId)
        return rotateRight(node);
    // Right Right
    if (balance < -1 && bookId > node->right->bookId)
        return rotateLeft(node);
    // Left Right
    if (balance > 1 && bookId > node->left->bookId) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    // Right Left
    if (balance < -1 && bookId < node->right->bookId) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

AVLTree::Node* AVLTree::find(Node* node, int bookId) {
    if (!node) return nullptr;
    if (bookId == node->bookId) return node;
    if (bookId < node->bookId) return find(node->left, bookId);
    return find(node->right, bookId);
}

void AVLTree::increment(int bookId) {
    root = insert(root, bookId);
}

int AVLTree::getCount(int bookId) {
    Node* node = find(root, bookId);
    return node ? node->count : 0;
}

void AVLTree::inOrder(Node* node, std::vector<std::pair<int, int>>& result) {
    if (!node) return;
    inOrder(node->left, result);
    result.push_back({ node->bookId, node->count });
    inOrder(node->right, result);
}

std::vector<std::pair<int, int>> AVLTree::getAll() {
    std::vector<std::pair<int, int>> result;
    inOrder(root, result);
    return result;
}

void AVLTree::topN(Node* node, std::vector<std::pair<int, int>>& result, int N) {
    if (!node) return;
    topN(node->left, result, N);
    result.push_back({ node->bookId, node->count });
    topN(node->right, result, N);
}

std::vector<std::pair<int, int>> AVLTree::getTopN(int N) {
    std::vector<std::pair<int, int>> all;
    topN(root, all, N);
    std::sort(all.begin(), all.end(), [](auto& a, auto& b) { return a.second > b.second; });
    if ((int)all.size() > N) all.resize(N);
    return all;
}