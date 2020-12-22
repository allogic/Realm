#pragma once

#include <Core.h>
#include <Types.h>
#include <Component.h>

/*
* Texture array base.
*/

struct TextureArray : Component
{
  u32 mNumTilesX     {};
  u32 mNumTilesY     {};
  u32 mTileWidth     {};
  u32 mTileHeight    {};
  u32 mChannels      {};
  u32 mFormatInternal{};
  u32 mFormat        {};
  u32 mFormatType    {};
  u32 mFilter        {};
  u32 mWrapping      {};
  u32 mTextureId     {};

  TextureArray(u32 numTilesX, u32 numTilesY, u32 tileWidth, u32 tileHeight, u32 format = GL_RGBA32F, u32 filter = GL_LINEAR, u32 wrapping = GL_REPEAT, void* pTextureBuffer = nullptr);
  TextureArray(s8 const* pTextureSource, u32 numTilesX, u32 numTilesY, u32 tileWidth, u32 tileHeight, u32 format = GL_RGBA32F, u32 filter = GL_LINEAR, u32 wrapping = GL_REPEAT);
  virtual ~TextureArray();

  void Bind();

  void MapSampler(u32 mountIndex);
};