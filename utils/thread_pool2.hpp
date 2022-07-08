#include <vector>
#include <thread>
#include <functional>
#include <future>
#include "../lockfree_queue.hpp"

struct Task {
    void operator()(std::vector<int>& v, int begin, int end) {
        printf("changing from %d to %d\n", begin, end);
        for (int i = begin; i <= end; ++i) {
            v[i]++;
        }
    }
};

class ThreadPool2 {
    bool finished;
    LockFreeQueue<std::packaged_task<void()>> tasks;
    std::vector<std::thread> threads_;
public:
    ThreadPool2() : finished(false) {
        int numCore = std::thread::hardware_concurrency();
        int numThread = std::min(numCore == 0 ? 2 : numCore, 8);
        for (int i = 0; i < numThread; ++i) {
            threads_.push_back(std::thread(&ThreadPool2::threadFuc, this));
        }
    }
    ~ThreadPool2() {
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
    std::future<void> submitTask(Func f) {
        printf("pusing task...\n");
        std::packaged_task<void()> task(std::move(f));
        std::future<void> res(task.get_future());
        tasks.push(std::move(task));
        return res;
    }

    void threadFuc() {
        while (!finished) {
            auto taskPtr = tasks.pop();
            if (taskPtr) {
                auto& task = *taskPtr;
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }
};


