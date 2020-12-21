#pragma once

#include <Api.h>

#include <Actors/Chunk.h>

/*
* Chunk manager actor.
*/

struct ChunkManager : Actor
{
  // Shared resources

  RenderShader        mShaderRenderSprite     { SHADER_RENDER_SPRITE_VERTEX, SHADER_RENDER_SPRITE_FRAGMENT };
  Mesh<VertexDefault> mMeshSprite             { 4u, 6u };
  TextureArray        mTextureEnvironmentAtlas{ ROOT_PATH "Textures\\EnvironmentAtlas.png", 10u, 10u, 16u, 16u };

  // Child chunks

  Chunk*              mpChunkBackground       { ACS::CreateChild<Chunk>(this, "Background", 0, &mShaderRenderSprite, &mMeshSprite, &mTextureEnvironmentAtlas) };
  Chunk*              mpChunkForground        { ACS::CreateChild<Chunk>(this, "Forground", 1, &mShaderRenderSprite, &mMeshSprite, &mTextureEnvironmentAtlas) };

  ChunkManager(Object* pObject);

  void InitializeMesh();
};

/*
* Chunk manager actor implementation.
*/

ChunkManager::ChunkManager(Object* pObject)
  : Actor{ pObject }
{
  InitializeMesh();
}

void ChunkManager::InitializeMesh()
{
  std::vector<VertexDefault> vertices
  {
    { { 0.f, 0.f, 0.f }, { 0.f, 0.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 0.f, 1.f, 0.f }, { 1.f, 0.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 1.f, 0.f, 0.f }, { 0.f, 1.f }, { 0.f, 0.f, 0.f, 1.f } },
    { { 1.f, 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 0.f, 0.f, 1.f } },
  };
  std::vector<u32> elements{ 0, 1, 2, 2, 3, 1 };

  mMeshSprite.Set(vertices.data(), elements.data());
}