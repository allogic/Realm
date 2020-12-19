#pragma once

#include <Core.h>
#include <Types.h>

/*
* Forward decls.
*/

struct Actor;

/*
* Component base.
*/

struct Component
{
  Actor* mpActor{};
};