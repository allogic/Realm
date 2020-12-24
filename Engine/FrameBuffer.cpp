#include <FrameBuffer.h>

/*
* Default framebuffer implementation.
*/

FrameBuffer::FrameBuffer(u32 width, u32 height)
  : mWidth{ width }
  , mHeight{ height }
  , mTextureAlbedo{ width, height, GL_RGBA32F, GL_NEAREST, GL_CLAMP_TO_EDGE }
  , mTextureDepthStencil{ width, height, GL_NEAREST, GL_CLAMP_TO_EDGE }
{
  glGenFramebuffers(1, &mFrameBufferId);

  assert(mFrameBufferId);

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBufferId);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureAlbedo.mTextureId, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mTextureDepthStencil.mTextureId, 0);

  u32 buffers[]{ GL_COLOR_ATTACHMENT0 };

  glDrawBuffers(1, buffers);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
FrameBuffer::~FrameBuffer()
{
  glDeleteFramebuffers(1, &mFrameBufferId);
}

void FrameBuffer::Bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferId);
}
void FrameBuffer::BindRead()
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, mFrameBufferId);
}
void FrameBuffer::BindWrite()
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBufferId);
}

void FrameBuffer::Unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::UnbindRead()
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
void FrameBuffer::UnbindWrite()
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}