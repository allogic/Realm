#include <Components/Shader.h>

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

void Shader::Bind()
{
  glUseProgram(mProgramId);
}

u32 Shader::CompileShader(u32 shaderId, s8 const* pShaderSource)
{
  glShaderSource(shaderId, 1, &pShaderSource, nullptr);
  glCompileShader(shaderId);

  return CheckCompileStatus(shaderId);
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

void Shader::SetS32(s8 const* pName, s32 value)
{
  glUniform1i(
    glGetUniformLocation(mProgramId, pName),
    value
  );
}
void Shader::SetU32(s8 const* pName, u32 value)
{
  glUniform1ui(
    glGetUniformLocation(mProgramId, pName),
    value
  );
}