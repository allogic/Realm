#include <Components/RenderShader.h>

/*
* Render shader implementation.
*/

RenderShader::RenderShader(s8 const* pVertexSource, s8 const* pFragmentSource)
  : Shader{}
{
  mVertexId = glCreateShader(GL_VERTEX_SHADER);
  mFragmentId = glCreateShader(GL_FRAGMENT_SHADER);

  assert(mVertexId);
  assert(mFragmentId);

  u32 status{};

  status |= CompileShader(mVertexId, pVertexSource);
  status |= CompileShader(mFragmentId, pFragmentSource);
  status |= LinkShader(mVertexId, mFragmentId);

  assert(!status);
}
RenderShader::~RenderShader()
{
  glDeleteShader(mVertexId);
  glDeleteShader(mFragmentId);
}