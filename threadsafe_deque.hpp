//
// threadsafe deuque, only use mutex, no efficiency guaranty
//
#include <deque>
#include <mutex>

namespace structalgo {

template <typename DataType>
class ThreadsafeDeque {
    std::deque<DataType> queue_;
    mutable std::mutex mtx_;

public:
    ThreadsafeDeque() {}
    ThreadsafeDeque(const ThreadsafeDeque&) = delete;
    ThreadsafeDeque& operator=(const ThreadsafeDeque&) = delete;

    void push(DataType data) {
        std::lock_guard<std::mutex> lk(mtx_);
        queue_.push_front(std::move(data));
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return queue_.empty();
    }

    bool tryPop(DataType& res) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (queue_.empty()) {
            return false;
        }
        res = std::move(queue_.front());
        queue_.pop_front();
        return true;
    }

    bool trySteal(DataType& res) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (queue_.empty()) {
            return false;
        }
        res = std::move(queue_.back());
        queue_.pop_back();
        return true;
    }
};

}
