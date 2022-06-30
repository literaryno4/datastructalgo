#include <vector>
#include <atomic>
#include <thread>
#include <future>
#include "../threadsafe_queue.hpp"
#include "../threadsafe_deque.hpp"
#include "join_threads.hpp"

namespace structalgo {

template <typename TaskType>
class ThreadPool {
    std::atomic_bool done_;
    ThreadsafeQueue<TaskType> poolWorkQueue_;
    std::vector<std::unique_ptr<ThreadsafeDeque<TaskType>>> queues_;
    std::vector<std::thread> threads_;
    JoinThreads joiner_;
    static thread_local ThreadsafeDeque<TaskType>* localWorkQueue_;
    static thread_local unsigned myIndex_;
    void workerThread(unsigned myIndex) {
        myIndex_ = myIndex;
        localWorkQueue_ = queues_[myIndex_].get();
        while (!done_) {
            runPendingTask();
        }
    }

    bool popTaskFromLocalQueue(TaskType& task) {
        return localWorkQueue_ && localWorkQueue_->tryPop(task);
    }

    bool popTaskFromPoolQueue(TaskType& task) {
        return poolWorkQueue_.tryPop(task) != nullptr;
    }

    bool popTaskFromOtherThreadQueue(TaskType& task) {
        for (unsigned i = 0; i < queues_.size(); ++i) {
            const unsigned index = (myIndex_+ i + 1) % queues_.size();
            if (queues_[index]->trySteal(task)) {
                return true;
            }
        }
        return false;
    }

public:
    ThreadPool() : done_(false), joiner_(threads_) {
        const unsigned threadCount = std::thread::hardware_concurrency();
        try {
            for (unsigned i = 0; i < threadCount; ++i) {
                queues_.push_back(std::make_unique<ThreadsafeDeque<TaskType>>());
            }
            for (unsigned i = 0; i < threadCount; ++i) {
                threads_.emplace_back(&ThreadPool::workerThread, this, i);
            }
        } catch (...) {
            done_ = true;
            throw;
        }
    }

    ~ThreadPool() {
        done_ = true;
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f) {
        using resultType = typename std::result_of<FunctionType()>::type;
        std::packaged_task<resultType()> task(f);
        std::future<resultType> res(task.get_future());
        if (localWorkQueue_) {
            localWorkQueue_->push(std::move(task));
        } else {
            poolWorkQueue_.push(std::move(task));
        }
        return res;
    }

    void runPendingTask() {
        TaskType task;

        if (popTaskFromLocalQueue(task) ||
            popTaskFromPoolQueue(task) ||
            popTaskFromOtherThreadQueue(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }
};

}
