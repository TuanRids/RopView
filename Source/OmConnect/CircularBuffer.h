#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>

// Circular buffer structure with FIFO when buffer is full.
// Thread-safe buffer used for saving data from multiple threads.
// If the buffer is full, the oldest data is overwritten (FIFO).
// TODO: Update to directly save data into GPU instead of CPU memory.
template<typename T>
class CircularBuffer {

public:
    /// <summary>
    /// Constructor that initializes the buffer with a fixed size.
    /// </summary>
    /// <param name="size">Maximum number of elements the buffer can hold.</param>
    CircularBuffer(size_t size) : maxSize(size), buffer(size) {}

    /// <summary>
    /// Pushes a new item into the buffer.
    /// If the buffer is full, the oldest element is discarded (FIFO behavior).
    /// </summary>
    /// <param name="item">The item to be added to the buffer.</param>
    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (bufferSize_ == maxSize) {
            // If buffer is full, move the tail forward to discard the oldest element
            tail_ = (tail_ + 1) % maxSize;
            --bufferSize_; // Reduce size to make room for new element
        }
        buffer[head_] = item;
        head_ = (head_ + 1) % maxSize;
        ++bufferSize_;
        condVar_.notify_all(); // Notify waiting threads that new data is available
    }

    /// <summary>
    /// Pops an item from the buffer.
    /// Waits if the buffer is empty until an item becomes available.
    /// </summary>
    /// <returns>The oldest item from the buffer.</returns>
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        condVar_.wait(lock, [this]() { return bufferSize_ > 0; }); // Wait until there is data
        T item = buffer[tail_];
        tail_ = (tail_ + 1) % maxSize;
        --bufferSize_;
        condVar_.notify_all(); // Notify waiting threads that space is available
        return item;
    }

    /// <summary>
    /// Returns the current number of elements in the buffer.
    /// </summary>
    /// <returns>The number of elements in the buffer.</returns>
    size_t size() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return bufferSize_;
    }

private:
    std::vector<T> buffer;          // The internal buffer storage
    size_t head_ = 0;               // Index for the next element to be added
    size_t tail_ = 0;               // Index for the next element to be removed
    size_t bufferSize_ = 0;         // Current number of elements in the buffer
    const size_t maxSize;           // Maximum capacity of the buffer
    mutable std::mutex mutex_;      // Mutex for thread safety
    std::condition_variable condVar_; // Condition variable for waiting on data availability
};
