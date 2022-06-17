//
// Red Black Binary Search Tree Implementation.
// Using left-leaning version, avoid lots of cases, less code, easy to
// understand More info about left-leaning RedBlackTree:
// https://sedgewick.io/talks/#ll-red-black-trees
//
#pragma once

#include <functional>
#include <iostream>
#include <map>

#include "./utils/colors.hpp"
#include "./utils/printbst.hpp"

namespace structalgo {

enum class Color { Red = 0, Black = 1 };

template <typename K, typename V>
struct Node {
    K key;
    V val;
    Node *left, *right;
    Color color;
    Node(K k, V v, Color c)
        : key(k), val(v), color(c), left(nullptr), right(nullptr) {}
    void print() {
        std::string color = (this->color == Color::Black) ? BOLDBLACK : BOLDRED;
        printf("%s[%d]%s", color.c_str(), this->val, RESET);
    }
};

template <typename K, typename V, typename Cmp = std::less<K>>
class RedBlackTree {
   public:
    using Node = Node<K, V>;

    RedBlackTree() : root(nullptr) {}

    V get(Node* node, K key) {
        Node* x = node;
        while (x != nullptr) {
            if (key == x->key) {
                return x->val;
            } else if (cmp(key, x->key)) {
                x = x->left;
            } else {
                x = x->right;
            }
        }
        return -1;
    }
    void put(K key, V val) {
        root = insert(root, key, val);
        root->color = Color::Black;
    }
    void put(K key) {
        root = insert(root, key, key);
        root->color = Color::Black;
    }
    void deleteNode(K key) {
        root = deleteNode(root, key);
        root->color = Color::Black;
    }
    void deleteMin() {
        root = deleteMin(root);
        root->color = Color::Black;
    }
    void deleteMax() {
        root = deleteMax(root);
        root->color = Color::Black;
    }
    void printTree() { printBST("", root, false); }

   private:
    K min(Node* node) {
        while (node->left) {
            node = node->left;
        }
        return node->key;
    }
    K max(Node* node) {
        while (node->right) {
            node = node->right;
        }
        return node->key;
    }
    bool isRed(Node* x) {
        if (x == nullptr) return false;
        return x->color == Color::Red;
    }

    Color flip(Color color) {
        return (color == Color::Black) ? Color::Red : Color::Black;
    }

    Node* rotateLeft(Node* h) {
        Node* x = h->right;
        h->right = x->left;
        x->left = h;
        x->color = x->left->color;
        x->left->color = Color::Red;
        return x;
    }

    Node* rotateRight(Node* h) {
        Node* x = h->left;
        h->left = x->right;
        x->right = h;
        x->color = x->right->color;
        x->right->color = Color::Red;
        return x;
    }

    Node* colorFlip(Node* h) {
        h->color = flip(h->color);
        h->left->color = flip(h->left->color);
        h->right->color = flip(h->right->color);
        return h;
    }

    Node* fixUp(Node* h) {
        // keep rbt left leaning and flip color for node two red child
        if (isRed(h->right)) {
            h = rotateLeft(h);
        }
        if (isRed(h->left) && isRed(h->left->left)) {
            h = rotateRight(h);
        }
        if (isRed(h->left) && isRed(h->right)) {
            colorFlip(h);
        }
        return h;
    }

    //
    // insert
    //
    Node* insert(Node* h, K key, V val) {
        if (h == nullptr) {
            return new Node(key, val, Color::Red);
        }
        if (key == h->key)
            h->val = val;
        else if (cmp(key, h->key)) {
            h->left = insert(h->left, key, val);
        } else {
            h->right = insert(h->right, key, val);
        }
        return fixUp(h);
    }

    //
    // delete
    //

    // deleteMax
    Node* moveRedRight(Node* h) {
        colorFlip(h);
        if (isRed(h->left->left)) {
            h = rotateRight(h);
            colorFlip(h);
        }
        return h;
    }

    Node* deleteMax(Node* h) {
        if (isRed(h->left)) {
            h = rotateRight(h);
        }
        if (h->right == nullptr) {
            return nullptr;
        }
        if (!isRed(h->right) && !isRed(h->right->left)) {
            h = moveRedRight(h);
        }
        h->right = deleteMax(h->right);
        return fixUp(h);
    }

    // deleteMin
    Node* moveRedLeft(Node* h) {
        colorFlip(h);
        if (isRed(h->right->left)) {
            h->right = rotateRight(h->right);
            h = rotateLeft(h);
            colorFlip(h);
        }
        return h;
    }

    Node* deleteMin(Node* h) {
        if (h->left == nullptr) {
            return nullptr;
        }
        if (!isRed(h->left) && !isRed(h->left->left)) {
            h = moveRedLeft(h);
        }
        h->left = deleteMin(h->left);
        return fixUp(h);
    }

    // deleteNode
    Node* deleteNode(Node* h, K key) {
        if (cmp(key, h->key)) {
            if (!isRed(h->left) && !isRed(h->left->left)) {
            }
            h->left = deleteNode(h->left, key);
        } else {
            if (isRed(h->left)) {
                h = rotateRight(h);
            }
            if (key == h->key && (h->right == nullptr)) {
                return nullptr;
            }
            if (!isRed(h->right) && !isRed(h->right->left)) {
                h = moveRedRight(h);
            }
            if (key == h->key) {
                // This is exactly Chinese idiom "借尸还魂"
                h->key = min(h->right);
                h->val = get(h->right, h->key);
                h->right = deleteMin(h->right);
            } else {
                h->right = deleteNode(h->right, key);
            }
        }
        return fixUp(h);
    }

   private:
    Node* root;
    Cmp cmp;
};

}  // namespace structalgo
