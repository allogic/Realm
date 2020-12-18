#pragma once

#include <Core.h>
#include <Types.h>

#include <Interface/Shader.h>

/*
* Render shader.
*/

struct RenderShader : Shader
{
  RenderShader(s8 const* pVertexSource, s8 const* pFragmentSource);
};

/*
* Compute shader.
*/

struct ComputeShader : Shader
{
  ComputeShader(s8 const* pComputeSource);
};

/*
* Render shader implementation.
*/

Shader::Shader(s8 const* pVertexSOurce, s8 const* pFragmentSource)
{

}
Shader::~Shader()
{
  glDeleteProgram(mPid);
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

    std::printf("Shader %s", log);

    return 0;
  }

  return 1;
}
u32 Shader::CheckLinkStatus()
{
  s32 linkStatus{};
  s32 infoLogLength{ 1024 };
  s8 log[1024]{};

  glGetProgramiv(mPid, GL_LINK_STATUS, &linkStatus);

  if (!linkStatus)
  {
    glGetProgramInfoLog(mPid, infoLogLength, &infoLogLength, log);

    std::printf("Shader %s", log);

    return 0;
  }

  return 1;
}
void Shader::Bind()
{
  glUseProgram(mPid);
}

/*
* Shader management.
*/

template<typename ShaderLayout> void ShaderLayoutDestroy(ShaderLayout const& shaderLayout)
{
  if (shaderLayout.mVertId) glDeleteShader(shaderLayout.mVertId);
  if (shaderLayout.mFragId) glDeleteShader(shaderLayout.mFragId);
  if (shaderLayout.mCompId) glDeleteShader(shaderLayout.mCompId);

  if (shaderLayout.mProgId) glDeleteProgram(shaderLayout.mProgId);
}
template<typename ShaderLayout> void ShaderLayoutCreate(ShaderLayout& shaderLayout, ShaderPaths const& shaderPaths)
{
  std::string log{};

  switch (ShaderLayout::sLayout)
  {
  case eShaderLayoutLambert:
  case eShaderLayoutLambertInstanced:
  case eShaderLayoutGizmo:
  case eShaderLayoutScreen:
  {
    shaderLayout.mVertId = glCreateShader(GL_VERTEX_SHADER);
    shaderLayout.mFragId = glCreateShader(GL_FRAGMENT_SHADER);
    break;
  }
  case eShaderLayoutCompute:
  {
    shaderLayout.mCompId = glCreateShader(GL_COMPUTE_SHADER);
    break;
  }
  }

  std::vector<s8> shaderBytes{};

  switch (ShaderLayout::sLayout)
  {
  case eShaderLayoutLambert:
  case eShaderLayoutLambertInstanced:
  case eShaderLayoutGizmo:
  case eShaderLayoutScreen:
  {
    ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mVertex);
    glShaderBinary(1, &shaderLayout.mVertId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
    glSpecializeShader(shaderLayout.mVertId, "main", 0, nullptr, nullptr);
    if (!ShaderLayoutCheckCompileStatus(shaderLayout.mVertId, log)) std::printf("%s\n", log.data());

    ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mFragment);
    glShaderBinary(1, &shaderLayout.mFragId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
    glSpecializeShader(shaderLayout.mFragId, "main", 0, nullptr, nullptr);
    if (!ShaderLayoutCheckCompileStatus(shaderLayout.mFragId, log)) std::printf("%s\n", log.data());
    break;
  }
  case eShaderLayoutCompute:
  {
    ShaderLayoutLoadBinary(shaderBytes, shaderPaths.mCompute);
    glShaderBinary(1, &shaderLayout.mCompId, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBytes.data(), (s32)shaderBytes.size());
    glSpecializeShader(shaderLayout.mCompId, "main", 0, nullptr, nullptr);
    if (!ShaderLayoutCheckCompileStatus(shaderLayout.mCompId, log)) std::printf("%s\n", log.data());
    break;
  }
  }

  shaderLayout.mProgId = glCreateProgram();

  switch (ShaderLayout::sLayout)
  {
  case eShaderLayoutLambert:
  case eShaderLayoutLambertInstanced:
  case eShaderLayoutGizmo:
  case eShaderLayoutScreen:
  {
    glAttachShader(shaderLayout.mProgId, shaderLayout.mVertId);
    glAttachShader(shaderLayout.mProgId, shaderLayout.mFragId);
    break;
  }
  case eShaderLayoutCompute:
  {
    glAttachShader(shaderLayout.mProgId, shaderLayout.mCompId);
    break;
  }
  }

  glLinkProgram(shaderLayout.mProgId);

  if (!ShaderLayoutCheckLinkStatus(shaderLayout.mProgId, log)) std::printf("%s\n", log.data());

  switch (ShaderLayout::sLayout)
  {
  case eShaderLayoutLambert:
  case eShaderLayoutLambertInstanced:
  case eShaderLayoutGizmo:
  case eShaderLayoutScreen:
  {
    glDetachShader(shaderLayout.mProgId, shaderLayout.mVertId);
    glDetachShader(shaderLayout.mProgId, shaderLayout.mFragId);
    break;
  }
  case eShaderLayoutCompute:
  {
    glDetachShader(shaderLayout.mProgId, shaderLayout.mCompId);
    break;
  }
  }
}