#include <Components/Texture.h>

/*
* Texture base implementation.
*/

Texture::Texture(s8 const* pTextureSource)
{
  u32 width{};
  u32 height{};
  u32 channels{};

  u8* pTextureBuffer{ stbi_load(pTextureSource, (s32*)&width, (s32*)&height, (s32*)&channels, STBI_rgb_alpha) };

  assert(pTextureBuffer);
  assert(width > 0 && width % 2 == 0);
  assert(height > 0 && height % 2 == 0);
  assert(channels == 4);

  glGenTextures(1, &mTextureId);

  assert(mTextureId);

  glBindTexture(GL_TEXTURE_2D, mTextureId);

  glTextureParameteri(mTextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(mTextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTextureParameteri(mTextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(mTextureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTextureBuffer);

  stbi_image_free(pTextureBuffer);

  glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::~Texture()
{
  glDeleteTextures(1, &mTextureId);
}

void Texture::Bind()
{
  glBindTexture(GL_TEXTURE_2D, mTextureId);
}
void Texture::Map(u32 mountIndex)
{
  glBindTextureUnit(mountIndex, mTextureId);
}