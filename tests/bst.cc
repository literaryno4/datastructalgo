#include "../bst.hpp"

using namespace structalgo;

int main() {
    BST<int, int> bst;
    bst.put(3);
    bst.put(2);
    bst.put(5);
    bst.put(1);
    bst.put(4);
    bst.put(2);
    bst.put(5);
    bst.print();
    bst.deleteNode(2);
    bst.print();
    bst.deleteNode(5);
    bst.print();

}
