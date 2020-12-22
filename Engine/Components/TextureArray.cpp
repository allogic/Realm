#include <Components/TextureArray.h>

/*
* Texture array base implementation.
*/

TextureArray::TextureArray(u32 numTilesX, u32 numTilesY, u32 tileWidth, u32 tileHeight, u32 format, u32 filter, u32 wrapping, void* pTextureBuffer)
  : mNumTilesX{ numTilesX }
  , mNumTilesY{ numTilesY }
  , mTileWidth{ tileWidth }
  , mTileHeight{ tileHeight }
  , mFormatInternal{ format }
  , mFilter{ filter }
  , mWrapping{ wrapping }
{
  glGenTextures(1, &mTextureId);

  assert(mTextureId);
  assert(mNumTilesX > 0);
  assert(mNumTilesY > 0);
  assert(mTileWidth > 0 && mTileWidth % 2 == 0);
  assert(mTileHeight > 0 && mTileHeight % 2 == 0);

  glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureId);

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

  glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, mTileWidth, mTileHeight, mNumTilesX * mNumTilesY);

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
TextureArray::TextureArray(s8 const* pTextureSource, u32 numTilesX, u32 numTilesY, u32 tileWidth, u32 tileHeight, u32 format, u32 filter, u32 wrapping)
  : TextureArray(numTilesX, numTilesY, tileWidth, tileHeight, format, filter, wrapping, nullptr)
{
  u32 srcWidth{};
  u32 srcHeight{};
  u32 srcChannels{};

  u8* pTextureBuffer{ stbi_load(pTextureSource, (s32*)&srcWidth, (s32*)&srcHeight, (s32*)&srcChannels, STBI_rgb_alpha) };

  assert(pTextureBuffer);
  assert(srcWidth / (mNumTilesX * tileWidth) == 1);
  assert(srcHeight / (mNumTilesY * tileHeight) == 1);
  assert(srcChannels == mChannels);
  assert(GL_RGBA8 == mFormatInternal);

  glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureId);

  std::vector<u8> tile{};

  tile.resize(mTileWidth * mTileHeight * mChannels);

  u32 tileSizeX{ mTileWidth * mChannels };
  u32 rowLen{ mNumTilesX * tileSizeX };

  for (u32 iy{}; iy < mNumTilesY; iy++)
  {
    for (u32 ix{}; ix < mNumTilesX; ix++)
    {
      u8* ptr{ pTextureBuffer + iy * rowLen * mTileHeight + ix * tileSizeX };

      for (u32 row{}; row < mTileHeight; row++)
      {
        std::copy(
          ptr + row * rowLen,
          ptr + row * rowLen + tileSizeX,
          tile.begin() + row * tileSizeX
        );
      }

      u32 i{ iy * mNumTilesX + ix };

      glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, mTileWidth, mTileHeight, 1, mFormat, mFormatType, tile.data());
    }
  }

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

  stbi_image_free(pTextureBuffer);
}
TextureArray::~TextureArray()
{
  glDeleteTextures(1, &mTextureId);
}

void TextureArray::Bind()
{
  glBindTexture(GL_TEXTURE_2D_ARRAY, mTextureId);
}

void TextureArray::MapSampler(u32 mountIndex)
{
  glBindTextureUnit(mountIndex, mTextureId);
}