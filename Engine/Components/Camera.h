#pragma once

#include <Core.h>
#include <Types.h>
#include <Component.h>

/*
* Default camera.
*/

struct Camera : Component
{
  r32   mFov       {};
  r32v3 mRight     { 1.f, 0.f, 0.f };
  r32v3 mUp        { 0.f, 1.f, 0.f };
  r32v3 mFront     { 0.f, 0.f, -1.f };
  r32v3 mLocalRight{ mRight };
  r32v3 mLocalUp   { mUp };
  r32v3 mLocalFront{ mFront };
  r32   mNear      { 0.01f };
  r32   mFar       { 100.f };
  r32   mZoom      { 10.f };
};