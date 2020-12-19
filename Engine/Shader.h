#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

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
* Shader base implementation.
*/

Shader::Shader()
{
  mProgramId = glCreateProgram();

  assert(mProgramId);
}
Shader::~Shader()
{
  glDeleteProgram(mProgramId);
}

u32 Shader::CompileShader(u32 shaderId, s8 const* pShaderSource)
{
  glShaderSource(shaderId, 1, &pShaderSource, nullptr);
  glCompileShader(shaderId);

  return CheckCompileStatus(shaderId);
}

template<typename ... ShaderIds>
requires (std::is_same_v<u32, typename Identity<ShaderIds>::Type> && ...)
u32 Shader::LinkShader(ShaderIds ... shaderIds)
{
  (glAttachShader(mProgramId, shaderIds), ...);
  glLinkProgram(mProgramId);

  return CheckLinkStatus();
}

u32 Shader::CheckCompileStatus(u32 shaderId)
{
  s32 compileStatus{};
  s32 infoLogLength{ 1024 };
  s8 log[1024]{};

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

  if (!compileStatus)
  {
    glGetShaderInfoLog(shaderId, infoLogLength, &infoLogLength, log);

    std::printf("Shader %s\n", log);

    return 1;
  }

  return 0;
}
u32 Shader::CheckLinkStatus()
{
  s32 linkStatus{};
  s32 infoLogLength{ 1024 };
  s8 log[1024]{};

  glGetProgramiv(mProgramId, GL_LINK_STATUS, &linkStatus);

  if (!linkStatus)
  {
    glGetProgramInfoLog(mProgramId, infoLogLength, &infoLogLength, log);

    std::printf("Shader %s\n", log);

    return 1;
  }

  return 0;
}
void Shader::Bind()
{
  glUseProgram(mProgramId);
}