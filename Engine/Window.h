#pragma once

#include <Core.h>
#include <Types.h>
#include <Event.h>
#include <Instance.h>

/*
* Forward decls.
*/

struct Renderer;

/*
* Window graphic context.
*/

struct Window
{
  GLFWwindow* mpContext{};
  u32         mWidth{};
  u32         mHeight{};
  u32         mStatus{};
  r32v2       mMousePosition{};
  r32v2       mMouseScrollWheel{};
  Event       mMouseKeyStates[GLFW_MOUSE_BUTTON_LAST]{};
  Event       mKeyboardKeyStates[GLFW_KEY_LAST - 32]{};

  Window();
  Window(u32 width, u32 height, s8 const* pTitle, u32 registerDebugMessenger);
  virtual ~Window();

  u32 Run(Renderer* pRenderer);
  void PollEvents();

  u32 WindowStatus();
  r32v2 WindowSize();
  r32 AspectRatio();
  r32v2 MousePosition();
  r32v2 MouseScroll();
  u32 MouseDown(u32 key);
  u32 MouseHeld(u32 key);
  u32 MouseUp(u32 key);
  u32 KeyDown(u32 key);
  u32 KeyHeld(u32 key);
  u32 KeyUp(u32 key);
};
