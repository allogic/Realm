#pragma once

#include <Api.h>

/*
* Sprite render shader.
*/

s8 const* spSpriteVertexShader
{
R"glsl(
#version 460 core

struct Transform
{
  float position[3];
  float rotation[3];
  float scale[3];
};

layout (binding = 0) uniform GlProjection
{
  mat4 uProjection;
  mat4 uView;
  mat4 uTransform;
};

layout (binding = 0) buffer GlTransform
{
  Transform transforms[];
};

layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec2 iUv;
layout (location = 2) in vec4 iColor;

layout (location = 0) out VertOut
{
  vec3 position;
  vec2 uv;
  vec4 color;
} vertOut;

vec3 ToVec3(in float a[3]) { return vec3(a[0], a[1], a[2]); }

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
  uint objIndex = gl_InstanceID;

  vec3 transformPosition = ToVec3(transforms[objIndex].position);
  vec3 transformRotation = ToVec3(transforms[objIndex].rotation);
  vec3 transformScale = ToVec3(transforms[objIndex].scale);

  mat4 tvp = uProjection * uView * uTransform;

  vertOut.position = vec4(uTransform * vec4(transformPosition + iPosition * transformScale, 1.f)).xyz;
  vertOut.uv = iUv;
  vertOut.color = iColor;

  gl_Position = tvp * vec4(transformPosition + iPosition * transformScale, 1.f);
}
)glsl"
};
s8 const* spSpriteFragmentShader
{
R"glsl(
#version 460 core

layout (binding = 0) uniform sampler2DArray uEnvironmentAtlas;

layout (location = 0) in VertOut
{
  vec3 position;
  vec2 uv;
  vec4 color;
} fragIn;

layout (location = 0) out vec4 oColor;

void main()
{
  oColor = texture(uEnvironmentAtlas, vec3(fragIn.uv, 91));
}
)glsl"
};

/*
* Sprite compute shader.
*/

s8 const* spSpriteComputeShader
{
R"glsl(
#version 460 core

layout (local_size_x = 32) in;

void main()
{

}
)glsl"
};

/*
* World manager.
*/

struct WorldManager : Actor
{
  u32v2                        mNumSprites              { 10, 10 };
  u32v2                        mSpriteSize              { 1, 1 };

  TextureArray*                mpTextureEnvironmentAtlas{ ACS::Attach<TextureArray>(this, ROOT_PATH "Textures\\EnvironmentAtlas.png", 16u, 100u) };

  RenderShader*                mpRenderSprite           { ACS::Attach<RenderShader>(this, spSpriteVertexShader, spSpriteFragmentShader) };
  ComputeShader*               mpComputeSprite          { ACS::Attach<ComputeShader>(this, spSpriteComputeShader) };

  Mesh<VertexDefault>*         mpMeshSprite             { ACS::Attach<Mesh<VertexDefault>>(this, 4u, 6u) };
  ComputeBuffer<GlTransform>*  mpTransformBuffer        { ACS::Attach<ComputeBuffer<GlTransform>>(this, mNumSprites.x * mNumSprites.y) };

  WorldManager(Object* pObject);
  virtual ~WorldManager();
};

/*
* World manager implementation.
*/

WorldManager::WorldManager(Object* pObject)
  : Actor{ pObject }
{
  std::vector<VertexDefault> vertices
  {
    { { 0.f, 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 1.f, 0.f, 0.f }, { 1.f, 0.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 0.f, 1.f, 0.f }, { 0.f, 1.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 1.f, 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 0.f, 0.f, 1.f } },
  };
  std::vector<u32> elements{ 0, 1, 2, 2, 3, 1 };

  mpMeshSprite->Set(vertices.data(), elements.data());

  std::vector<GlTransform> transforms{};

  transforms.resize(mNumSprites.x * mNumSprites.y);

  for (u32 i{}; i < mNumSprites.x; i++)
    for (u32 j{}; j < mNumSprites.y; j++)
    {
      u32 const index{ i + j * mNumSprites.x };

      transforms[index] =
      {
        .mPosition{ i * mSpriteSize.x, j * mSpriteSize.y, 0.f },
        .mScale   { mSpriteSize.x, mSpriteSize.y, 1.f },
      };
    }

  mpTransformBuffer->Set(transforms.data());
}
WorldManager::~WorldManager()
{

}