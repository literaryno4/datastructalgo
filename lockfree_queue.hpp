//
// an implementation of a lock free queue. It is similar to the implementation of the lock free stack. Both of them use
// external and internal reference count to manage memory.
//

#include <atomic>
#include <memory>

template<typename T>
class LockFreeQueue {
private:
    struct Node;
    struct CountedNodePtr {
        int externalCount;
        Node* ptr;
    };
    std::atomic<CountedNodePtr> head;
    std::atomic<CountedNodePtr> tail;

    struct NodeCounter {
        unsigned internalCount:30;
        unsigned externalCounters:2;
    };
    struct Node {

        // decrease reference count
        void releaseRef() {
            NodeCounter oldCounter = count.load(std::memory_order_relaxed);
            NodeCounter newCounter;
            do {
                newCounter = oldCounter;
                --newCounter.internalCount;
            } while (!count.compare_exchange_strong(oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));
            if (!newCounter.internalCount && !newCounter.externalCounters) {
                delete this;
            }
        }

        std::atomic<T*> data;
        std::atomic<NodeCounter> count;
        CountedNodePtr next;
        Node() {
            NodeCounter newCount;
            newCount.internalCount = 0;
            newCount.externalCounters = 2;
            count.store(newCount);
            next.ptr = nullptr;
            next.externalCount = 0;
        }
    };

    // increase reference count of counter
    static void increaseExternalCount(std::atomic<CountedNodePtr>& counter, CountedNodePtr& oldCounter) {
        CountedNodePtr newCounter;
        do {
            newCounter = oldCounter;
            ++newCounter.externalCount;
        } while (!counter.compare_exchange_strong(oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));
        oldCounter.externalCount = newCounter.externalCount;
    }

    // add external reference count to internal, delete external counter if it decreases to zero.
    static void freeExternalCounter(CountedNodePtr& oldNodePtr) {
        Node* const ptr = oldNodePtr.ptr;
        int const countIncrease = oldNodePtr.externalCount - 2;
        NodeCounter oldCounter = ptr->count.load(std::memory_order_relaxed);
        NodeCounter newCounter;
        do {
            newCounter = oldCounter;
            --newCounter.externalCounters;
            newCounter.internalCount += countIncrease;
        } while (!ptr->count.compare_exchange_strong(oldCounter, newCounter, std::memory_order_acquire, std::memory_order_relaxed));
        if (!newCounter.internalCount && !newCounter.externalCounters) {
            delete ptr;
        }
    }

public:
    LockFreeQueue () {
        CountedNodePtr newNext; 
        newNext.ptr = new Node;
        newNext.externalCount = 1;
        head.store(newNext);
        tail.store(head);
    }

    void push (T&& newValue) {
        auto newData = std::make_unique<T>(std::move(newValue));
        CountedNodePtr newNext;
        newNext.ptr = new Node;
        newNext.externalCount = 1;
        CountedNodePtr oldTail = tail.load();
        for (;;) {
            increaseExternalCount(tail, oldTail);
            T* oldData = nullptr;

            // push data to the dummy tail node, the make newNext be new dummy tail
            if (oldTail.ptr->data.compare_exchange_strong(oldData, newData.get())) {
                oldTail.ptr->next = newNext;
                oldTail = tail.exchange(newNext);
                freeExternalCounter(oldTail);
                newData.release();
                break;
            }
            oldTail.ptr->releaseRef();
        }
    }

    std::unique_ptr<T> pop() {
        CountedNodePtr oldHead = head.load(std::memory_order_relaxed);
        for (;;) {
            increaseExternalCount(head, oldHead);
            Node* const ptr = oldHead.ptr;

            // this means queue has only a dummy tail. It is empty.
            if (ptr == tail.load().ptr) {
                ptr->releaseRef();
                return std::unique_ptr<T>();
            }

            // exchange head to head->next
            if (head.compare_exchange_strong(oldHead, ptr->next)) {
                T* const res = ptr->data.exchange(nullptr);
                freeExternalCounter(oldHead);
                return std::unique_ptr<T>(res);
            }
            ptr->releaseRef();
        }
    }
};

