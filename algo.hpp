#include <iostream>
#include <list>
#include <queue>
#include <vector>
#include <future>

namespace structalgo {

//
// sort algo for array
//
// quicksort for array
template <typename C>
int partition(C& c, int l, int r) {
    int i = l - 1, j = l, pivot = c[r];
    for (; j < r; ++j) {
        if (c[j] < pivot) {
            std::swap(c[i + 1], c[j]);
            ++i;
        }
    }
    std::swap(c[r], c[i + 1]);
    return i + 1;
}
template <typename C>
void quickSort(C& c, int l, int r) {
    if (l >= r) {
        return;
    }
    int mid = partition(c, l, r);
    quickSort(c, l, mid - 1);
    quickSort(c, mid + 1, r);
}

// quicksort for list
template <typename T>
std::list<T> sequentialQuickSort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }
    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    const T& pivot = *result.begin();
    auto dividePoint = std::partition(input.begin(), input.end(), [&](const T& t) { return t < pivot; });
    std::list<T> lowerPart;
    lowerPart.splice(lowerPart.end(), input, input.begin(), dividePoint);
    auto newLower{sequentialQuickSort(std::move(lowerPart))};
    auto newHigher{sequentialQuickSort(std::move(input))};
    result.splice(result.end(), newHigher);
    result.splice(result.begin(), newLower);
    return result;
}

// parallel quickSort for list, code from C++ concurrency in action
template <typename T>
std::list<T> parallelQuickSort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }
    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    const T& pivot = *result.begin();
    auto dividePoint = std::partition(input.begin(), input.end(), [&](const T& t) { return t < pivot; });
    std::list<T> lowerPart;
    lowerPart.splice(lowerPart.end(), input, input.begin(), dividePoint);

    // here I use async to lauch a new thread to sort lowerPart, store the result in std::future;
    auto newLower{std::async(std::launch::async, parallelQuickSort<T>, std::move(lowerPart))};
    auto newHigher{parallelQuickSort(std::move(input))};
    result.splice(result.end(), newHigher);
    result.splice(result.begin(), newLower.get());
    return result;
}

// mergesort
template <typename C>
void merge(C& c, int l, int mid, int r) {
    using value_type = typename C::value_type;
    std::vector<value_type> tmp;
    int i = l, j = mid + 1;
    while (i <= mid && j <= r) {
        if (c[i] < c[j]) {
            tmp.push_back(c[i++]);
        } else {
            tmp.push_back(c[j++]);
        }
    }
    for (; i <= mid; ++i) {
        tmp.push_back(c[i]);
    }
    for (; j <= r; ++j) {
        tmp.push_back(c[j]);
    }
    std::copy(tmp.begin(), tmp.end(), c.begin() + l);
}

template <typename C>
void mergeSort(C& c, int l, int r) {
    if (l >= r) {
        return;
    }
    int mid = l + ((r - l) >> 1);
    mergeSort(c, l, mid);
    mergeSort(c, mid + 1, r);
    merge(c, l, mid, r);
}

// heap sort
template <typename C>
void heapSort(C& c, int l, int r) {
    using value_type = typename C::value_type;
    std::priority_queue<value_type> pq;
    for (int i = l; i <= r; ++i) {
        pq.push(c[i]);
    }
    while (!pq.empty() && l <= r) {
        c[r--] = pq.top();
        pq.pop();
    }
}

// bubbleSort
template <typename C>
void bubbleSort(C& c, int l, int r) {
    for (int t = r; t >= l; --t) {
        for (int i = l + 1; i <= t; ++i) {
            if (c[i - 1] > c[i]) {
                std::swap(c[i - 1], c[i]);
            }
        }
    }
}

// selectionSort
template <typename C>
void selectionSort(C& c, int l, int r) {
    for (int be = l; be <= r; ++be) {
        int minIdx = be;
        for (int i = be + 1; i <= r; ++i) {
            if (c[minIdx] > c[i]) {
                minIdx = i;
            }
        }
        std::swap(c[be], c[minIdx]);
    }
}

// insertionSort
template <typename C>
void insertionSort(C& c, int l, int r) {
    using value_type = typename C::value_type;
    for (int i = l + 1; i <= r; ++i) {
        int j = i;
        value_type tmp = c[i];
        for (; j > l && tmp < c[j - 1]; --j) {
            c[j] = c[j - 1];
        }
        c[j] = tmp;
    }
}

