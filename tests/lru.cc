#include "../lru.h"

int main() {
    LRU<int, int> lru(5);
    lru.put(3, 3);
    lru.put(4, 4);
    lru.put(5, 5);
    lru.put(6, 6);
    lru.put(7, 7);
}


