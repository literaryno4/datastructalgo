//
// in this version of threadpool, every thread has its own queue, that means it need not to be thread safe.
//

#include <thread>
#include <future>
#include <vector>
#include <list>

namespace structalgo {

template <typename Func, typename T>
class ThreadPool3 {
    using QueueType = std::list<std::packaged_task<Func>>;
    const int maxThread_ = 25;
    bool done_;
    int queueIdx_;
    std::mutex mtx_;
    std::vector<std::thread> threads_;
    std::vector<QueueType> queues_;

    QueueType& getNextQueue() {
        int idx = queueIdx_;
        ++queueIdx_;
        if (queueIdx_ == queues_.size()) {
            queueIdx_ = 0;
        }
        return queues_[idx];
    }

public:
    using resultType = T;

    ThreadPool3 () : queueIdx_(0), done_(false){
        int numCore = std::thread::hardware_concurrency();
        int numThread = std::min(numCore == 0 ? 2 : numCore, maxThread_);
        for (int i = 0; i < numThread; ++i) {
            queues_.push_back(std::list<std::packaged_task<Func>>());
        }
        for (int i = 0; i < numThread; ++i) {
            threads_.push_back(std::thread(&ThreadPool3::threadFunc, this, i));
        }
    }

    ~ThreadPool3() {
        for (auto& q : queues_) {
            while (q.size());
        }
        done_ = true;
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void threadFunc(int queueIdx) {
        auto& q = queues_[queueIdx];
        while (!done_) {
            if (q.size()) {
                auto& task = q.front();
                task();
                std::unique_lock<std::mutex> lk(mtx_);
                q.pop_front();
                lk.unlock();
            } else {
                std::this_thread::yield();
            }
        }
    }

    template <typename F>
    std::future<resultType> submitTask(F f) {
        std::packaged_task<Func> task(std::move(f));
        std::future<resultType> res(task.get_future());
        auto& q = getNextQueue();
        std::unique_lock<std::mutex> lk(mtx_);
        q.push_back(std::move(task));
        lk.unlock();
        return res;
    }
};

}
