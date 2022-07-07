#include "../threadsafe_queue.hpp"
#include "join_threads.hpp"
#include <thread>
#include <vector>
#include <functional>
#include <atomic>

namespace structalgo {

class SimpleThreadPool {
    std::atomic<bool> done_;
    ThreadsafeQueue<std::function<void()>> workQueue_;
    std::vector<std::thread> threads_;
    JoinThreads joiner_;

    void doWork() {
        while (!done_) {
            std::function<void()> task;
            if (workQueue_.tryPop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

public:
    
    SimpleThreadPool () : done_(false), joiner_(threads_) {
        const unsigned int numThreads_ = std::thread::hardware_concurrency();
        try {
            for (unsigned int i = 0; i < numThreads_; ++i) {
                threads_.emplace_back(&SimpleThreadPool::doWork, this);
            }
        } catch (...) {
            done_ = true;
            throw;
        }
    }
    
    ~SimpleThreadPool() {
        done_ = true;
    }

    void submit(std::function<void()> f) {
        workQueue_.push(f);
    }
};

}
