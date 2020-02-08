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
