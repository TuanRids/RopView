#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>

// Circular buffer structure
template<typename T>
class CircularBuffer {
public:
    CircularBuffer(size_t size) : maxSize(size), buffer(size) {}

    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        condVar_.wait(lock, [this]() { return bufferSize_ < maxSize; });
        buffer[head_] = item;
        head_ = (head_ + 1) % maxSize;
        ++bufferSize_;
        condVar_.notify_all();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        condVar_.wait(lock, [this]() { return bufferSize_ > 0; });
        T item = buffer[tail_];
        tail_ = (tail_ + 1) % maxSize;
        --bufferSize_;
        condVar_.notify_all();
        return item;
    }
    size_t size() const {   
		return bufferSize_;
	}
private:
    std::vector<T> buffer;
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t bufferSize_ = 0;
    const size_t maxSize;
    std::mutex mutex_;
    std::condition_variable condVar_;
};

