#pragma once

#include <Api.h>

#include <Shaders/ComputeDensity.h>
#include <Shaders/ComputeSmoothing.h>
#include <Shaders/ComputeTexture.h>

/*
* Chunk actor.
*/

template<u32 Layer>
struct Chunk : Actor
{
  u32v2                     mNumSprites             { 32, 32 };
  u32v2                     mSpriteSize             { 1, 1 };

  // Render components

  RenderLayer<Layer>*       mpRenderLayer           {};
  RenderShader*             mpShaderRenderSprite    {};
  Mesh<VertexDefault>*      mpMeshSprite            {};
  TextureArray*             mpTextureAtlas          {};
  ComputeBuffer<GlSprite>*  mpBufferSprite          {};

  // Compute specific

  std::vector<r32>          mDensity                {};
  std::vector<GlSprite>     mSprites                {};

  ComputeBuffer<r32>        mBufferDensity          { mNumSprites.x * mNumSprites.y };

  ComputeShader*            mpShaderComputeDensity  {};
  ComputeShader*            mpShaderComputeSmoothing{};
  ComputeShader*            mpShaderComputeTexture  {};

  Chunk(
    Object* pObject,
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

template<u32 Layer>
Chunk<Layer>::Chunk(
  Object* pObject,
  RenderShader* pShaderRenderSprite,
  Mesh<VertexDefault>* pMesh,
  TextureArray* pTextureAtlas,
  ComputeShader* pShaderComputeDensity,
  ComputeShader* pShaderComputeSmoothing,
  ComputeShader* pShaderComputeTexture)
  : mpRenderLayer{ ACS::Attach<RenderLayer<Layer>>(this) }
  , mpShaderRenderSprite{ ACS::AttachShared<RenderShader>(this, pShaderRenderSprite) }
  , mpMeshSprite{ ACS::AttachShared<Mesh<VertexDefault>>(this, pMesh) }
  , mpTextureAtlas{ ACS::AttachShared<TextureArray>(this, pTextureAtlas) }
  , mpBufferSprite{ ACS::Attach<ComputeBuffer<GlSprite>>(this, mNumSprites.x * mNumSprites.y) }
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

template<u32 Layer>
void Chunk<Layer>::OnUpdateFixed(r32 time, r32 timeDelta)
{
  
}
template<u32 Layer>
void Chunk<Layer>::OnGizmo()
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

template<u32 Layer>
void Chunk<Layer>::InitializeDensity()
{
  mDensity.resize(mNumSprites.x * mNumSprites.y);

  mBufferDensity.Set(mDensity.data());
}
template<u32 Layer>
void Chunk<Layer>::InitializeSprites()
{
  mSprites.resize(mNumSprites.x * mNumSprites.y);

  for (u32 i{}; i < mNumSprites.x; i++)
    for (u32 j{}; j < mNumSprites.y; j++)
    {
      u32 const index{ i + j * mNumSprites.x };

      mSprites[index] =
      {
        .mPosition{ i * mSpriteSize.x, j * mSpriteSize.y, Layer, 0.f },
        .mScale   { mSpriteSize.x, mSpriteSize.y, 1.f, 1.f },
      };
    }

  mpBufferSprite->Set(mSprites.data());
}

template<u32 Layer>
void Chunk<Layer>::ComputeDensity()
{
  mBufferDensity.Map(0);

  mpShaderComputeDensity->Bind();
  mpShaderComputeDensity->SetS32("uLayer", Layer);
  mpShaderComputeDensity->Execute(mNumSprites.x / 32, mNumSprites.y / 32, 1);

  mBufferDensity.Get(mDensity.data());
}
template<u32 Layer>
void Chunk<Layer>::ComputeSmoothing()
{
  mBufferDensity.Map(0);

  mpShaderComputeSmoothing->Bind();

  for (u32 i{}; i < 5; i++)
    mpShaderComputeSmoothing->Execute(mNumSprites.x / 32, mNumSprites.y / 32, 1);

  mBufferDensity.Get(mDensity.data());
}
template<u32 Layer>
void Chunk<Layer>::ComputeTextures()
{
  mpBufferSprite->Map(0);
  mBufferDensity.Map(1);

  mpShaderComputeTexture->Bind();
  mpShaderComputeTexture->Execute(mNumSprites.x / 32, mNumSprites.y / 32, 1);
}

template<u32 Layer>
void Chunk<Layer>::SetDensity(r32v2 tilePosition, r32 density)
{
  u32 tileIndex{ (u32)(tilePosition.x + tilePosition.y * mNumSprites.x) };

  mBufferDensity.Set(&density, tileIndex, 1);
}
template<u32 Layer>
void Chunk<Layer>::SetSprite(r32v2 tilePosition, u32 textureIndex)
{
  GlSprite sprite
  {
    .mPosition    { tilePosition, 0.f, 0.f },
    .mTextureIndex{ textureIndex },
  };
  u32 tileIndex{ (u32)(tilePosition.x + tilePosition.y * mNumSprites.x) };

  mpBufferSprite->Set(&sprite, tileIndex, 1);
}