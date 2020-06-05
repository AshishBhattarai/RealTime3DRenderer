#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

/**
 * A simple threadsafe wrapper for std::queue
 */
template <typename T> class SharedQueue {
private:
  std::queue<T> queue;
  std::mutex mutex;
  std::condition_variable condVar;
  std::atomic<bool> stop;

public:
  SharedQueue() : stop(false) {}
  ~SharedQueue() {
    stop = true;
    condVar.notify_all();
    mutex.lock();
  }

  bool front(T &front) {
    std::unique_lock<std::mutex> lock(mutex);
    while (queue.empty() && !stop)
      condVar.wait(lock);
    if (!stop)
      front = queue.front();
    return !stop;
  }

  bool popFront() {
    std::unique_lock<std::mutex> lock(mutex);
    while (queue.empty() && !stop)
      condVar.wait(lock);
    if (!stop)
      queue.pop();
    return !stop;
  }

  bool popGetFront(T &front, const bool &discard) {
    std::unique_lock<std::mutex> lock(mutex);
    while (queue.empty() && !stop && !discard)
      condVar.wait(lock);
    if (!stop && !discard) {
      front = queue.front();
      queue.pop();
    }
    return !stop && !discard;
  }

  void pushBack(const T &item) {
    std::unique_lock<std::mutex> lock(mutex);
    queue.push(item);
    lock.unlock();
    condVar.notify_one();
  }

  void pushBack(const T &&item) {
    std::unique_lock<std::mutex> lock(mutex);
    queue.push(std::move(item));
    lock.unlock();
    condVar.notify_one();
  }

  template <typename... Args> void emplaceBack(Args &&... args) {
    std::unique_lock<std::mutex> lock(mutex);
    queue.emplace(T(std::forward<Args>(args)...));
    lock.unlock();
    condVar.notify_one();
  }

  size_t size() {
    std::unique_lock<std::mutex> lock(mutex);
    size_t size = queue.size();
    lock.unlock();
    return size;
  }

  bool isEmpty() const { return size() == 0; }
};
