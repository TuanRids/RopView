#ifndef BLOCKQUEUEASCAN_H
#define BLOCKQUEUEASCAN_H

#include <deque>
#include <mutex>
#include <condition_variable>
#include <optional>

template <typename T>
class BlockingQueue {
private:
    std::deque<T> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    BlockingQueue() = default;

    // Add data to the queue (Producer)
    void push_back(const T& item) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push_back(item);
        }
        cv.notify_one(); // Notify consumer that data is available
    }

    // Remove data from the queue (Consumer)
    std::optional<T> pop_front() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return !queue.empty(); }); // Wait until queue is not empty
        T item = queue.front();
        queue.pop_front();
        return item;
    }

    // Clear all data in the queue
    void clear() {
        std::lock_guard<std::mutex> lock(mtx);
        queue.clear();
    }

    // Check if queue has data or wait until it does
    bool checkOrWait() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return !queue.empty(); }); // Wait until queue is not empty
        return !queue.empty(); // Should always return true after wait
    }

    // Get the size of the queue
    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }
};


#endif