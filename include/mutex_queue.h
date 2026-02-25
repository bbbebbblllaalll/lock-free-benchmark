#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template <typename T>
class MutexQueue {
public:
    explicit MutexQueue(size_t capacity) : capacity_(capacity) {}

    // Blocking push — waits if full
    void push(const T& item) {
        std::unique_lock<std::mutex> lock(mtx_);
        not_full_.wait(lock, [this] { return buffer_.size() < capacity_; });
        buffer_.push(item);
        not_empty_.notify_one();
    }

    // Blocking pop — waits if empty
    T pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        not_empty_.wait(lock, [this] { return !buffer_.empty(); });
        T item = buffer_.front();
        buffer_.pop();
        not_full_.notify_one();
        return item;
    }

    // Non-blocking push — returns false if full
    bool try_push(const T& item) {
        std::unique_lock<std::mutex> lock(mtx_);
        if (buffer_.size() >= capacity_) return false;
        buffer_.push(item);
        not_empty_.notify_one();
        return true;
    }

    // Non-blocking pop — returns nullopt if empty
    std::optional<T> try_pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        if (buffer_.empty()) return std::nullopt;
        T item = buffer_.front();
        buffer_.pop();
        not_full_.notify_one();
        return item;
    }

private:
    std::queue<T> buffer_;
    size_t capacity_;
    std::mutex mtx_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
};
