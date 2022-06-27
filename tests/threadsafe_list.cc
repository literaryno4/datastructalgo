#include <thread>
#include <iostream>
#include "../threadsafe_list.hpp"

using namespace structalgo;

ThreadsafeList<int> tl;

void printList() {
    tl.forEach([](auto& data) {
                   printf("%d\n", data);
               });
}

void threadpushFront() {
    tl.pushFront(3);
    tl.pushFront(5);
    tl.pushFront(6);
}

void threadpushFront2() {
    tl.pushFront(7);
    tl.pushFront(9);
    tl.pushFront(0);
}

void threadPop() {
    printf("removeing 5\n");
    tl.remove(7);
    printf("removeing 9\n");
    tl.remove(9);
    printf("removeing 0\n");
    tl.remove(0);
    // tl.insertAfter(9, 8);
    // tl.insertAfter(6, 2);
}

void threadPop2() {
    printf("removeing 3\n");
    tl.remove(3);
    printf("removeing 5\n");
    tl.remove(5);
    printf("removeing 6\n");
    tl.remove(6);
}

int main() {
    std::thread t1(threadpushFront);
    std::thread t2(threadpushFront2);
    std::thread t3(threadPop);
    std::thread t4(threadPop2);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    printList();

    return 0;
}
