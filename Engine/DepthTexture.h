#pragma once

#include <Core.h>
#include <Types.h>

/*
* Depth texture base.
*/

struct DepthTexture
{
  u32 mWidth    {};
  u32 mHeight   {};
  u32 mFilter   {};
  u32 mWrapping {};
  u32 mTextureId{};

  DepthTexture(u32 width, u32 height, u32 filter = GL_LINEAR, u32 wrapping = GL_REPEAT);
  virtual ~DepthTexture();
};

/*
* Depth texture base implementation.
*/

DepthTexture::DepthTexture(u32 width, u32 height, u32 filter, u32 wrapping)
  : mWidth{ width }
  , mHeight{ height }
  , mFilter{ filter }
  , mWrapping{ wrapping }
{
  glGenTextures(1, &mTextureId);

  assert(mTextureId);
  assert(mWidth > 0 && mWidth % 2 == 0);
  assert(mHeight > 0 && mHeight % 2 == 0);

  glBindTexture(GL_TEXTURE_2D, mTextureId);

  glTextureParameteri(mTextureId, GL_TEXTURE_WRAP_S, mWrapping);
  glTextureParameteri(mTextureId, GL_TEXTURE_WRAP_T, mWrapping);

  glTextureParameteri(mTextureId, GL_TEXTURE_MIN_FILTER, mFilter);
  glTextureParameteri(mTextureId, GL_TEXTURE_MAG_FILTER, mFilter);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);
}
DepthTexture::~DepthTexture()
{
  glDeleteTextures(1, &mTextureId);
}