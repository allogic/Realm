#pragma once

#include <Shaders/Utilities.h>

/*
* Gizmo shader.
*/

#define SHADER_RENDER_GIZMO_VERTEX \
SHADER_VERSION                     \
R"glsl(
layout(binding = 0) uniform GlProjection
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
};
layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec4 iColor;
layout(location = 0) out VertOut
{
  vec4 color;
} vertOut;
void main()
{
  vertOut.color = iColor;
  gl_Position = uProjection * uView * uTransform * vec4(iPosition, 1.f);
}
)glsl"

#define SHADER_RENDER_GIZMO_FRAGMENT \
SHADER_VERSION                       \
R"glsl(
layout(location = 0) in VertOut
{
  vec4 color;
} fragIn;
layout(location = 0) out vec4 oColor;
void main()
{
  oColor = fragIn.color;
}
)glsl"