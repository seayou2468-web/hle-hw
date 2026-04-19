// Copyright Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <vector>

#include "profile.h"

namespace Pica::Shader {
struct FSConfig;
} // namespace Pica::Shader

namespace Pica::Shader::Generator::SPIRV {

/**
 * Generates SPIR-V bytecode for the provided fragment shader config.
 *
 * On iOS-first builds in this repository, SPIR-V backend generation is intentionally disabled and
 * this returns an empty bytecode blob. Callers should use the GLSL/Metal paths.
 */
std::vector<u32> GenerateFragmentShader(const FSConfig& config, const Profile& profile);

} // namespace Pica::Shader::Generator::SPIRV
