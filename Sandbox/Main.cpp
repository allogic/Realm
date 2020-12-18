#pragma once

#define ENGINE_IMLEMENTATION
#include <Api.h>

s32 main()
{
  return Window{ 1280, 720, "Realm", 1 }.Run();
}