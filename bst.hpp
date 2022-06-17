//
// Binary Search Tree Implementation.
//
#pragma once

#include <functional>
#include <iostream>

#include "./utils/printbst.hpp"

namespace structalgo {

template <typename K, typename V>
struct Node {
    K key;
    V val;
    Node *left, *right;
    Node(K k, V v) : key(k), val(v), left(nullptr), right(nullptr) {}
    void print() { std::cout << "[" << val << "]"; }
};

template <typename K, typename V, typename Cmp = std::less<K>>
class BST {
   public:
    using Node = Node<K, V>;

    BST() : root(nullptr) {}
    V get(Node* node, K key) {
        if (node == nullptr) {
            throw;
        }
        if (key == node->key) {
            return node->val;
        } else if (cmp(key, node->key)) {
            return get(node->left, key);
        } else {
            return get(node->right, key);
        }
    }
    void put(K key, V val) { root = insert(root, key, val); }
    void put(K key) { root = insert(root, key, key); }
    void deleteMin() { root = deleteMin(root); }
    void deleteMax() { root = deleteMax(root); }
    void deleteNode(K key) { root = deleteNode(root, key); }
    void print() { printBST("", root, false); }

   private:
    K min(Node* node) {
        if (node->left == nullptr) {
            return node->key;
        }
        return min(node->left);
    }
    K max(Node* node) {
        if (node->right == nullptr) {
            return node->key;
        }
        return max(node->right);
    }

    //
    // insert
    //
    Node* insert(Node* node, K key, V val) {
        if (node == nullptr) {
            return new Node(key, val);
        }
        if (cmp(key, node->key)) {
            node->left = insert(node->left, key, val);
        } else {
            node->right = insert(node->right, key, val);
        }
        return node;
    }

    //
    // delete
    //
    Node* deleteMin(Node* node) {
        if (node->left) {
            node->left = deleteMin(node->left);
        } else if (node->right == nullptr) {
            return nullptr;
        } else {
            node->key = min(node->right);
            node->val = get(node->right, node->key);
            node->right = deleteMin(node->right);
        }
        return node;
    }
    Node* deleteMax(Node* node) {
        if (node->right) {
            node->right = deleteMax(node->right);
        } else if (node->left == nullptr) {
            return nullptr;
        } else {
            node->key = max(node->left);
            node->val = get(node->right, node->key);
            node->left = deleteMax(node->left);
        }
        return node;
    }
    Node* deleteNode(Node* node, K key) {
        if (key == node->key) {
            if (node->left == nullptr && node->right == nullptr) {
                return nullptr;
            } else if (node->left == nullptr) {
                node->key = min(node->right);
                node->val = get(node->right, node->key);
                node->right = deleteMin(node->right);
            } else {
                node->key = max(node->left);
                node->val = get(node->left, node->key);
                node->left = deleteMax(node->left);
            }
        } else if (cmp(key, node->key)) {
            node->left = deleteNode(node->left, key);
        } else {
            node->right = deleteNode(node->right, key);
        }
        return node;
    }

   private:
    Node* root;
    Cmp cmp;
};

}  // namespace structalgo
