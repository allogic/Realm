#pragma once

#include <Core.h>
#include <Types.h>

/*
* Projection uniform layouts.
*/

struct alignas(4) GlProjection
{
  r32m4 mProjection{ glm::identity<r32m4>() };
  r32m4 mView      { glm::identity<r32m4>() };
  r32m4 mTransform { glm::identity<r32m4>() };
};