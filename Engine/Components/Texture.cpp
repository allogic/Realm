#include <Components/Texture.h>

/*
* Texture base implementation.
*/

Texture::Texture(u32 width, u32 height, u32 format, u32 filter, u32 wrapping, void* pTextureBuffer)
  : mWidth{ width }
  , mHeight{ height }
  , mFormatInternal{ format }
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

  switch (mFormatInternal)
  {
    case GL_RGB8:
    {
      mChannels = 3;

      mFormat = GL_RGB;
      mFormatType = GL_UNSIGNED_BYTE;
      break;
    }
    case GL_RGBA8:
    {
      mChannels = 4;

      mFormat = GL_RGBA;
      mFormatType = GL_UNSIGNED_BYTE;
      break;
    }
    case GL_RGB32F:
    {
      mChannels = 3;

      mFormat = GL_RGB;
      mFormatType = GL_FLOAT;
      break;
    }
    case GL_RGBA32F:
    {
      mChannels = 4;

      mFormat = GL_RGBA;
      mFormatType = GL_FLOAT;
      break;
    }
  }

  glTexImage2D(GL_TEXTURE_2D, 0, mFormatInternal, mWidth, mHeight, 0, mFormat, mFormatType, pTextureBuffer);

  glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::Texture(s8 const* pTextureSource, u32 width, u32 height, u32 format, u32 filter, u32 wrapping)
  : Texture(width, height, format, filter, wrapping, nullptr)
{
  u32 srcWidth{};
  u32 srcHeight{};
  u32 srcChannels{};

  u8* pTextureBuffer{ stbi_load(pTextureSource, (s32*)&srcWidth, (s32*)&srcHeight, (s32*)&srcChannels, STBI_rgb_alpha) };

  assert(pTextureBuffer);
  assert(srcWidth == mWidth);
  assert(srcHeight == mHeight);
  assert(srcChannels == mChannels);
  assert(GL_RGBA8 == mFormatInternal);

  glBindTexture(GL_TEXTURE_2D, mTextureId);

  glTexImage2D(GL_TEXTURE_2D, 0, mFormatInternal, mWidth, mHeight, 0, mFormat, mFormatType, pTextureBuffer);

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(pTextureBuffer);
}
Texture::~Texture()
{
  glDeleteTextures(1, &mTextureId);
}

void Texture::Bind()
{
  glBindTexture(GL_TEXTURE_2D, mTextureId);
}

void Texture::MapSampler(u32 mountIndex)
{
  glBindTextureUnit(mountIndex, mTextureId);
}
void Texture::MapTexture(u32 mountIndex)
{
  glBindImageTexture(mountIndex, mTextureId, 0, GL_FALSE, 0, GL_READ_WRITE, mFormatInternal);
}