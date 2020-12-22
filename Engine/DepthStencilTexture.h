#pragma once

#include <Core.h>
#include <Types.h>

/*
* Depth stencil texture base.
*/

struct DepthStencilTexture
{
  u32 mWidth    {};
  u32 mHeight   {};
  u32 mFilter   {};
  u32 mWrapping {};
  u32 mTextureId{};

  DepthStencilTexture(u32 width, u32 height, u32 filter = GL_LINEAR, u32 wrapping = GL_REPEAT);
  virtual ~DepthStencilTexture();
};