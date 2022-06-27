#include <atomic>
#include <thread>
#include <memory>

template <typename T>
class LockfreeStack {
   private:
    struct Node;
    struct CountedNodePtr {
        int externalCount;
        Node* ptr;
    };
    struct Node {
        std::shared_ptr<T> data;
        std::atomic<int> internalCount;
        CountedNodePtr next;
        Node(const T& d) : data(std::make_shared<T>(d)), internalCount(0) {}
        Node(T&& d) : data(std::make_shared<T>(std::move(d))), internalCount(0) {}
    };

    std::atomic<CountedNodePtr> head;

    void increaseHeadCount(CountedNodePtr& oldCounter) {
        CountedNodePtr newCounter;
        do {
            newCounter = oldCounter;
            ++newCounter.externalCount;
        } while (!head.compare_exchange_strong(oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));
        oldCounter.externalCount = newCounter.externalCount;
    }

public:
    ~LockfreeStack() {
        while (pop());
    }

    void push(T&& data) {
        CountedNodePtr newNode;
        newNode.ptr = new Node(std::forward<T&&>(data));
        newNode.externalCount = 1;
        newNode.ptr->next = head.load(std::memory_order_relaxed);
        while (!head.compare_exchange_weak(newNode.ptr->next, newNode, std::memory_order_release, std::memory_order_relaxed));
    }

    std::shared_ptr<T> pop() {
        CountedNodePtr oldHead = head.load(std::memory_order_relaxed);
        for (;;) {
            increaseHeadCount(oldHead);
            Node* const ptr = oldHead.ptr;
            if (!ptr) {
                return std::shared_ptr<T>();
            }
            if (head.compare_exchange_strong(oldHead, ptr->next)) {
                std::shared_ptr<T> res;
                res.swap(ptr->data);
                int const countIncrease = oldHead.externalCount - 2;
                if (ptr->internalCount.fetch_add(countIncrease, std::memory_order_release) == -countIncrease ) {
                    delete ptr;
                }
                return res;
            } else if (ptr->internalCount.fetch_add(-1, std::memory_order_relaxed) == 1) {
                ptr->internalCount.load(std::memory_order_acquire);
                delete ptr;
            }
        }
    }
};
