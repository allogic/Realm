#pragma once

#include <Core.h>
#include <Types.h>
#include <DepthStencilTexture.h>

#include <Components/Texture.h>

/*
* Default framebuffer.
*/

struct FrameBuffer
{
  u32                 mWidth        {};
  u32                 mHeight       {};
  u32                 mFrameBufferId{};

  Texture             mTextureAlbedo;
  DepthStencilTexture mTextureDepthStencil;

  FrameBuffer(u32 width, u32 height);
  virtual ~FrameBuffer();

  void Bind();
  void BindRead();
  void BindWrite();

  void Unbind();
  void UnbindRead();
  void UnbindWrite();
};