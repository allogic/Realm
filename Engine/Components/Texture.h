#pragma once

#include <Core.h>
#include <Types.h>
#include <Component.h>

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