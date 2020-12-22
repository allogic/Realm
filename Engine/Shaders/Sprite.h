#pragma once

#include <Shaders/Utilities.h>

/*
* Sprite render shader.
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
vec2 RotateUV(in vec2 uv, in float rotation, in vec2 mid)
{
    return vec2(
      cos(rotation) * (uv.x - mid.x) + sin(rotation) * (uv.y - mid.y) + mid.x,
      cos(rotation) * (uv.y - mid.y) - sin(rotation) * (uv.x - mid.x) + mid.y
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
  vertOut.uv = RotateUV(iUv, radians(90.f), vec2(0.5f, 0.5f));
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