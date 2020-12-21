#pragma once

#include <Api.h>

/*
* Compute density.
*/

#define SHADER_COMPUTE_DENSITY                   \
SHADER_VERSION                                   \
R"glsl(
layout(local_size_x = 32, local_size_y = 32) in;
)glsl"                                           \
SHADER_NOISE_FUNCTIONS                           \
R"glsl(
layout (binding = 0) buffer GlDensity
{
  float density[];
};
void main()
{
  vec3 samplePosition = vec3(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, 0);
  density[gl_LocalInvocationIndex] = fbm(samplePosition);
}
)glsl"