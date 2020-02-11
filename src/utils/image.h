#pragma once

#include "types.h"
#include <memory>

class Image {
private:
  uchar *buffer;
  int width;
  int height;
  int numChannels;

public:
  Image(uchar *buffer, int width, int height, int numChannels)
      : buffer(buffer), width(width), height(height), numChannels(numChannels) {
  }
  Image() = default;

  Image(Image &&image) {
    this->buffer = image.buffer;
    this->width = image.width;
    this->height = image.height;
    this->numChannels = image.numChannels;
    image.buffer = nullptr;
    image.width = image.height = image.numChannels = 0;
  }
  Image &operator=(Image &&image) {
    this->buffer = image.buffer;
    this->width = image.width;
    this->height = image.height;
    this->numChannels = image.numChannels;
    image.buffer = nullptr;
    image.width = image.height = image.numChannels = 0;
    return *this;
  }

  Image(Image &) = delete;
  Image &operator=(Image &) = delete;

  ~Image() {
    if (buffer) {
      free(buffer);
      buffer = 0;
      width = 0;
      height = 0;
    }
  }

  const uchar *getBuffer() const { return buffer; }
  int getWidth() const { return width; }
  int getHeight() const { return height; }
  int getNumChannels() const { return numChannels; }
  bool isValid() const { return buffer; }
};
