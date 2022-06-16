#include "../llredblack.h"

int main() {
    RedBlackTree<int, int> rbt;
    rbt.put(1);
    rbt.put(2);
    rbt.put(3);
    rbt.put(4);
    rbt.put(5);
    rbt.put(100);
    rbt.put(10);
    rbt.put(-10);
    rbt.put(923);
    rbt.put(23);
    rbt.printTree();
    // for (int i = 0; i < 9; ++i) {
    //     rbt.deleteMax();
    //     rbt.printTree();
    // }
    rbt.deleteNode(5);
    rbt.deleteNode(2);
    rbt.printTree();

    std::cout << !(-1);
    
    
    return 0;
}
