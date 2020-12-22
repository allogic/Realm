#pragma once

#include <Api.h>

/*
* Compute density.
*/

#define SHADER_COMPUTE_DENSITY                    \
SHADER_VERSION                                    \
R"glsl(
layout (local_size_x = 32, local_size_y = 32) in;
)glsl"                                            \
SHADER_NOISE_FUNCTIONS                            \
R"glsl(
layout (binding = 0) buffer GlDensity
{
  float density[];
};
layout (location = 0) uniform int uLayer;
void main()
{
  uint globalIndex = gl_GlobalInvocationID.z * (gl_WorkGroupSize.x * gl_NumWorkGroups.x) * (gl_WorkGroupSize.y * gl_NumWorkGroups.y) +
                     gl_GlobalInvocationID.y * (gl_WorkGroupSize.x * gl_NumWorkGroups.x) + 
                     gl_GlobalInvocationID.x;

  vec3 samplePosition = vec3(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, uLayer);

  density[globalIndex] = fbm(samplePosition, 0.2f);
}
)glsl"