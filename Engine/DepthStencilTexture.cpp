#include <DepthStencilTexture.h>

/*
* Depth stencil texture base implementation.
*/

DepthStencilTexture::DepthStencilTexture(u32 width, u32 height, u32 filter, u32 wrapping)
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

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mWidth, mHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);
}
DepthStencilTexture::~DepthStencilTexture()
{
  glDeleteTextures(1, &mTextureId);
}