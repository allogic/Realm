#include <Components/TextureArray.h>

/*
* Texture array base implementation.
*/

TextureArray::TextureArray(s8 const* pTextureSource, u32 numTilesX, u32 numTilesY, u32 tileWidth, u32 tileHeight)
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

  glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureId);
  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, tileWidth, tileHeight, numTilesX * numTilesY);

  glTextureParameteri(mTextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTextureParameteri(mTextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTextureParameteri(mTextureId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTextureParameteri(mTextureId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  std::vector<u8> tile{};

  tile.resize(tileWidth * tileHeight * channels);

  u32 tileSizeX{ tileWidth * channels };
  u32 rowLen{ numTilesX * tileSizeX };

  for (u32 iy{}; iy < numTilesY; iy++)
  {
    for (u32 ix{}; ix < numTilesX; ix++)
    {
      u8* ptr{ pTextureBuffer + iy * rowLen * tileHeight + ix * tileSizeX };

      for (u32 row{}; row < tileHeight; row++)
      {
        std::copy(
          ptr + row * rowLen,
          ptr + row * rowLen + tileSizeX,
          tile.begin() + row * tileSizeX
        );
      }

      u32 i{ iy * numTilesX + ix };

      glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, tileWidth, tileHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, tile.data());
    }
  }

  stbi_image_free(pTextureBuffer);

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
TextureArray::~TextureArray()
{
  glDeleteTextures(1, &mTextureId);
}

void TextureArray::Bind()
{
  glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureId);
}
void TextureArray::Map(u32 mountIndex)
{
  glBindTextureUnit(mountIndex, mTextureId);
}