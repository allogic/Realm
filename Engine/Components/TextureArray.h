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

  TextureArray(s8 const* pTextureSource, u32 tileSize, u32 numTilesX);
  virtual ~TextureArray();

  void Bind();
  void Map(u32 mountIndex);
};