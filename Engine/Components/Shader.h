#pragma once

#include <Core.h>
#include <Types.h>
#include <Component.h>

/*
* Shader base.
*/

struct Shader : Component
{
  u32 mProgramId{};

  Shader();
  virtual ~Shader();

  void Bind();

  u32 CompileShader(u32 shaderId, s8 const* pShaderSource);

  template<typename ... ShaderIds>
  requires (std::is_same_v<u32, typename Identity<ShaderIds>::Type> && ...)
  u32 LinkShader(ShaderIds ... shaderIds);

  u32 CheckCompileStatus(u32 shaderId);
  u32 CheckLinkStatus();
};

/*
* Inline shader base implementation.
*/

template<typename ... ShaderIds>
requires (std::is_same_v<u32, typename Identity<ShaderIds>::Type> && ...)
u32 Shader::LinkShader(ShaderIds ... shaderIds)
{
  (glAttachShader(mProgramId, shaderIds), ...);
  glLinkProgram(mProgramId);

  return CheckLinkStatus();
}