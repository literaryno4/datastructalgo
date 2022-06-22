#include <random>
#include <vector>

namespace structalgo {

template <typename T>
T genRandom() {
    return static_cast<T>(rand());
}

template <typename T>
std::vector<T> genRandom(int num) {
    std::vector<T> ans;
    for (int i = 0; i < num; ++i) {
        ans.push_back(genRandom<T>());
    }
    return ans;
}

}
