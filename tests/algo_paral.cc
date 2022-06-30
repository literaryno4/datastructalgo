#include <vector>
#include <iostream>
#include <chrono>
#include <cassert>
#include "../algo_parallel.hpp"
#include "../utils/genRandom.h"

using namespace structalgo;

int main() {
    vector<int> v1 = genRandom<int>(1000000);
    vector<int> v2 = v1;

    auto start1 = std::chrono::steady_clock::now();
    std::for_each(v1.begin(), v1.end(), [](auto& ele) {
                          ele = ele / 2;
                      });

    auto stop1 = std::chrono::steady_clock::now();
    auto start2 = std::chrono::steady_clock::now();

    parallel_for_each(v2.begin(), v2.end(), [](auto& ele) {
                          ele = ele / 2;
                      });

    auto stop2 = std::chrono::steady_clock::now();

    std::cout << std::chrono::duration<double, std::milli>(stop1 - start1).count() << "ms\n";
    std::cout << std::chrono::duration<double, std::milli>(stop2 - start2).count() << "ms\n";

    assert(v1 == v2);
}
