#pragma once

#include <Api.h>

/*
* Compute density smoothing.
*/

#define SHADER_COMPUTE_SMOOTHING                  \
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
void main()
{
  uint globalIndex = gl_GlobalInvocationID.z * (gl_WorkGroupSize.x * gl_NumWorkGroups.x) * (gl_WorkGroupSize.y * gl_NumWorkGroups.y) +
                     gl_GlobalInvocationID.y * (gl_WorkGroupSize.x * gl_NumWorkGroups.x) + 
                     gl_GlobalInvocationID.x;

  uint neighbourCount = 0;

  uint px = uint(gl_GlobalInvocationID.x);
  uint py = uint(gl_GlobalInvocationID.y);

  uint globalSizeX = gl_WorkGroupSize.x * gl_NumWorkGroups.x;

  float ground = 0.5f;

  uint itl = (px - 1) + (py + 1) * globalSizeX;
  uint it  = (px    ) + (py + 1) * globalSizeX;
  uint itr = (px + 1) + (py + 1) * globalSizeX;
  uint il  = (px - 1) + (py    ) * globalSizeX;

  uint ir  = (px + 1) + (py    ) * globalSizeX;
  uint ibl = (px - 1) + (py - 1) * globalSizeX;
  uint ib  = (px    ) + (py - 1) * globalSizeX;
  uint ibr = (px + 1) + (py - 1) * globalSizeX;

  if (density[itl] > ground) neighbourCount++;
  if (density[it ] > ground) neighbourCount++;
  if (density[itr] > ground) neighbourCount++;
  if (density[il ] > ground) neighbourCount++;

  if (density[ir ] > ground) neighbourCount++;
  if (density[ibl] > ground) neighbourCount++;
  if (density[ib ] > ground) neighbourCount++;
  if (density[ibr] > ground) neighbourCount++;

  if (neighbourCount < 4)
    density[globalIndex] = 0.f;
  else if (neighbourCount > 4)
    density[globalIndex] = 1.f;
}
)glsl"