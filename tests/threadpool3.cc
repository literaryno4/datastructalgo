#include <functional>
#include "../utils/thread_pool3.hpp"

using namespace structalgo;

struct Task {
    void operator()(std::vector<int>& v, int begin, int end) {
        for (int i = begin; i <= end; ++i) {
            v[i]++;
        }
    }
};

int main() {
    int numOfV = 13400000;
    std::vector<int> v(numOfV);
    for (int i = 0; i < numOfV; ++i) {
        v[i] = i;
    }

    int length = v.size();

    ThreadPool3<void(), void> tp;
    int numPerThread = 25;
    std::vector<std::future<void>> futs(length / numPerThread);
    int i, idx;
    auto start = std::chrono::steady_clock::now();
    for (i = 0, idx = 0; i <= length - numPerThread; i += numPerThread, ++idx) {
        futs[idx] = tp.submitTask([=, &v]() {
                          Task()(v, i, i + numPerThread - 1);
                      });
    }
    Task()(v, i, length - 1);

    for (auto& fut : futs) {
        fut.get();
    }
    auto stop = std::chrono::steady_clock::now();
    printf("thread_pool3: %fms\n", std::chrono::duration<double, std::milli>(stop - start).count());
    //for (int i = 0; i < numOfV; ++i) {
    //    printf("%d ", v[i]);
    //}
    //printf("\n");

    std::vector<int> v2(numOfV);
    for (int i = 0; i < numOfV; ++i) {
        v2[i] = i + 1;
    }
    assert(v == v2);
}
