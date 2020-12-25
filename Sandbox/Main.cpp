#pragma once

#define ENGINE_IMPLEMENTATION
#include <Api.h>

#include <Actors/ChunkManager.h>
#include <Actors/Player.h>

#include <ShaderFactory/ShaderFactory.h>

s32 main()
{
  Window* mpWindow{ Instance<Window>::Get("Window", 1280u, 720u, "Realm", 1u) };
  Renderer* mpRenderer{ Instance<Renderer>::Get("Renderer", 1280u, 720u) };

  ACS::Create<ChunkManager>("ChunkManager");
  ACS::Create<Player>("Player");

  struct MVP
  {
    Member<r32m4> mProjection{ Registry<MVP, Member<r32m4>>::Resolve() };
    Member<r32m4> mView      { Registry<MVP, Member<r32m4>>::Resolve() };
    Member<r32m4> mModel     { Registry<MVP, Member<r32m4>>::Resolve() };
  };

  struct Sprite
  {
    Member<r32v4> mPosition    { Registry<Sprite, Member<r32v4>>::Resolve() };
    Member<r32v4> mRotation    { Registry<Sprite, Member<r32v4>>::Resolve() };
    Member<r32v4> mScale       { Registry<Sprite, Member<r32v4>>::Resolve() };
    Member<r32>   mTextureIndex{ Registry<Sprite, Member<r32>>::Resolve() };
  };

  Tuple<MVP> uniforms{};
  Tuple<Sprite> buffers{};

  auto const [vertexShader, fragmentShader]
  {
    ShaderFactory::Create(uniforms, buffers)
  };

  std::cout << vertexShader << std::endl;
  std::cout << fragmentShader << std::endl;

  return mpWindow->Run(mpRenderer);
}