#pragma once

#include <Core.h>
#include <Types.h>

/*
* Sprite buffer layouts.
*/

struct alignas(4) GlSprite
{
  r32v4 mPosition    {};
  r32v4 mRotation    {};
  r32v4 mScale       { 1.f, 1.f, 1.f, 1.f };
  u32   mTextureIndex{};
};