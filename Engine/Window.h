#pragma once

#include <Core.h>
#include <Types.h>
#include <Event.h>

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

  Window(u32 width, u32 height, s8 const* pTitle, u8 registerDebugMessenger);
  virtual ~Window();

  u32 Run();
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

/*
* Window graphic context implementation.
*/

Window::Window(u32 width, u32 height, s8 const* pTitle, u8 registerDebugMessenger)
  : mWidth{ width }
  , mHeight{ height }
{
  glfwInit();

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
  glfwWindowHint(GLFW_DECORATED, GL_TRUE);
  glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 8);

  mpContext = glfwCreateWindow((s32)width, (s32)height, pTitle, nullptr, nullptr);

  assert(mpContext);

  glfwMakeContextCurrent(mpContext);

  gladLoadGL();

  if (registerDebugMessenger)
  {
    glEnable(GL_DEBUG_OUTPUT);

    glDebugMessageCallback([](u32 source, u32 type, u32 id, u32 severity, s32 length, s8 const* pMsg, void const* pUserParam)
      {
        std::printf("Error[%u] %s\n", type, pMsg);
      }, 0);
  }

  glfwSwapInterval(0);

  glfwSetWindowUserPointer(mpContext, this);

  glfwSetErrorCallback([](s32 errorCode, s8 const* pMsg)
    {
      std::printf("Error[%u] %s\n", errorCode, pMsg);
    });

  glfwSetWindowCloseCallback(mpContext, [](GLFWwindow* pContext)
    {
      Window* pWindow = (Window*)glfwGetWindowUserPointer(pContext);

      pWindow->mStatus = 1;
    });
  glfwSetWindowSizeCallback(mpContext, [](GLFWwindow* pContext, s32 width, s32 height)
    {
      Window* pWindow = (Window*)glfwGetWindowUserPointer(pContext);

      pWindow->mWidth = (u32)width;
      pWindow->mHeight = (u32)height;
    });
  glfwSetCursorPosCallback(mpContext, [](GLFWwindow* pContext, r64 x, r64 y)
    {
      Window* pWindow = (Window*)glfwGetWindowUserPointer(pContext);

      pWindow->mMousePosition = { (r32)x, (r32)y };
    });
  glfwSetScrollCallback(mpContext, [](GLFWwindow* pContext, r64 xOffset, r64 yOffset)
    {
      Window* pWindow = (Window*)glfwGetWindowUserPointer(pContext);

      pWindow->mMouseScrollWheel = { (r32)xOffset, (r32)yOffset };
    });
}
Window::~Window()
{
  glfwDestroyWindow(mpContext);
}

u32 Window::Run()
{
  r32 time{};
  r32 timePrev{};
  r32 timeDelta{};

  r32 timeRender{ 1.f / 60 };
  r32 timeRenderPrev{};

  while (!mStatus)
  {
    PollEvents();

    time = (r32)glfwGetTime();
    timeDelta = time - timePrev;

    if ((time - timeRenderPrev) >= timeRender)
    {
      glfwSwapBuffers(mpContext);

      timeRenderPrev = time;
    }

    timePrev = time;
  }

  return mStatus;
}
void Window::PollEvents()
{
  mMouseScrollWheel = {};

  glfwPollEvents();

  for (u32 i{}; i < GLFW_MOUSE_BUTTON_LAST; i++)
  {
    Event& event{ mMouseKeyStates[i] };
    s32 action{ glfwGetMouseButton(mpContext, (s32)i) };

    event.mActionPrev = event.mActionCurr;

    if (action == GLFW_PRESS)
    {
      if (event.mActionCurr != eEventDown && event.mActionPrev != eEventHeld)
      {
        event.mActionCurr = eEventDown;
      }
      else
      {
        event.mActionCurr = eEventHeld;
      }
    }
    if (action == GLFW_RELEASE)
    {
      if (event.mActionCurr != eEventUp && event.mActionPrev == eEventHeld)
      {
        event.mActionCurr = eEventUp;
      }
      else
      {
        event.mActionCurr = eEventNone;
      }
    }
  }
  for (u32 i{ 32 }; i < GLFW_KEY_LAST; i++)
  {
    Event& event{ mKeyboardKeyStates[i] };
    s32 action{ glfwGetKey(mpContext, (s32)i) };

    if (action == GLFW_PRESS)
    {
      if (event.mActionCurr != eEventDown && event.mActionPrev != eEventHeld)
      {
        event.mActionCurr = eEventDown;
      }
      else
      {
        event.mActionCurr = eEventHeld;
      }
    }
    if (action == GLFW_RELEASE)
    {
      if (event.mActionCurr != eEventUp && event.mActionPrev == eEventHeld)
      {
        event.mActionCurr = eEventUp;
      }
      else
      {
        event.mActionCurr = eEventNone;
      }
    }
  }
}
u32 Window::WindowStatus()
{
  return mStatus;
}
r32v2 Window::WindowSize()
{
  return { mWidth, mHeight };
}
r32 Window::AspectRatio()
{
  return (r32)mWidth / mHeight;
}
r32v2 Window::MousePosition()
{
  return mMousePosition;
}
r32v2 Window::MouseScroll()
{
  return mMouseScrollWheel;
}
u32 Window::MouseDown(u32 key)
{
  return mMouseKeyStates[key].mActionCurr == eEventDown;
}
u32 Window::MouseHeld(u32 key)
{
  return mMouseKeyStates[key].mActionCurr == eEventHeld;
}
u32 Window::MouseUp(u32 key)
{
  return mMouseKeyStates[key].mActionCurr == eEventUp;
}
u32 Window::KeyDown(u32 key)
{
  return mKeyboardKeyStates[key].mActionCurr == eEventDown;
}
u32 Window::KeyHeld(u32 key)
{
  return mKeyboardKeyStates[key].mActionCurr == eEventHeld;
}
u32 Window::KeyUp(u32 key)
{
  return mKeyboardKeyStates[key].mActionCurr == eEventUp;
}