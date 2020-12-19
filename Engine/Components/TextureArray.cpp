#include <Components/TextureArray.h>

/*
* Texture array base implementation.
*/

TextureArray::TextureArray(s8 const* pTextureSource, u32 tileSize, u32 numTiles)
{
  u32 width{};
  u32 height{};
  u32 channels{};
  u32 type{ STBI_rgb_alpha };

  u8* pTextureBuffer{ stbi_load(pTextureSource, (s32*)&width, (s32*)&height, (s32*)&channels, (s32)type) };

  assert(pTextureBuffer);
  assert(width > 0 && width % 2 == 0);
  assert(height > 0 && height % 2 == 0);
  assert(channels == 4);

  std::vector<r32> textureBuffer{};

  textureBuffer.resize(width * height * channels);

  for (u32 i{}; i < textureBuffer.size(); i++)
    textureBuffer[i] = (r32)pTextureBuffer[i] / 255.f;

  glGenTextures(1, &mTextureId);

  assert(mTextureId);

  glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureId);
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, tileSize - 2, tileSize - 2, numTiles);

  glTextureParameteri(mTextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(mTextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTextureParameteri(mTextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(mTextureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, tileSize - 2, tileSize - 2, numTiles, GL_RGBA, GL_FLOAT, textureBuffer.data());

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
TextureArray::~TextureArray()
{

}

void TextureArray::Bind()
{
  glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureId);
}
void TextureArray::Map(u32 mountIndex)
{
  glBindTextureUnit(mountIndex, mTextureId);
}