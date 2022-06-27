//
// Last Recently Used Cache
//

#include <unordered_map>

// Double linked List node
template <typename K, typename V>
struct Node {
    Node(K k, V v) : key(k), val(v), prev(nullptr), next(nullptr) {}
    K key;
    V val;
    Node* prev, *next;
};

// Double linked List
template <typename K, typename V>
class DList {
   public:
    using NodeType = Node<K, V>;

   private:
    NodeType *head, *tail;

   public:
    DList() : head(nullptr), tail(nullptr) {}

    NodeType* getHead() { return head; }

    NodeType* getTail() { return tail; }

    // move node of this list to head
    void moveToHead(NodeType* node) {
        if (node == head) {
            return;
        } else {
            if (node == tail) {
                tail = node->prev;
                tail->next = nullptr;
            } else {
                node->prev->next = node->next;
                node->next->prev = node->prev;
            }
            node->next = head;
            head->prev = node;
            node->prev = nullptr;
            head = node;
        }
    }

    void addToHead(NodeType* node) {
        if (head == nullptr) {
            head = node;
            tail = node;
        } else {
            node->next = head;
            head->prev = node;
            head = node;
        }
    }

    void removeTail() {
        NodeType* tmptail = tail;
        if (head == tail) {
            head = nullptr;
            tail = nullptr;
        } else {
            tail = tail->prev;
            tail->next = nullptr;
        }
        delete tmptail;
    }
};

template <typename K, typename V>
class LRU {
public:
    using NodeType = Node<K, V>;
   private:
    std::unordered_map<K, NodeType*> um_;
    DList<K, V> list_;
    int capacity_;

   public:
    explicit LRU(int capacity) : capacity_(capacity) {}

    V get(K key) {
        if (!um_.count(key)) {
            return V(-1);
        }
        NodeType* node = um_[key];
        list_.moveToHead(node);
        return node->val;
    }

    void put(K key, V val) {
        if (um_.count(key)) {
            NodeType* node = um_[key];
            node->val = val;
            list_.moveToHead(node);
        } else {
            if (um_.size() == capacity_) {
                // removeTail
                NodeType* tail = list_.getTail();
                um_.erase(tail->key);
                list_.removeTail();
            }
            NodeType* node = new NodeType(key, val);
            list_.addToHead(node);
            um_[key] = node;
        }
    }
};
