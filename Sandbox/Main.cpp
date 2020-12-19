#pragma once

#define ENGINE_IMPLEMENTATION
#include <Api.h>

#include <WorldManager.h>
#include <Player.h>

s32 main()
{
  Window* mpWindow{ Instance<Window>::Get("Window", 1280u, 720u, "Realm", 1u) };

  WorldManager* pWorldManager{ ACS::Create<WorldManager>("WorldManager") };
  Player* pPlayer{ ACS::Create<Player>("Player") };

  return mpWindow->Run();
}