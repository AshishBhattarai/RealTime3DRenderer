#pragma once

#include <cstdlib>

class Image;
class Buffer;
namespace tinygltf {
struct Model;
}

namespace app::Loaders {
bool loadModel(tinygltf::Model &model, const char *fileName);
bool loadImage(Image &image, const char *fileName);
bool loadBinaryFile(Buffer &buffer, const char *fileName);
}; // namespace app::Loaders
