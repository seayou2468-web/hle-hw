// Copyright Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <vector>

#include "../../../common/logging/log.h"
#include "pica_fs_config.h"
#include "spv_fs_shader_gen.h"

namespace Pica::Shader::Generator::SPIRV {

std::vector<u32> GenerateFragmentShader(const FSConfig&, const Profile&) {
    LOG_WARNING(Render,
                "SPIR-V fragment shader generation is disabled in this iOS-focused build; "
                "falling back to non-SPIR-V backend is required.");
    return {};
}

} // namespace Pica::Shader::Generator::SPIRV
