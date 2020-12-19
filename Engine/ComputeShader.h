#pragma once

#include <Core.h>
#include <Types.h>
#include <Shader.h>

/*
* Compute shader.
*/

struct ComputeShader : Shader
{
  u32 mComputeId{};

  ComputeShader(s8 const* pComputeSource);
  virtual ~ComputeShader();
};

/*
* Compute shader implementation.
*/

ComputeShader::ComputeShader(s8 const* pComputeSource)
  : Shader{}
{
  mComputeId = glCreateShader(GL_COMPUTE_SHADER);

  assert(mComputeId);

  u32 status{};

  status |= CompileShader(mComputeId, pComputeSource);
  status |= LinkShader(mComputeId);

  assert(!status);
}
ComputeShader::~ComputeShader()
{
  glDeleteShader(mComputeId);
}