#pragma once

#include <Core.h>
#include <Types.h>

#include <Components/Shader.h>

/*
* Render shader.
*/

struct RenderShader : Shader
{
  u32 mVertexId  {};
  u32 mFragmentId{};

  RenderShader(s8 const* pVertexSource, s8 const* pFragmentSource);
  virtual ~RenderShader();
};