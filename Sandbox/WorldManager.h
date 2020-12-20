#pragma once

#include <Api.h>

#include <Shaders/Sprite.h>

/*
* World manager.
*/

struct WorldManager : Actor
{
  u32v2                     mNumSprites              { 64, 64 };
  u32v2                     mSpriteSize              { 1, 1 };

  TextureArray*             mpTextureEnvironmentAtlas{ ACS::Attach<TextureArray>(this, ROOT_PATH "Textures\\EnvironmentAtlas.png", 10u, 10u, 16u, 16u) };

  RenderShader*             mpShaderRenderSprite     { ACS::Attach<RenderShader>(this, SHADER_RENDER_SPRITE_VERTEX, SHADER_RENDER_SPRITE_FRAGMENT) };
  ComputeShader*            mpShaderComputeWorld     { ACS::Attach<ComputeShader>(this, SHADER_COMPUTE_SPRITE_WORLD) };

  Mesh<VertexDefault>*      mpMeshSprite             { ACS::Attach<Mesh<VertexDefault>>(this, 4u, 6u) };
  UniformBuffer<GlMap>*     mpUniformMap             { ACS::Attach<UniformBuffer<GlMap>>(this, 1u) };
  ComputeBuffer<GlSprite>*  mpBufferSprite           { ACS::Attach<ComputeBuffer<GlSprite>>(this, mNumSprites.x * mNumSprites.y) };

  WorldManager(Object* pObject);
  virtual ~WorldManager();

  void OnUpdateFixed(r32 time, r32 timeDelta) override;

  void SetSprite(r32v2 tilePosition, u32 textureIndex);
};

/*
* World manager implementation.
*/

WorldManager::WorldManager(Object* pObject)
  : Actor{ pObject }
{
  std::vector<VertexDefault> vertices
  {
    { { 0.f, 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 1.f, 0.f, 0.f }, { 1.f, 0.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 0.f, 1.f, 0.f }, { 0.f, 1.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 1.f, 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 0.f, 0.f, 1.f } },
  };
  std::vector<u32> elements{ 0, 1, 2, 2, 3, 1 };

  mpMeshSprite->Set(vertices.data(), elements.data());

  std::vector<GlSprite> sprites{};

  sprites.resize(mNumSprites.x * mNumSprites.y);

  for (u32 i{}; i < mNumSprites.x; i++)
    for (u32 j{}; j < mNumSprites.y; j++)
    {
      u32 const index{ i + j * mNumSprites.x };

      sprites[index] =
      {
        .mPosition{ i * mSpriteSize.x, j * mSpriteSize.y, 0.f, 0.f },
        .mScale   { mSpriteSize.x, mSpriteSize.y, 1.f, 1.f },
      };
    }

  mpBufferSprite->Set(sprites.data());

  GlMap map
  {
    .mWidth { mNumSprites.x },
    .mHeight{ mNumSprites.y },
  };

  mpUniformMap->Set(&map);

  mpUniformMap->Map(0);

  mpBufferSprite->Map(0);

  mpShaderComputeWorld->Bind();
  mpShaderComputeWorld->Execute(mNumSprites.x * mNumSprites.y / 32, 1, 1);
}
WorldManager::~WorldManager()
{

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

void WorldManager::OnUpdateFixed(r32 time, r32 timeDelta)
{

}