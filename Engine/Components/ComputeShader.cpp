#include <Components/ComputeShader.h>

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

void ComputeShader::Execute(u32 numThreadX, u32 numThreadY, u32 numThreadZ)
{
  glDispatchCompute(numThreadX, numThreadY, numThreadZ);
}