#pragma once

#include "types.h"
#include <cstdlib>
#include <cstring>
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
  explicit Buffer(size_t size, uint alignment = 0) : size(size), alignment(alignment) {
    this->buf = new uchar[size];
  }

  explicit Buffer(const uchar *data, size_t size, uint alignment = 0) : Buffer(size, alignment) {
    if (data && (data + (size - 1)))
      memcpy((void *)this->buf, (void *)data, size);
    else {
      delete[] buf;
      this->size = 0;
    }
  }

  Buffer() = default;

  Buffer(const Buffer &buffer) : Buffer(buffer.data(), buffer.size, buffer.alignment) {}

  Buffer(Buffer &&buffer) {
    this->buf = buffer.buf;
    this->size = buffer.size;
    this->alignment = buffer.alignment;
    buffer.buf = nullptr;
    buffer.size = 0;
    buffer.alignment = 0;
  };
  Buffer &operator=(Buffer &&buffer) {
    this->buf = buffer.buf;
    this->size = buffer.size;
    this->alignment = buffer.alignment;
    buffer.buf = nullptr;
    buffer.size = 0;
    buffer.alignment = 0;
    return *this;
  }

  Buffer &operator=(Buffer &) = delete;

  ~Buffer() {
    if (buf) delete[] buf;
    size = 0;
  }

  bool isValid() const { return buf != nullptr; }
  const uchar *const data() const { return buf; }
  uchar * data() { return buf; } // TODO: better way to modify buffer content [[unsafe]]
  size_t getSize() const { return size; }
  uint getAlignment() const { return alignment; }
  void clear() { memset(buf, 0, size); }
  static size_t align(size_t offset, uint alignment) {
    return (offset + (alignment - 1)) & -alignment;
  }
};
