#pragma once

#include <Core.h>
#include <Types.h>

#include <Components/Shader.h>

/*
* Compute shader.
*/

struct ComputeShader : Shader
{
  u32 mComputeId{};

  ComputeShader(s8 const* pComputeSource);
  virtual ~ComputeShader();

  void Execute(u32 numThreadX, u32 numThreadY, u32 numThreadZ);
};