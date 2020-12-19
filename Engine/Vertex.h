#pragma once

#include <Core.h>
#include <Types.h>

/*
* Global vertex layouts.
*/

enum VertexType
{
  eVertexTypeDefault,
  eVertexTypeGizmo,
};

struct VertexDefault
{
  constexpr static VertexType sType{ eVertexTypeDefault };

  r32v3 mPosition{};
  r32v2 mUv      {};
  r32v4 mColor   {};
};
struct VertexGizmo
{
  constexpr static VertexType sType{ eVertexTypeGizmo };

  r32v3 mPosition{};
  r32v4 mColor   {};
};