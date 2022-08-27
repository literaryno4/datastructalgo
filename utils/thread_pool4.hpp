//
// in this version, we use thread safe queue for every thread.
//
#include <thread>
#include <future>
#include <vector>
#include "../lockfree_queue.hpp"

namespace structalgo {

template <typename Func, typename ReturnType>
class ThreadPool4{
    using TaskType = std::packaged_task<Func>;
    using QueueType = LockFreeQueue<TaskType>;

    bool done_ = false;
    int queueIdx_;
    const int maxThread = 25;
    const int numCores = std::thread::hardware_concurrency();
    const int numThread = std::min(numCores == 0 ? 2 : numCores, maxThread);
    std::vector<std::thread> threads_;
    std::vector<QueueType> queues_;

    QueueType& getNextQueue() {
        int idx = queueIdx_;
        ++queueIdx_;
        if (queueIdx_ == numThread) {
            queueIdx_ = 0;
        }
        return queues_[idx];
    }

public:
    ThreadPool4() : queues_(numThread) {
        for (int i = 0; i < numThread; ++i) {
            threads_.emplace_back(&ThreadPool4::threadFunc, this, i);
        }
    }
    ~ThreadPool4() {
        // for (auto& q : queues_) {
        //     while(!q.empty());
        // }
        done_ = true;
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void threadFunc(int queueIdx) {
        while (!done_) {
            auto& q = queues_[queueIdx];
            auto taskPtr = q.pop();
            if (taskPtr) {
                (*taskPtr)();
            } else {
                std::this_thread::yield();
            }
        }
    } 

    template <typename F>
    std::future<ReturnType> submitTask(F f) {
        std::packaged_task<Func> task(std::move(f));
        auto res = task.get_future();
        auto& q = getNextQueue();
        q.push(std::move(task));

        return res;
    }
};


}
