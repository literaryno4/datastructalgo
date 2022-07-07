#include <thread> 
#include <vector>

class JoinThread {
    std::thread& thread_;
public:
    explicit JoinThread(std::thread& thread) : thread_(thread) {}
    ~JoinThread() {
        if (thread_.joinable()) {
            thread_.join();
        }
    }
};

class JoinThreads {
    std::vector<std::thread>& threads_;
public:
    explicit JoinThreads(std::vector<std::thread>& threads) : threads_(threads) {}
    ~JoinThreads() {
        for (auto& thread: threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
};
