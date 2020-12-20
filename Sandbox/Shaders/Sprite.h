#pragma once

#include <Api.h>

/*
* Sprite shaders.
*/

#define SHADER_RENDER_SPRITE_VERTEX \
SHADER_VERSION                      \
R"glsl(
struct Sprite
{
  float position[4];
  float rotation[4];
  float scale[4];
  uint  textureIndex;
};
layout (binding = 0) uniform GlProjection
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
};
layout (binding = 0) buffer GlSprite
{
  Sprite sprites[];
};
layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec2 iUv;
layout (location = 2) in vec4 iColor;
layout (location = 0) out VertOut
{
  vec3 position;
  vec2 uv;
  vec4 color;
  uint textureIndex;
} vertOut;
vec3 ToVec3(in float a[4]) { return vec3(a[0], a[1], a[2]); }
mat4 Rotate3D(in vec3 axis, in float angle)
{
  axis = normalize(axis);
  float s = sin(angle);
  float c = cos(angle);
  float oc = 1.0f - c;
  return mat4(
    oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.f,
    oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.f,
    oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.f,
    0.f,                                0.f,                                0.f,                                1.f
  );
}
void main()
{
  vec3 spritePosition = ToVec3(sprites[gl_InstanceID].position);
  vec3 spriteRotation = ToVec3(sprites[gl_InstanceID].rotation);
  vec3 spriteScale = ToVec3(sprites[gl_InstanceID].scale);
  uint spriteTextureIndex = sprites[gl_InstanceID].textureIndex;
  mat4 tvp = uProjection * uView * uTransform;
  vertOut.position = vec4(uTransform * vec4(spritePosition + iPosition * spriteScale, 1.f)).xyz;
  vertOut.uv = iUv;
  vertOut.color = iColor;
  vertOut.textureIndex = spriteTextureIndex;
  gl_Position = tvp * vec4(spritePosition + iPosition * spriteScale, 1.f);
}
)glsl"

#define SHADER_RENDER_SPRITE_FRAGMENT \
SHADER_VERSION                        \
R"glsl(
layout (binding = 0) uniform sampler2DArray uEnvironmentAtlas;
layout (location = 0) in VertOut
{
  vec3 position;
  vec2 uv;
  vec4 color;
  flat uint textureIndex;
} fragIn;
layout (location = 0) out vec4 oColor;
void main()
{
  oColor = texture(uEnvironmentAtlas, vec3(fragIn.uv, fragIn.textureIndex));
}
)glsl"

#define SHADER_COMPUTE_SPRITE_WORLD \
SHADER_VERSION                      \
R"glsl(
layout(local_size_x = 32) in;
)glsl"                              \
SHADER_NOISE_FUNCTIONS              \
R"glsl(
struct Sprite
{
  float position[4];
  float rotation[4];
  float scale[4];
  uint  textureIndex;
};
layout (binding = 0) uniform GlMap
{
  uint width;
  uint height;
};
layout (binding = 0) buffer GlSprite
{
  Sprite sprites[];
};
vec3 ToVec3(in float a[4]) { return vec3(a[0], a[1], a[2]); }
uint GetTextureIndex(in vec3 position, float d)
{
  uint textureIndex = 11;

  float  l = fbm(position + vec3(-1,  0, 0));
  float  r = fbm(position + vec3( 1,  0, 0));
  float  b = fbm(position + vec3( 0, -1, 0));
  float  t = fbm(position + vec3( 0,  1, 0));
  float bl = fbm(position + vec3(-1, -1, 0));
  float tl = fbm(position + vec3(-1,  1, 0));
  float br = fbm(position + vec3( 1, -1, 0));
  float tr = fbm(position + vec3( 1,  1, 0));

  //if (l > d && r > d && b > d && t > d && bl > d && tl > d && tr > d) textureIndex = 0; // top right
  //if (r > d && tl > d && t > d) textureIndex = 2; // top left

  if (l > d && b > d && t > d && bl > d && tl > d) textureIndex = 10; // left
  if (r > d && b > d && t > d && br > d && tr > d) textureIndex = 12; // right
  if (t > d && l > d && r > d && tl > d && tr > d) textureIndex = 1;  // top
  if (b > d && l > d && r > d && bl > d && br > d) textureIndex = 21; // bottom

  if (l > d && r > d && b > d && t > d && bl > d && tl > d && br > d && tr > d) textureIndex = 3;

  return textureIndex;
}
void main()
{
  vec3 spritePosition = ToVec3(sprites[gl_GlobalInvocationID.x].position);
  sprites[gl_GlobalInvocationID.x].textureIndex = GetTextureIndex(spritePosition, 0.5f);
}
)glsl"