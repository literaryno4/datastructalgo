#include <memory>
#include <mutex>
#include <condition_variable>

namespace structalgo {

template <typename T>
class ThreadsafeQueue {
    struct Node {
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;
        Node() : next(nullptr) {}
    };
    std::unique_ptr<Node> head_;
    std::mutex headMutex_;
    Node* tail_;
    std::mutex tailMutex_;
    std::condition_variable dataCondVar_;

    // helper functions
    Node* getTail() {
        std::lock_guard<std::mutex> lk(tailMutex_);
        return tail_;
    }

    std::unique_ptr<Node> popHead() {
        std::unique_ptr<Node> oldHead = std::move(head_);
        head_ = std::move(oldHead->next);
        return oldHead;
    }

    std::unique_lock<std::mutex> waitForData() {
        std::unique_lock<std::mutex> lk(headMutex_);
        dataCondVar_.wait(lk, [&]() { return head_.get() != getTail(); });
        return std::move(lk);
    }

    std::unique_ptr<Node> waitPopHead() {
        std::unique_lock<std::mutex> lk(waitForData());
        return popHead();
    }

    std::unique_ptr<Node> waitPopHead(T& value) {
        std::unique_lock<std::mutex> lk(waitForData());
        value = std::move(*head_->data);
        return popHead();
    }

public:
    ThreadsafeQueue() : head_(new Node), tail_(head_.get()) {}
    bool empty() {
        std::lock_guard<std::mutex> lk(headMutex_);
        return head_.get() == getTail();
    }

    void push(T data) {
        auto newData = std::make_shared<T>(std::move(data));
        auto newNode = std::make_unique<Node>();
        Node* newTail = newNode.get();
        {
            std::lock_guard<std::mutex> lk(tailMutex_);
            tail_->data = std::move(newData);
            tail_->next = std::move(newNode);
            tail_ = newTail;
        }
        dataCondVar_.notify_one();
    }

    std::shared_ptr<Node> tryPop() {
        std::lock_guard<std::mutex> lk(headMutex_);
        if (head_.get() == getTail()) {
            return std::shared_ptr<Node>();
        }
        return popHead();
    }

    std::shared_ptr<Node> tryPop(T& value) {
        std::lock_guard<std::mutex> lk(headMutex_);
        if (head_.get() == getTail()) {
            return std::shared_ptr<Node>();
        }
        std::unique_ptr<Node> oldHead = std::move(popHead());
        value = *(oldHead->data);
        return oldHead;
    }

    std::shared_ptr<T> waitAndPop() {
        std::unique_ptr<Node> oldHead = waitPopHead();
        return oldHead->data;
    }

    void waitAndPop(T& value) {
        std::unique_ptr<Node> oldHead = waitPopHead(value);
    }
};

}
