#pragma once

#include <Api.h>

/*
* Compute texture ids.
*/

#define SHADER_COMPUTE_TEXTURE \
SHADER_VERSION                 \
R"glsl(
layout(local_size_x = 32) in;
struct Sprite
{
  float position[4];
  float rotation[4];
  float scale[4];
  uint  textureIndex;
};
layout (binding = 0) buffer GlSprite
{
  Sprite sprites[];
};
layout (binding = 1) buffer GlDensity
{
  float density[];
};
vec3 ToVec3(in float a[4]) { return vec3(a[0], a[1], a[2]); }
bool MatchPattern(in float[9] map, in float[9] pattern)
{
  bool match = true;

  if (map[0] == pattern[0]) match = false;
  if (map[1] == pattern[1]) match = false;
  if (map[2] == pattern[2]) match = false;
  if (map[3] == pattern[3]) match = false;
  if (map[4] == pattern[4]) match = false;
  if (map[5] == pattern[5]) match = false;
  if (map[6] == pattern[6]) match = false;
  if (map[7] == pattern[7]) match = false;
  if (map[8] == pattern[8]) match = false;

  return match;
}
uint GetTextureIndex(in vec3 position, float ground)
{
  uint textureIndex = 11;
  float map[9];
  float pattern[9];

  uint dimX = gl_NumWorkGroups.x * gl_WorkGroupSize.x;
  uint px = uint(position.x);
  uint py = uint(position.y);

  uint itl = (px - 1) + (py + 1) * dimX;
  uint it  = (px    ) + (py + 1) * dimX;
  uint itr = (px + 1) + (py + 1) * dimX;
  uint il  = (px - 1) + (py    ) * dimX;
  uint im  = (px    ) + (py    ) * dimX;
  uint ir  = (px + 1) + (py    ) * dimX;
  uint ibl = (px - 1) + (py - 1) * dimX;
  uint ib  = (px    ) + (py - 1) * dimX;
  uint ibr = (px + 1) + (py - 1) * dimX;

  map[0] = density[itl] > ground ? 1 : 0;
  map[1] = density[it ] > ground ? 1 : 0;
  map[2] = density[itr] > ground ? 1 : 0;
  map[3] = density[il ] > ground ? 1 : 0;
  map[4] = density[im ] > ground ? 1 : 0;
  map[5] = density[ir ] > ground ? 1 : 0;
  map[6] = density[ibl] > ground ? 1 : 0;
  map[7] = density[ib ] > ground ? 1 : 0;
  map[8] = density[ibr] > ground ? 1 : 0;

  // fill

  pattern = float[]
  (
    1, 1, 1,
    1, 1, 1,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 3;

  // right

  pattern = float[]
  (
    0, 1, 1,
    0, 1, 1,
    0, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 12;
  pattern = float[]
  (
    1, 1, 1,
    0, 1, 1,
    0, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 12;
  pattern = float[]
  (
    0, 1, 1,
    0, 1, 1,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 12;
  pattern = float[]
  (
    1, 1, 1,
    0, 1, 1,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 12;

  // left

  pattern = float[]
  (
    1, 1, 0,
    1, 1, 0,
    1, 1, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 10;
  pattern = float[]
  (
    1, 1, 1,
    1, 1, 0,
    1, 1, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 10;
  pattern = float[]
  (
    1, 1, 0,
    1, 1, 0,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 10;
  pattern = float[]
  (
    1, 1, 1,
    1, 1, 0,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 10;

  // top

  pattern = float[]
  (
    1, 1, 1,
    1, 1, 1,
    0, 0, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 1;
  pattern = float[]
  (
    1, 1, 1,
    1, 1, 1,
    1, 0, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 1;
  pattern = float[]
  (
    1, 1, 1,
    1, 1, 1,
    0, 0, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 1;
  pattern = float[]
  (
    1, 1, 1,
    1, 1, 1,
    1, 0, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 1;

  // bottom

  pattern = float[]
  (
    0, 0, 0,
    1, 1, 1,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 21;
  pattern = float[]
  (
    1, 0, 0,
    1, 1, 1,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 21;
  pattern = float[]
  (
    0, 0, 1,
    1, 1, 1,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 21;
  pattern = float[]
  (
    1, 0, 1,
    1, 1, 1,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 21;

  // bottom right

  pattern = float[]
  (
    0, 1, 1,
    1, 1, 1,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 22;

  // bottom left

  pattern = float[]
  (
    1, 1, 0,
    1, 1, 1,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 20;

  // top left

  pattern = float[]
  (
    1, 1, 1,
    1, 1, 1,
    1, 1, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 0;

  // top right

  pattern = float[]
  (
    1, 1, 1,
    1, 1, 1,
    0, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 2;

   // top left corner

  pattern = float[]
  (
    1, 1, 0,
    1, 1, 0,
    0, 0, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 24;
  pattern = float[]
  (
    1, 1, 1,
    1, 1, 0,
    0, 0, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 24;
  pattern = float[]
  (
    1, 1, 0,
    1, 1, 0,
    1, 0, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 24;

  // top right corner

  pattern = float[]
  (
    0, 1, 1,
    0, 1, 1,
    0, 0, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 23;
  pattern = float[]
  (
    1, 1, 1,
    0, 1, 1,
    0, 0, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 23;
  pattern = float[]
  (
    0, 1, 1,
    0, 1, 1,
    0, 0, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 23;

  // bottom left corner

  pattern = float[]
  (
    0, 0, 0,
    1, 1, 0,
    1, 1, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 14;
  pattern = float[]
  (
    1, 0, 0,
    1, 1, 0,
    1, 1, 0
  );
  if (MatchPattern(map, pattern)) textureIndex = 14;
  pattern = float[]
  (
    0, 0, 0,
    1, 1, 0,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 14;

  // bottom right corner

  pattern = float[]
  (
    0, 0, 0,
    0, 1, 1,
    0, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 13;
  pattern = float[]
  (
    0, 0, 0,
    0, 1, 1,
    1, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 13;
  pattern = float[]
  (
    0, 0, 1,
    0, 1, 1,
    0, 1, 1
  );
  if (MatchPattern(map, pattern)) textureIndex = 13;

  return textureIndex;
}
void main()
{
  vec3 spritePosition = ToVec3(sprites[gl_GlobalInvocationID.x].position);
  sprites[gl_GlobalInvocationID.x].textureIndex = GetTextureIndex(spritePosition, 0.5f);
}
)glsl"