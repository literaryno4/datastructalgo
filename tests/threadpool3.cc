#include <functional>
#include "../utils/thread_pool3.hpp"

using namespace structalgo;

struct Task {
    void operator()(std::vector<int>& v, int begin, int end) {
        printf("changing from %d to %d\n", begin, end);
        for (int i = begin; i <= end; ++i) {
            v[i]++;
        }
    }
};

int main() {
    int numOfV = 134;
    std::vector<int> v(numOfV);
    for (int i = 0; i < numOfV; ++i) {
        v[i] = i;
    }

    int length = v.size();

    ThreadPool3<void(), void> tp;
    int numPerThread = 25;
    std::vector<std::future<void>> futs(length / numPerThread);
    int i, idx;
    for (i = 0, idx = 0; i <= length - numPerThread; i += numPerThread, ++idx) {
        futs[idx] = tp.submitTask([=, &v]() {
                          Task()(v, i, i + numPerThread - 1);
                      });
        printf("%d\n", i);
    }
    Task()(v, i, length - 1);

    for (auto& fut : futs) {
        fut.get();
    }
    for (int i = 0; i < numOfV; ++i) {
        printf("%d ", v[i]);
    }
    printf("\n");

    std::vector<int> v2(numOfV);
    for (int i = 0; i < numOfV; ++i) {
        v2[i] = i + 1;
    }
    assert(v == v2);
}
