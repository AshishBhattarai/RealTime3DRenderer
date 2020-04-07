#include "loaders.h"
#include "core/buffer.h"
#include "core/image.h"
#include "types.h"
#include "utils/slogger.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <third_party/tinygltf/tiny_gltf.h>

namespace app::Loaders {
bool loadModel(tinygltf::Model &model, const char *fileName) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;
  bool res = loader.LoadASCIIFromFile(&model, &err, &warn, fileName);
  if (!warn.empty()) {
    SLOG("WARN:", warn);
  }
  if (!err.empty()) {
    SLOG("ERR:", err);
  }
  if (!res)
    SLOG("Failed to load glTF:", fileName);
  else {
    DEBUG_SLOG("Loaded glTF:", fileName);
  }
  return res;
}

bool loadImage(Image &image, const char *fileName) {
  int width = 0;
  int height = 0;
  int numChannels = 0;

  uchar *buffer = stbi_load(std::string(fileName).c_str(), &width, &height,
                            &numChannels, 0);
  assert(buffer && "failed to load the checker texture.");
  if (!buffer) {
    SLOG("Failed to load image:", fileName);
    return false;
  } else {
    DEBUG_SLOG("Loaded image: ", fileName);
    image = Image(buffer, width, height, numChannels);
    return true;
  }
}

bool loadBinaryFile(Buffer &buffer, const char *fileName) {
  // read from file
  std::ifstream file(fileName, std::ios::binary | std::ios::ate);
  if (file.is_open()) {
    size_t size = file.tellg();
    buffer = Buffer(size);
    file.seekg(0, std::ios::beg);
    file.read((char *)buffer.data(), size);
    file.close();
  }
  // check for errors
  if (file.fail() && buffer.isValid()) {
    SLOG("Failed to read form binary file:", fileName);
    return false;
  } else {
    DEBUG_SLOG("Loaded binary file:", fileName);
    return true;
  }
}

bool writeImage(std::shared_ptr<Image> image, const char *fileName) {
  int nrChannels = image->getNumChannels();
  int width = image->getWidth();
  int height = image->getHeight();
  int stride =
      Buffer::align(nrChannels * width, image->getBuffer()->getAlignment());
  return stbi_write_png(fileName, width, height, nrChannels,
                        image->getBuffer()->data(), stride);
}

} // namespace app::Loaders
