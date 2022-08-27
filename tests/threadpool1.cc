#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include "../lockfree_queue.hpp"

struct Task {
    void operator()(std::vector<int>& v, int begin, int end) {
        for (int i = begin; i <= end; ++i) {
            v[i]++;
        }
    }
};

class ThreadPool {
    bool finished;
    LockFreeQueue<std::function<void()>> tasks;
    std::vector<std::thread> threads_;
public:
    ThreadPool() : finished(false) {
        int numCore = std::thread::hardware_concurrency();
        int numThread = std::min(numCore == 0 ? 2 : numCore, 8);
        for (int i = 0; i < numThread; ++i) {
            threads_.push_back(std::thread(&ThreadPool::threadFuc, this));
        }
    }
    ~ThreadPool() {
        finished = true;
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    std::vector<std::thread>& getThreads() {
        return threads_;
    }
    int sizeOfThreads() {
        return threads_.size();
    }

    template <typename Func>
    void submitTask(Func f) {
        // printf("pusing task...\n");
        tasks.push(std::function<void()>(f));
    }

    void threadFuc() {
        while (!finished) {
            auto taskPtr = tasks.pop();
            if (taskPtr) {
                std::function<void()> task = *taskPtr;
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }
};

class ThreadPoolLock {
    bool finished;
    std::queue<std::function<void()>> tasks;
    std::vector<std::thread> threads_;
    std::mutex mtx;
public:
    ThreadPoolLock() : finished(false) {
        int numCore = std::thread::hardware_concurrency();
        int numThread = std::min(numCore == 0 ? 2 : numCore, 8);
        for (int i = 0; i < numThread; ++i) {
            threads_.push_back(std::thread(&ThreadPoolLock::threadFuc, this));
        }
    }
    ~ThreadPoolLock() {
        finished = true;
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    void wait() {
        while (!tasks.empty()) {
            std::this_thread::yield();
        }
    }
    std::vector<std::thread>& getThreads() {
        return threads_;
    }
    int sizeOfThreads() {
        return threads_.size();
    }

    template <typename Func>
    void submitTask(Func f) {
        // printf("pusing task...\n");
        std::lock_guard<std::mutex> lk(mtx);
        tasks.push(std::function<void()>(f));
    }

    void threadFuc() {
        while (!finished) {
            std::unique_lock<std::mutex> lk(mtx);
            if (!tasks.empty()) {
                auto task = tasks.front();
                tasks.pop();
                lk.unlock();
                task();
            } else {
                lk.unlock();
                std::this_thread::yield();
            }
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
    int minPerThread = 25;
    int maxThreads = (length + minPerThread - 1) / minPerThread;
    int numCore = std::thread::hardware_concurrency();
    int numThread = std::min(numCore == 0 ? 2 : numCore, maxThreads);

    ThreadPoolLock tp;
    int numPerThread = 25;
    int i;
    auto start = std::chrono::steady_clock::now();
    for (i = 0; i <= length - numPerThread; i += numPerThread) {
        tp.submitTask([=, &v]() {
                          Task()(v, i, i + numPerThread - 1);
                      });
    }
    Task()(v, i, length - 1);
    tp.wait();

    auto stop = std::chrono::steady_clock::now();
    printf("thread_pool2: %fms\n", std::chrono::duration<double, std::milli>(stop - start).count());

    // for (int i = 0; i < numOfV; ++i) {
    //     printf("%d ", v[i]);
    // }
    // printf("\n");

    std::vector<int> v2(numOfV);
    for (int i = 0; i < numOfV; ++i) {
        v2[i] = i + 1;
    }
    assert(v == v2);
}
