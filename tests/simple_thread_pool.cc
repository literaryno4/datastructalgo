#include "../utils/simple_thread_pool.hpp"

using namespace structalgo;

void task1() {
    auto start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(0));
    auto stop = std::chrono::steady_clock::now();
    printf("task1 done! within %f ms\n", std::chrono::duration<double, std::milli>(stop - start).count());
}

void task2() {
    auto start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(0));
    auto stop = std::chrono::steady_clock::now();
    printf("task2 done! within %f ms\n", std::chrono::duration<double, std::milli>(stop - start).count());
}

void task3() {
    auto start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(0));
    auto stop = std::chrono::steady_clock::now();
    printf("task3 done! within %f ms\n", std::chrono::duration<double, std::milli>(stop - start).count());
}

void task4() {
    auto start = std::chrono::steady_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(0));
    auto stop = std::chrono::steady_clock::now();
    printf("task4 done! within %f ms\n", std::chrono::duration<double, std::milli>(stop - start).count());
}

void task() {

}

int main() {
    SimpleThreadPool pool;
    for (int i = 0; i < 100; ++i) {
        pool.submit(task1);
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));
}
