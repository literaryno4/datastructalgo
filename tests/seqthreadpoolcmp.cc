#include <vector>

int main() {
    int numOfV = 13400000;
    std::vector<int> v(numOfV);
    for (int i = 0; i < numOfV; ++i) {
        v[i] = i;
    }

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < numOfV; ++i) {
        ++v[i];
    }
    auto stop = std::chrono::steady_clock::now();
    printf("thread_pool2: %fms\n", std::chrono::duration<double, std::milli>(stop - start).count());
}
