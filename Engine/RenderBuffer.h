#pragma once

#include <Core.h>
#include <Types.h>

/*
* RenderBuffer base.
*/

struct RenderBuffer
{
  u32 mWidth         {};
  u32 mHeight        {};
  u32 mRenderBufferId{};

  RenderBuffer(u32 width, u32 height);
  virtual ~RenderBuffer();
};

/*
* RenderBuffer base implementation.
*/

RenderBuffer::RenderBuffer(u32 width, u32 height)
  : mWidth{ width }
  , mHeight{ height }
{
  glGenRenderbuffers(1, & mRenderBufferId);

  assert(mRenderBufferId);

  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
}
RenderBuffer::~RenderBuffer()
{

}