// countingSort
template <typename C>
void countingSort(C& c, int l, int r) {
    using value_type = typename C::value_type;
    value_type maxEle = 0;
    for (int i = l; i <= r; ++i) {
        maxEle = std::max(maxEle, c[i]);
    }
    std::vector<value_type> v(maxEle + 1, 0);
    for (int i = l; i <= r; ++i) {
        v[c[i]]++;
    }

    for (int i = 0; i < v.size(); ++i) {
        while (v[i] != 0) {
            c[l++] = i;
            v[i]--;
        }
    }
}

// bucketSort
const int BUCKET_NUM = 10;

template <typename T>
struct ListNode {
    ListNode(T v) : val(v), next(nullptr) {}
    ListNode<T>* next;
    T val;
};

template <typename T>
ListNode<T>* insert(ListNode<T>* head, T newVal) {
    if (head == nullptr) {
        return new ListNode<T>(newVal);
    }
    if (newVal < head->val) {
        ListNode<T>* node  = new ListNode<T>(newVal);
        node->next = head;
        return node;
    }
    ListNode<T>* tmpHead = head;
    while (head->next != nullptr && newVal > head->next->val) {
        head = head->next;
    }
    ListNode<T>* newNode = new ListNode<T>(newVal);
    ListNode<T>* tmp = head->next;
    head->next = newNode;
    newNode->next = tmp;
    return tmpHead;
}

template <typename T>
ListNode<T>* merge(ListNode<T>* head1, ListNode<T>* head2) {
    ListNode<T>* dummyNode = new ListNode<T>(0);
    ListNode<T>* head = dummyNode;
    while (head1 && head2) {
        if (head1->val < head2->val) {
            dummyNode->next = head1;
            head1 = head1->next;
        } else {
            dummyNode->next = head2;
            head2 = head2->next;
        }
        dummyNode = dummyNode->next;
    }
    while (head1) {
        dummyNode->next = head1;
        head1 = head1->next;
        dummyNode = dummyNode->next;
    }
    while (head2) {
        dummyNode->next = head2;
        head2 = head2->next;
        dummyNode = dummyNode->next;
    }
    return head->next;
}

template <typename C>
void bucketSort(C& c, int l, int r) {
    using value_type = typename C::value_type;
    std::vector<ListNode<value_type>*> buckets(BUCKET_NUM, nullptr);
    for (int i = l; i <= r; ++i) {
        int idx = c[i] / BUCKET_NUM;
        ListNode<value_type>* head = buckets[idx];
        buckets[idx] = insert(head, c[i]);
    }
    ListNode<value_type>* head = buckets[0];
    for (int i = 1; i < BUCKET_NUM; ++i) {
        head = merge(head, buckets[i]);
    }
    for (int i = l; i <= r && head != nullptr; ++i) {
        c[i] = head->val;
        head = head->next;
    }
}

// radixSort
template <typename C>
void radixSort(C& c, int l, int r) {
    using value_type = typename C::value_type;
    int maxEle = 0;
    for (int i = l; i <= r; ++i) {
        maxEle = std::max(maxEle, c[i]);
    }
    int d = 1;
    while (maxEle / 10) {
        d++;
        maxEle /= 10;
    }

    int radix = 1;
    int k;
    for (int t = 0; t < d; ++t) {
        std::vector<int> count(10, 0);
        for (int i = l; i <= r; ++i) {
            k = (c[i] / radix) % 10;
            count[k]++;
        }
        for (int i = 1; i < 10; ++i) {
            count[i] = count[i] + count[i - 1];
        }
        std::vector<value_type> tmp(r - l + 1);
        // notice !! you must loop from r to l.
        for (int j = r; j >= l; --j) {
            k = (c[j] / radix) % 10;
            tmp[count[k] - 1] = c[j];
            count[k]--;
        }
        for (int i = 0, j = l; i < tmp.size(); ++i, ++j) {
            c[j] = tmp[i];
        }
        radix *= 10;
    }
}

// shellSort
template <typename C>
void shellSort(C& c, int l, int r) {
    int length = r - l + 1;
    int gap = length / 2;
    while (gap >= 1) {
        for (int i = l + gap; i <= r; ++i) {
            for (int j = i; j >= gap && c[j] < c[j - gap]; j -= gap) {
                std::swap(c[j], c[j - gap]);
            }
        }
        gap /= 2;
    }
}

}  // namespace structalgo
