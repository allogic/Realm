#pragma once

#include <Core.h>
#include <Types.h>

/*
* Global shader interface.
*/

struct Shader
{
  u32 mProgramId{};

  Shader();
  virtual ~Shader();

  void Bind();
  void Unbind();

  void CheckCompileStatus(u32 shaderId);
  void CheckLinkStatus();
};