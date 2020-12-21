#pragma once

#include <Api.h>

#include <Shaders/ComputeDensity.h>
#include <Shaders/ComputeTexture.h>

/*
* World manager.
*/

struct WorldManager : Actor
{
  u32v2                     mNumSprites              { 64, 64 };
  u32v2                     mSpriteSize              { 1, 1 };

  // Render components

  RenderShader*             mpShaderRenderSprite     { ACS::Attach<RenderShader>(this, SHADER_RENDER_SPRITE_VERTEX, SHADER_RENDER_SPRITE_FRAGMENT) };

  Mesh<VertexDefault>*      mpMeshSprite             { ACS::Attach<Mesh<VertexDefault>>(this, 4u, 6u) };

  ComputeBuffer<GlSprite>*  mpBufferSprite           { ACS::Attach<ComputeBuffer<GlSprite>>(this, mNumSprites.x * mNumSprites.y) };

  TextureArray*             mpTextureEnvironmentAtlas{ ACS::Attach<TextureArray>(this, ROOT_PATH "Textures\\EnvironmentAtlas.png", 10u, 10u, 16u, 16u) };

  // Compute specific

  std::vector<r32>          mDensity                 {};
  std::vector<GlSprite>     mSprites                 {};

  ComputeBuffer<r32>        mBufferDensity           { mNumSprites.x * mNumSprites.y };

  ComputeShader             mShaderComputeDensity    { SHADER_COMPUTE_DENSITY };
  ComputeShader             mShaderComputeTexture    { SHADER_COMPUTE_TEXTURE };

  WorldManager(Object* pObject);
  virtual ~WorldManager();

  void OnUpdateFixed(r32 time, r32 timeDelta) override;
  void OnGizmo() override;

  void InitializeMesh();
  void InitializeSprites();

  void ComputeDensityMap();
  void ComputeSpriteTextures();

  void SetSprite(r32v2 tilePosition, u32 textureIndex);
};

/*
* World manager implementation.
*/

WorldManager::WorldManager(Object* pObject)
  : Actor{ pObject }
{
  InitializeMesh();
  InitializeSprites();

  ComputeDensityMap();
  ComputeSpriteTextures();
}
WorldManager::~WorldManager()
{

}

void WorldManager::OnUpdateFixed(r32 time, r32 timeDelta)
{

}
void WorldManager::OnGizmo()
{
  mpBufferSprite->Get(mSprites.data());

  for (auto const& sprite : mSprites)
  {
    r32v4 color{ sprite.mTextureIndex == 11 ? r32v4{ 0.f, 0.f, 0.f, 0.2f } : r32v4{ 1.f, 1.f, 1.f, 0.2f } };

    mpRenderer->PushRect({ sprite.mPosition.x + 0.5f, sprite.mPosition.y + 0.5f, 0.f }, { 0.5f, 0.5f, 1.f }, color);
  }
}

void WorldManager::InitializeMesh()
{
  std::vector<VertexDefault> vertices
  {
    { { 0.f, 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 0.f, 1.f, 0.f }, { 1.f, 0.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 1.f, 0.f, 0.f }, { 0.f, 1.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 1.f, 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 0.f, 0.f, 1.f } },
  };
  std::vector<u32> elements{ 0, 1, 2, 2, 3, 1 };

  mpMeshSprite->Set(vertices.data(), elements.data());
}
void WorldManager::InitializeSprites()
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

void WorldManager::ComputeDensityMap()
{
  mBufferDensity.Map(0);

  mShaderComputeDensity.Bind();
  mShaderComputeDensity.Execute(mNumSprites.x / 32, mNumSprites.y / 32, 1);
}
void WorldManager::ComputeSpriteTextures()
{
  mpBufferSprite->Map(0);
  mBufferDensity.Map(1);

  mShaderComputeTexture.Bind();
  mShaderComputeTexture.Execute(mNumSprites.x * mNumSprites.y / 32, 1, 1);
}

void WorldManager::SetSprite(r32v2 tilePosition, u32 textureIndex)
{
  GlSprite sprite
  {
    .mPosition    { tilePosition, 0.f, 0.f },
    .mTextureIndex{ textureIndex },
  };
  u32 tileIndex{ (u32)(tilePosition.x + tilePosition.y * mNumSprites.x) };

  mpBufferSprite->Set(&sprite, tileIndex, 1);
}