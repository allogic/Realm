#pragma once

#include <Shaders/Utilities.h>

/*
* Sprite render shader.
*/

#define SHADER_RENDER_POST_PROCESS_VERTEX \
SHADER_VERSION                            \
R"glsl(
layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec2 iUv;

layout (location = 0) out VertOut
{
  vec2 uv;
} vertOut;

void main()
{
  vertOut.uv = iUv;

  gl_Position = vec4(iPosition, 1.f);
}
)glsl"

#define SHADER_RENDER_POST_PROCESS_FRAGMENT \
SHADER_VERSION                              \
R"glsl(
layout (binding = 0) uniform sampler2D uAlbedo;

layout (location = 0) in VertOut
{
  vec2 uv;
} fragIn;

layout (location = 0) out vec4 oColor;

void main()
{
  oColor = texture(uAlbedo, fragIn.uv);
}
)glsl"