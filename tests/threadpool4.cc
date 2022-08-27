#include "../utils/thread_pool4.hpp"
#include <chrono>
#include <cassert>

using namespace structalgo;

struct Task {
    void operator()(std::vector<int>& v, int b, int e) {
        for (int i = b; i <= e; ++i) {
            ++v[i];
        }
    }
};


int main() {
    const int  numOfV = 13400000;
    const int numPerThread = 10000;

    std::vector<int> v(numOfV);
    for (int i = 0; i < numOfV; ++i) {
        v[i] = i;
    }

    int length = v.size();
    ThreadPool4<void(), void> tp;
    std::vector<std::future<void>> futs(length / numPerThread);
    int i, idx;
    auto start = std::chrono::steady_clock::now();
    for (i = 0, idx = 0; i <= length - numPerThread; i += numPerThread, ++idx) {
        futs[idx] = tp.submitTask([=, &v]() {
                          Task()(v, i, i + numPerThread -1);
                      });
    }
    Task()(v, i, v.size() - 1);
    for (auto& fut : futs) {
        fut.get();
    }
    auto stop = std::chrono::steady_clock::now();
    printf("thread_pool4: %fms\n", std::chrono::duration<double, std::milli>(stop - start).count());

    // for (auto& n : v) {
    //     printf("%d ", n);
    // }
    // printf("\n");

    std::vector<int> v2(numOfV);
    for (int i = 0; i < numOfV; ++i) {
        v2[i] = i + 1;
    }

    assert(v == v2);
    return 0;
}
