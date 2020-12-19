#pragma once

#include <Api.h>

/*
* Default camera controller.
*/

struct Controller : Component
{
  r32   mPositionSpeed{ 0.4f };
  r32   mRotationSpeed{ 1.8f };
  r32   mZoomSpeed    { 100.f };
  r32   mPositionDecay{ 8.f };
  r32   mRotationDecay{ 1.5f };
  r32   mZoomDecay    { 20.f };
  r32v3 mPositionAccel{};
  r32v3 mRotationAccel{};
  r32   mZoomAccel    {};
};