#pragma once

#include <Core.h>
#include <Types.h>
#include <Component.h>

/*
* Texture array base.
*/

struct TextureArray : Component
{
  u32 mTextureId{};

  TextureArray(s8 const* pTextureSource, u32 numTilesX, u32 numTilesY, u32 tileWidth, u32 tileHeight);
  virtual ~TextureArray();

  void Bind();
  void Map(u32 mountIndex);
};