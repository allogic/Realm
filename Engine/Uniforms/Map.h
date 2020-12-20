#pragma once

#include <Core.h>
#include <Types.h>

/*
* Map uniform layouts.
*/

struct alignas(4) GlMap
{
  u32 mWidth {};
  u32 mHeight{};
};