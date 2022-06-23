#include <thread>
#include <iostream>
#include "../threadsafe_queue.hpp"

using namespace structalgo;

template <typename T>
void print(ThreadsafeQueue<T> tq) {

}

ThreadsafeQueue<int> tq;

void threadPush() {
    tq.push(3);
    tq.push(5);
    tq.push(6);
}

void threadPush2() {
    tq.push(7);
    tq.push(9);
    tq.push(0);
}

void threadPop() {
    printf("popped: %d\n", *tq.tryPop()->data);
    printf("popped: %d\n", *tq.tryPop()->data);
    printf("popped: %d\n", *tq.tryPop()->data);
}

void threadPop2() {
    printf("waitpopped: %d\n", *tq.waitAndPop());
    printf("waitpopped: %d\n", *tq.waitAndPop());
    printf("waitpopped: %d\n", *tq.waitAndPop());
}
int main() {
    std::thread t1(threadPush);
    std::thread t2(threadPush2);
    std::thread t3(threadPop);
    std::thread t4(threadPop2);

    std::cout << tq.empty() << '\n';

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
