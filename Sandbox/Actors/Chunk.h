#pragma once

#include <Api.h>

#include <Shaders/ComputeDensity.h>
#include <Shaders/ComputeSmoothing.h>
#include <Shaders/ComputeTexture.h>

/*
* Chunk actor.
*/

struct Chunk : Actor
{
  u32v2                     mNumSprites             { 32, 32 };
  u32v2                     mSpriteSize             { 1, 1 };

  s32                       mLayer                  {};

  // Render components

  RenderShader*             mpShaderRenderSprite    {};
  Mesh<VertexDefault>*      mpMeshSprite            {};
  TextureArray*             mpTextureAtlas          {};
  ComputeBuffer<GlSprite>*  mpBufferSprite          { ACS::Attach<ComputeBuffer<GlSprite>>(this, mNumSprites.x * mNumSprites.y) };

  // Compute specific

  std::vector<r32>          mDensity                {};
  std::vector<GlSprite>     mSprites                {};

  ComputeBuffer<r32>        mBufferDensity          { mNumSprites.x * mNumSprites.y };

  ComputeShader*            mpShaderComputeDensity  {};
  ComputeShader*            mpShaderComputeSmoothing{};
  ComputeShader*            mpShaderComputeTexture  {};

  Chunk(
    Object* pObject,
    s32 layer,
    RenderShader* pShaderRenderSprite,
    Mesh<VertexDefault>* pMesh,
    TextureArray* pTextureAtlas,
    ComputeShader* pShaderComputeDensity,
    ComputeShader* pShaderComputeSmoothing,
    ComputeShader* pShaderComputeTexture
  );

  void OnUpdateFixed(r32 time, r32 timeDelta) override;
  void OnGizmo() override;

  void InitializeDensity();
  void InitializeSprites();

  void ComputeDensity();
  void ComputeSmoothing();
  void ComputeTextures();

  void SetDensity(r32v2 tilePosition, r32 density);
  void SetSprite(r32v2 tilePosition, u32 textureIndex);
};

/*
* Chunk actor implementation.
*/

Chunk::Chunk(
  Object* pObject,
  s32 layer,
  RenderShader* pShaderRenderSprite,
  Mesh<VertexDefault>* pMesh,
  TextureArray* pTextureAtlas,
  ComputeShader* pShaderComputeDensity,
  ComputeShader* pShaderComputeSmoothing,
  ComputeShader* pShaderComputeTexture)
  : mLayer{ layer }
  , mpShaderRenderSprite{ ACS::AttachShared<RenderShader>(this, pShaderRenderSprite) }
  , mpMeshSprite{ ACS::AttachShared<Mesh<VertexDefault>>(this, pMesh) }
  , mpTextureAtlas{ ACS::AttachShared<TextureArray>(this, pTextureAtlas) }
  , mpShaderComputeDensity{ pShaderComputeDensity }
  , mpShaderComputeSmoothing{ pShaderComputeSmoothing }
  , mpShaderComputeTexture{ pShaderComputeTexture }
  , Actor{ pObject }
{
  InitializeDensity();
  InitializeSprites();

  ComputeDensity();
  ComputeSmoothing();
  ComputeTextures();
}

void Chunk::OnUpdateFixed(r32 time, r32 timeDelta)
{
  
}
void Chunk::OnGizmo()
{
  mBufferDensity.Get(mDensity.data());

  for (u32 i{}; i < mNumSprites.x; i++)
    for (u32 j{}; j < mNumSprites.y; j++)
    {
      u32 index{ i + j * mNumSprites.x };

      r32v3 position{ mpTransform->mPosition + r32v3{ i + 0.5f, j + 0.5f, 0.f } };
      r32v4 color{ mDensity[index] > 0.5f ? r32v4{ 1.f, 0.f, 0.f, 1.f } : r32v4{ 0.f, 0.f, 1.f, 1.f } };

      mpRenderer->PushRect(position, { 0.5f, 0.5f, 1.f }, color);
    }
}

void Chunk::InitializeDensity()
{
  mDensity.resize(mNumSprites.x * mNumSprites.y);

  mBufferDensity.Set(mDensity.data());
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
        .mPosition{ i * mSpriteSize.x, j * mSpriteSize.y, mLayer, 0.f },
        .mScale   { mSpriteSize.x, mSpriteSize.y, 1.f, 1.f },
      };
    }

  mpBufferSprite->Set(mSprites.data());
}

void Chunk::ComputeDensity()
{
  mBufferDensity.Map(0);

  mpShaderComputeDensity->Bind();
  mpShaderComputeDensity->SetS32("uLayer", mLayer);
  mpShaderComputeDensity->Execute(mNumSprites.x / 32, mNumSprites.y / 32, 1);

  mBufferDensity.Get(mDensity.data());
}
void Chunk::ComputeSmoothing()
{
  mBufferDensity.Map(0);

  mpShaderComputeSmoothing->Bind();

  for (u32 i{}; i < 5; i++)
    mpShaderComputeSmoothing->Execute(mNumSprites.x / 32, mNumSprites.y / 32, 1);

  mBufferDensity.Get(mDensity.data());
}
void Chunk::ComputeTextures()
{
  mpBufferSprite->Map(0);
  mBufferDensity.Map(1);

  mpShaderComputeTexture->Bind();
  mpShaderComputeTexture->Execute(mNumSprites.x / 32, mNumSprites.y / 32, 1);
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