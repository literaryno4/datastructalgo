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
        
        // before make head be newNode, we must make sure head is now point to next of newNode(compare and exchange). 
        // If not, make head point to next of newNode and compare and exchange again.
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
            if (head.compare_exchange_strong(oldHead, ptr->next, std::memory_order_relaxed)) {
                std::shared_ptr<T> res;
                res.swap(ptr->data);

                // here minus 2 because externalCount inited to be 1 in push function, and add another 1 when loaded.
                int const countIncrease = oldHead.externalCount - 2;

                // add countIncrease to internalCount because CountedNodePtr is no longer existing, but node may not deleted
                // unless no other thread is reading this node. Anyway, we must add all external reference count to internal.
                if (ptr->internalCount.fetch_add(countIncrease, std::memory_order_release) == -countIncrease ) {
                    delete ptr;
                }
                return res;
            // if another thread popped, check this node whether is only 1 reference count. If it is, delete this node.
            } else if (ptr->internalCount.fetch_add(-1, std::memory_order_relaxed) == 1) {
                ptr->internalCount.load(std::memory_order_acquire);
                delete ptr;
            }
        }
    }
};
