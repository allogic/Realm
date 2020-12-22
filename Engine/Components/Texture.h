#pragma once

#include <Core.h>
#include <Types.h>
#include <Component.h>

/*
* Texture base.
*/

struct Texture : Component
{
  u32 mWidth         {};
  u32 mHeight        {};
  u32 mChannels      {};
  u32 mFormatInternal{};
  u32 mFormat        {};
  u32 mFormatType    {};
  u32 mFilter        {};
  u32 mWrapping      {};
  u32 mTextureId     {};

  Texture(u32 width, u32 height, u32 format = GL_RGBA32F, u32 filter = GL_LINEAR, u32 wrapping = GL_REPEAT, void* pTextureBuffer = nullptr);
  Texture(s8 const* pTextureSource, u32 width, u32 height, u32 format = GL_RGBA32F, u32 filter = GL_LINEAR, u32 wrapping = GL_REPEAT);
  virtual ~Texture();

  void Bind();

  void MapSampler(u32 mountIndex);
  void MapTexture(u32 mountIndex);
};