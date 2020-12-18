#pragma once

#include <Core.h>
#include <Types.h>

/*
* Event state handling.
*/

enum EventType : u32
{
  eEventNone,
  eEventDown,
  eEventHeld,
  eEventUp,
};

struct Event
{
  EventType mActionCurr{ eEventNone };
  EventType mActionPrev{ eEventNone };
};