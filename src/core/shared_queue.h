#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

/**
 * A simple threadsafe wrapper for std::queue
 */
template <typename T> class SharedQueue {
public:
  SharedQueue() {}

  T &front() {
    std::unique_lock<std::mutex> lock(mutex);
    while (queue.empty())
      condVar.wait(lock);
    return queue.front();
  }
  void popFront() {
    std::unique_lock<std::mutex> lock(mutex);
    while (queue.empty())
      condVar.wait(lock);
    return queue.pop_front();
  }

  void pushBack(const T &item) {
    std::unique_lock<std::mutex> lock(mutex);
    queue.push_back(item);
    lock.unlock();
    condVar.notify_one();
  }

  void pushBack(const T &&item) {
    std::unique_lock<std::mutex> lock(mutex);
    queue.push_back(std::move(item));
    lock.unlock();
    condVar.notify_one();
  }

  template <typename... Args> void emplaceBack(Args &&... args) {
    std::unique_lock<std::mutex> lock(mutex);
    queue.emplace_back(T(std::forward<Args>(args)...));
    lock.unlock();
    condVar.notify_one();
  }

  size_t size() const {
    std::unique_lock<std::mutex> lock(mutex);
    size_t size = queue.size();
    lock.unlock();
    return size;
  }

  bool isEmpty() const { return size() == 0; }

private:
  std::deque<T> queue;
  std::mutex mutex;
  std::condition_variable condVar;
};
