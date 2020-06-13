#pragma once

#include "buffer.h"
#include "types.h"
#include <memory>

class Image {
private:
  Buffer buffer;
  int width;
  int height;
  int numChannels;
  bool isHDR;

public:
  explicit Image(uchar *buffer, int width, int height, int numChannels,
                 bool isHDR = false)
      : buffer(buffer, width * height * numChannels), width(width),
        height(height), numChannels(numChannels), isHDR(isHDR) {}

  explicit Image(uchar *buffer, size_t size, uint align, int width, int height,
                 int numChannels, bool isHDR = false)
      : buffer(buffer, size, align), width(width), height(height),
        numChannels(numChannels), isHDR(isHDR) {}

  explicit Image(Buffer &&buffer, int width, int height, int numChannels,
                 bool isHDR = false)
      : buffer(std::move(buffer)), width(width), height(height),
        numChannels(numChannels), isHDR(isHDR) {}

  Image() = default;

  Image(Image &&image) {
    this->buffer = std::move(image.buffer);
    this->width = image.width;
    this->height = image.height;
    this->numChannels = image.numChannels;
    this->isHDR = image.isHDR;
    image.width = image.height = image.numChannels = 0;
  }

  Image &operator=(Image &&image) {
    this->buffer = std::move(image.buffer);
    this->width = image.width;
    this->height = image.height;
    this->numChannels = image.numChannels;
    this->isHDR = image.isHDR;
    image.width = image.height = image.numChannels = 0;
    return *this;
  }

  Image(Image &) = delete;
  Image &operator=(Image &) = delete;

  ~Image() {
    if (buffer.data()) {
      width = 0;
      height = 0;
    }
  }

  const Buffer *getBuffer() const { return &buffer; }
  int getWidth() const { return width; }
  int getHeight() const { return height; }
  int getNumChannels() const { return numChannels; }
  bool getIsHDR() const { return isHDR; }
  bool isValid() const { return buffer.isValid(); }
};
