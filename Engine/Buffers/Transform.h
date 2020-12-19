#pragma once

#include <Core.h>
#include <Types.h>

/*
* Transform buffer layouts.
*/

struct alignas(4) GlTransform
{
  r32v3 mPosition{};
  r32v3 mRotation{};
  r32v3 mScale   { 1.f, 1.f, 1.f };
};