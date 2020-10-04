#include "general_vs_ubo.h"
#include "config.h"

namespace render_system::shader {
GeneralVSUBO::GeneralVSUBO() : UniformBuffer(TOTAL_SIZE, vertex::generalUBOBinding) {}
}
