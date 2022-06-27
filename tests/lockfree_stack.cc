#include "../lockfree_stack.hpp"
#include <thread>

LockfreeStack<int> ls;

int main() {
    std::thread t1([&]() {
                       ls.push(1);
                       ls.push(2);
                       ls.push(3);
                   });
    std::thread t2([&]() {
                  });
    std::thread t3([&]() {
                       for (int i = 0; i < 3; ++i) {
                           auto p = ls.pop();
                           if (p) {
                                printf("poping: %d in thread 3\n", *p);
                           }
                       }
                   });
    std::thread t4([&]() {
                       for (int i = 0; i < 4; ++i) {
                           auto p = ls.pop();
                           if (p) {
                                printf("poping: %d in thread 4\n", *p);
                           }
                       }
                   });

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
