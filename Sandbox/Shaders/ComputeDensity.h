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
float GetDensity(vec2 position, float frequency, float ground)
{
  float density = 0.f;
  uint neighbourCount = 0;

  uint px = uint(position.x);
  uint py = uint(position.y);

  float itl = fbm(vec2(px - 1, py + 1), frequency);
  float it  = fbm(vec2(px    , py + 1), frequency);
  float itr = fbm(vec2(px + 1, py + 1), frequency);
  float il  = fbm(vec2(px - 1, py    ), frequency);

  float ir  = fbm(vec2(px + 1, py    ), frequency);
  float ibl = fbm(vec2(px - 1, py - 1), frequency);
  float ib  = fbm(vec2(px    , py - 1), frequency);
  float ibr = fbm(vec2(px + 1, py - 1), frequency);

  if (itl > ground) neighbourCount++;
  if (it  > ground) neighbourCount++;
  if (itr > ground) neighbourCount++;
  if (il  > ground) neighbourCount++;

  if (ir  > ground) neighbourCount++;
  if (ibl > ground) neighbourCount++;
  if (ib  > ground) neighbourCount++;
  if (ibr > ground) neighbourCount++;

  if (neighbourCount <= 4)
    density = 0.f;
  else
    density = 1.f;

  return density;
}
void main()
{
  uint globalIndex = gl_GlobalInvocationID.z * (gl_WorkGroupSize.x * gl_NumWorkGroups.x) * (gl_WorkGroupSize.y * gl_NumWorkGroups.y) +
                     gl_GlobalInvocationID.y * (gl_WorkGroupSize.x * gl_NumWorkGroups.x) + 
                     gl_GlobalInvocationID.x;
  vec2 samplePosition = vec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);

  density[globalIndex] = GetDensity(samplePosition, 0.06f, 0.5f);
}
)glsl"