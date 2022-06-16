#include <deque>
#include <iostream>
#include <vector>

#include "../algo.hpp"
using namespace structalgo;

template <typename C>
void printContainer(C& c) {
    for (auto& e : c) {
        std::cout << e << ' ';
    }
    std::cout << '\n';
}

template <typename C>
void testSortAlgo(void (*sortAlgo)(C& c, int l, int r), C& c, int l, int r) {
    sortAlgo(c, l, r);
}

int main() {
    std::vector<int> v1{9, 8, 1, 333, 21, 19, 88, 0};
    std::deque<int> dq1{9, 8, 1, 3, 2, 1, 8, 0};
    std::vector<double> v2{-9.9, 8.1, 1.32, 33.1, 2.1, 1.1, 8.9, 0.9, -0.93};
    std::deque<double> dq2{-9.9, 8.1, 1.32, 33.1, 2.1, 1.1, 8.9, 0.9, -0.93};

    // quickSort(v1, 0, v1.size() - 1);
    // quickSort(dq1, 0, dq1.size() - 1);
    // quickSort(v2, 0, v1.size() - 1);
    // quickSort(dq2, 0, dq1.size() - 1);
    // mergeSort(v1, 0, v1.size() - 1);
    // mergeSort(dq1, 0, v1.size() - 1);
    // mergeSort(v2, 0, v1.size() - 1);
    // mergeSort(dq2, 0, v1.size() - 1);
    // std::cout << "start heapSort\n";
    // heapSort(v1, 0, v1.size() - 1);
    // heapSort(dq1, 0, dq1.size() - 1);
    // heapSort(v2, 0, v1.size() - 1);
    // heapSort(dq2, 0, dq1.size() - 1);
    // std::cout << "start bubbleSort\n";
    // bubbleSort(v1, 0, v1.size() - 1);
    // bubbleSort(dq1, 0, dq1.size() - 1);
    // bubbleSort(v2, 0, v2.size() - 1);
    // bubbleSort(dq2, 0, dq2.size() - 1);
    // std::cout << "start SelectionSort\n";
    // selectionSort(v1, 0, v1.size() - 1);
    // selectionSort(dq1, 0, dq1.size() - 1);
    // selectionSort(v2, 0, v2.size() - 1);
    // selectionSort(dq2, 0, dq2.size() - 1);
    // std::cout << "start insertionSort\n";
    // insertionSort(v1, 0, v1.size() - 1);
    // insertionSort(dq1, 0, dq1.size() - 1);
    // insertionSort(v2, 0, v2.size() - 1);
    // insertionSort(dq2, 0, dq2.size() - 1);
    // std::cout << "start countingSort\n";
    // countingSort(v1, 0, v1.size() - 1);
    // countingSort(dq1, 0, dq1.size() - 1);
    // std::cout << "start bucketSort\n";
    // bucketSort(v1, 0, v1.size() - 1);
    // bucketSort(dq1, 0, dq1.size() - 1);
    // bucketSort(v2, 0, v2.size() - 1);
    // bucketSort(dq2, 0, dq2.size() - 1);
    // std::cout << "start radixSort\n";
    // radixSort(v1, 0, v1.size() - 1);
    // radixSort(dq1, 0, dq1.size() - 1);
    std::cout << "start shellSort\n";
    shellSort(v1, 0, v1.size() - 1);
    shellSort(dq1, 0, dq1.size() - 1);
    shellSort(v2, 0, v2.size() - 1);
    shellSort(dq2, 0, dq2.size() - 1);

    printContainer(v1);
    printContainer(dq1);
    printContainer(v2);
    printContainer(dq2);
    return 0;
}
