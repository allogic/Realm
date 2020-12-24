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
    Member<r32m4, "projection"> mProjection{ ShaderFactory::Register<MVP>(this) };
    Member<r32m4, "view">       mView      { ShaderFactory::Register<MVP>(this) };
    Member<r32m4, "model">      mModel     { ShaderFactory::Register<MVP>(this) };
  };

  struct Sprite
  {
    Member<r32v4, "position">  mPosition    {};
    Member<r32v4, "rotation">  mRotation    {};
    Member<r32v4, "scale">     mScale       {};
    Member<r32,   "textureId"> mTextureIndex{};
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