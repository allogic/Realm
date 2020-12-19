#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

/*
* Texture base.
*/

struct Texture : Component
{
  u32 mTextureId{};

  Texture(s8 const* pTextureSource);
  virtual ~Texture();

  void Bind();
  void Map(u32 mountIndex);
};

/*
* Texture base implementation.
*/

Texture::Texture(s8 const* pTextureSource)
{
  u32 width{};
  u32 height{};
  u32 channels{};
  u32 type{ STBI_rgb_alpha };

  u8* pTextureBuffer{ stbi_load(pTextureSource, (s32*)&width, (s32*)&height, (s32*)&channels, (s32)type) };

  assert(pTextureBuffer);
  assert(width > 0 && width % 2 == 0);
  assert(height > 0 && height % 2 == 0);
  assert(channels == 4);

  std::vector<r32> textureBuffer{};

  textureBuffer.resize(width * height * channels);

  for (u32 i{}; i < textureBuffer.size(); i++)
    textureBuffer[i] = (r32)pTextureBuffer[i] / 255.f;

  glGenTextures(1, &mTextureId);

  assert(mTextureId);

  glBindTexture(GL_TEXTURE_2D, mTextureId);

  glTextureParameteri(mTextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(mTextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTextureParameteri(mTextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(mTextureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, textureBuffer.data());

  glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::~Texture()
{

}

void Texture::Bind()
{
  glBindTexture(GL_TEXTURE_2D, mTextureId);
}
void Texture::Map(u32 mountIndex)
{
  glBindTextureUnit(mountIndex, mTextureId);
}