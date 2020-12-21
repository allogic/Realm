#pragma once

#include <Api.h>

#include <Shaders/ComputeDensity.h>
#include <Shaders/ComputeTexture.h>

/*
* Chunk actor.
*/

struct Chunk : Actor
{
  u32v2                     mNumSprites          { 64, 64 };
  u32v2                     mSpriteSize          { 1, 1 };

  s32                       mDepth               {};

  // Render components

  RenderShader*             mpShaderRenderSprite {};
  Mesh<VertexDefault>*      mpMeshSprite         {};
  TextureArray*             mpTextureAtlas       {};
  ComputeBuffer<GlSprite>*  mpBufferSprite       { ACS::Attach<ComputeBuffer<GlSprite>>(this, mNumSprites.x * mNumSprites.y) };

  // Compute specific

  std::vector<r32>          mDensity             {};
  std::vector<GlSprite>     mSprites             {};

  ComputeBuffer<r32>        mBufferDensity       { mNumSprites.x * mNumSprites.y };

  ComputeShader             mShaderComputeDensity{ SHADER_COMPUTE_DENSITY };
  ComputeShader             mShaderComputeTexture{ SHADER_COMPUTE_TEXTURE };

  Chunk(
    Object* pObject,
    s32 depth,
    RenderShader* pRenderShader,
    Mesh<VertexDefault>* pMesh,
    TextureArray* pTextureAtlas
  );

  void OnUpdateFixed(r32 time, r32 timeDelta) override;
  void OnGizmo() override;

  void InitializeSprites();

  void ComputeDensityMap();
  void ComputeSpriteTextures();

  void SetDensity(r32v2 tilePosition, r32 density);
  void SetSprite(r32v2 tilePosition, u32 textureIndex);
};

/*
* Chunk actor implementation.
*/

Chunk::Chunk(
  Object* pObject,
  s32 depth,
  RenderShader* pRenderShader,
  Mesh<VertexDefault>* pMesh,
  TextureArray* pTextureAtlas)
  : mDepth{ depth }
  , mpShaderRenderSprite{ ACS::AttachShared<RenderShader>(this, pRenderShader) }
  , mpMeshSprite{ ACS::AttachShared<Mesh<VertexDefault>>(this, pMesh) }
  , mpTextureAtlas{ ACS::AttachShared<TextureArray>(this, pTextureAtlas) }
  , Actor{ pObject }
{
  InitializeSprites();

  ComputeDensityMap();
}

void Chunk::OnUpdateFixed(r32 time, r32 timeDelta)
{
  ComputeSpriteTextures();
}
void Chunk::OnGizmo()
{
  mBufferDensity.Get(mDensity.data());

  for (u32 i{}; i < mNumSprites.x; i++)
    for (u32 j{}; j < mNumSprites.y; j++)
    {
      u32 index{ i + j * mNumSprites.x };

      r32v4 color{ mDensity[index] > 0.5f ? r32v4{ 1.f, 0.f, 0.f, 0.2f } : r32v4{ 0.f, 0.f, 1.f, 0.2f } };

      mpRenderer->PushRect({ i + 0.5f, j + 0.5f, 0.f }, { 0.5f, 0.5f, 1.f }, color);
    }
}

void Chunk::InitializeSprites()
{
  mSprites.resize(mNumSprites.x * mNumSprites.y);

  for (u32 i{}; i < mNumSprites.x; i++)
    for (u32 j{}; j < mNumSprites.y; j++)
    {
      u32 const index{ i + j * mNumSprites.x };

      mSprites[index] =
      {
        .mPosition{ i * mSpriteSize.x, j * mSpriteSize.y, 0.f, 0.f },
        .mScale   { mSpriteSize.x, mSpriteSize.y, 1.f, 1.f },
      };
    }

  mpBufferSprite->Set(mSprites.data());
}

void Chunk::ComputeDensityMap()
{
  mBufferDensity.Map(0);

  mShaderComputeDensity.Bind();
  mShaderComputeDensity.Execute(mNumSprites.x / 32, mNumSprites.y / 32, 1);

  mDensity.resize(mNumSprites.x * mNumSprites.y);

  mBufferDensity.Get(mDensity.data());
}
void Chunk::ComputeSpriteTextures()
{
  mpBufferSprite->Map(0);
  mBufferDensity.Map(1);

  mShaderComputeTexture.Bind();
  mShaderComputeTexture.Execute(mNumSprites.x / 32, mNumSprites.y / 32, 1);
}

void Chunk::SetDensity(r32v2 tilePosition, r32 density)
{
  u32 tileIndex{ (u32)(tilePosition.x + tilePosition.y * mNumSprites.x) };

  mBufferDensity.Set(&density, tileIndex, 1);
}
void Chunk::SetSprite(r32v2 tilePosition, u32 textureIndex)
{
  GlSprite sprite
  {
    .mPosition    { tilePosition, 0.f, 0.f },
    .mTextureIndex{ textureIndex },
  };
  u32 tileIndex{ (u32)(tilePosition.x + tilePosition.y * mNumSprites.x) };

  mpBufferSprite->Set(&sprite, tileIndex, 1);
}