// 
// This file is used for print binary search tree in terminal in a human visible way.
// I use preorder traversal and some symbol to do this, every node in a single line.
// Hopefully, this makes algorithms of BST easy debug.
//
#pragma once

#include <iostream>

template <typename N>
void printBST(const std::string& prefix, N* node, bool isLeft) {
    if (node != nullptr) {
        std::cout << prefix;
        std::cout << (isLeft ? "├──" : "└──");
        node->print();
        std::cout << '\n';
        printBST(prefix + (isLeft ? "│   " : "    "), node->left, true);
        printBST(prefix + (isLeft ? "│   " : "    "), node->right, false);
    }
}
