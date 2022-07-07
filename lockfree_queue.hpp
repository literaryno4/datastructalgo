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
public:
    void push (T newValue) {
        auto newData = std::make_shared<T>(newValue);
        CountedNodePtr newNext;
        newNext.ptr = new Node;
        newNext.externalCount = 1;
        CountedNodePtr oldTail = tail.load();
        for (;;) {
            increaseExternalCount(tail, oldTail);
            T* oldData = nullptr;
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
            if (ptr == tail.load().ptr) {
                ptr->releaseRef();
                return std::unique_ptr<T>();
            }
            if (head.compare_exchange_strong(oldHead, ptr->next)) {
                T* const res = ptr->data.exchange(nullptr);
                freeExternalCounter(oldHead);
                return std::unique_ptr<T>(res);
            }
            ptr->releaseRef();
        }
    }
};

