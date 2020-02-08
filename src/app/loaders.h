#pragma once

class Image;
namespace tinygltf {
struct Model;
}

namespace app::Loaders {
bool loadModel(tinygltf::Model &model, const char *fileName);
bool loadImage(Image &image, const char *fileName);
}; // namespace app::Loaders
