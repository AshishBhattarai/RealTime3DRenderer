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
  uchar *buf;
  size_t size;
  uint alignment;

public:
  Buffer(size_t size, uint alignment = 0) : size(size), alignment(alignment) {
    this->buf = (uchar *)malloc(size);
  }
  Buffer(uchar *buf, size_t size, uint alignment = 0)
      : buf(buf), size(size), alignment(alignment) {}
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
      free(buf);
    size = 0;
  }

  bool isValid() const { return buf; }
  uchar *data() { return buf; }
  const uchar *data() const { return buf; }
  size_t getSize() const { return size; }
  uint getAlignment() const { return alignment; }
  static size_t align(size_t offset, uint alignment) {
    return (offset + (alignment - 1)) & -alignment;
  }
};
