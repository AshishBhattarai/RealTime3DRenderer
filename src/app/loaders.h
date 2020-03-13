#pragma once

#include <cstdlib>
#include <memory>

class Image;
class Buffer;
namespace tinygltf {
struct Model;
}

namespace app::Loaders {
bool loadModel(tinygltf::Model &model, const char *fileName);
bool loadImage(Image &image, const char *fileName);
bool loadBinaryFile(Buffer &buffer, const char *fileName);
bool writeImage(std::shared_ptr<Image> image, const char *fileName);
}; // namespace app::Loaders
