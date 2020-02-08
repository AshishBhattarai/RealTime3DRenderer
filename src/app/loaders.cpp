#include "loaders.h"
#include "types.h"
#include "utils/image.h"
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
    SLOG("Loaded glTF:", fileName);
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
  if (!buffer)
    return false;
  image = Image(buffer, width, height, numChannels);
  return true;
}
} // namespace app::Loaders
