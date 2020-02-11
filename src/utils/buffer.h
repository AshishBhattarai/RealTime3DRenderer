#pragma once

#include "types.h"
#include <cstdlib>
/**
 * @brief The Buffer class
 *
 * Represents data buffer. Can be use to store content of binary files.
 */
class Buffer {
private:
  char *buf;
  size_t size;

public:
  Buffer(char *buf, size_t size) : buf(buf), size(size) {}
  Buffer() = default;

  Buffer(Buffer &&buffer) {
    this->buf = buffer.buf;
    this->size = buffer.size;
    buffer.buf = nullptr;
    buffer.size = 0;
  };
  Buffer &operator=(Buffer &&buffer) {
    this->buf = buffer.buf;
    this->size = buffer.size;
    buffer.buf = nullptr;
    buffer.size = 0;
    return *this;
  }

  Buffer(Buffer &) = delete;
  Buffer &operator=(Buffer &) = delete;

  ~Buffer() {
    if (buf)
      delete buf;
    size = 0;
  }

  const char *getContent() const { return buf; }
  size_t getSize() const { return size; }
};
