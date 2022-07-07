//
// thread-safe, exception-safe and truely concurrent forward list.
//

#include <memory>
#include <mutex>

namespace structalgo {

template <typename T>
class ThreadsafeList {
    //
    // every node has a unique mutex, which make threads visit list
    // concurrently. note this method must keep threads visiting list
    // sequentially. For example: list: A->B->C->D->E. if thread1 is visiting
    // node C, new threads can only visit nodes before node C
    //
    struct Node {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;
        Node() : next() {}
        Node(T&& value) : data(std::make_shared<T>(std::move(value))), next() {}
    };

    // this forward list has a dummy head
    Node head;

   public:
    ThreadsafeList() {}
    ~ThreadsafeList() {
        removeIf([](auto&){ return true; });
    }

    ThreadsafeList(const ThreadsafeList&) = delete;
    ThreadsafeList& operator=(const ThreadsafeList&) = delete;

    // add new node after the dummy head
    void pushFront(T&& data) {
        auto newHead = std::make_unique<Node>(std::move(data));
        std::lock_guard<std::mutex> lk(head.m);
        newHead->next = std::move(head.next);
        head.next = std::move(newHead);
    }

    void insertAfter(Node* pos, T&& data) {
        auto newNode = std::make_unique<Node>(std::move(data));
        std::unique_lock<std::mutex> lk(head.m);
        Node* current = &head;
        while ((current = current->next.get())) {
            std::unique_lock<std::mutex> lk_next(current->m);
            lk.unlock();
            if (current == pos) {
                newNode->next = std::move(current->next);
                current->next = std::move(newNode);
            }
            lk = std::move(lk_next);
        }
    }

    //
    // Iterate this list and do something to every node.
    // to safely iterate, we must make other thread no way to pass by current node
    // that is to say:
    // list: A->B->C->D->E
    // first lock Node B, then make sure lock C before unlock B 
    //
    template <typename Func>
    void forEach(Func func) {
        Node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while ((current = current->next.get())) {
            std::unique_lock<std::mutex> lk_next(current->m);
            lk.unlock();
            func(*current->data);
            lk = std::move(lk_next);
        }
    }

    //
    // find first of node which satisfy pred
    // to safely iterate, we must make other thread no way to pass by current node
    // that is to say:
    // list: A->B->C->D->E
    // first lock Node B, then make sure lock C before unlock B 
    //
    template <typename Pred>
    Node* findFirstIF(Pred pred) {
        Node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while ((current = current->next.get())) {
            std::unique_lock<std::mutex> lk_next(current->m);
            lk.unlock();
            if (pred(*current->data)) {
                return current;
            }
            lk = std::move(lk_next);
        }
        return nullptr;
    }

    // remove first of node which satisfy pred
    template <typename Pred>
    void removeIf(Pred pred) {
        Node* current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (current->next) {
            std::unique_lock<std::mutex> lk_next(current->next->m);
            if (pred(*current->next->data)) {
                auto oldNext = std::move(current->next);
                current->next = std::move(oldNext->next);
                lk_next.unlock();
            } else {
                current = current->next.get();
                lk = std::move(lk_next);
            }
        }
    }
    
    void remove(const T& value) {
        removeIf([&](auto& data) {return data == value; });
    }
};

}  // namespace structalgo
