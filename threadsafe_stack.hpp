#include <exception>
#include <stack>
#include <mutex>

struct empty_stack: std::exception {
};

template<typename T>
class ThreadsafeStack {
    std::stack<T> data;
    mutable std::mutex m;
public:
    ThreadsafeStack() {}
    ThreadsafeStack(const ThreadsafeStack& other) {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;
    }

    ThreadsafeStack& operator=(const ThreadsafeStack&) = delete;
    void push(T newValue) {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(newValue));
    }

    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) {
            return std::shared_ptr<T>();
            // throw empty_stack();
        }
        std::shared_ptr<T> const res{std::make_shared<T>(std::move(data.top()))};
        data.pop();
        return res;
    }

    void pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) {
            throw empty_stack();
        }
        value = std::move(data.top());
        data.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};
