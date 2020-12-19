#pragma once

#include <Core.h>
#include <Types.h>
#include <Instance.h>
#include <Vertex.h>
#include <ShaderUtilities.h>
#include <Utility.h>

#include <Components/Camera.h>
#include <Components/Mesh.h>
#include <Components/GenericBuffer.h>
#include <Components/RenderShader.h>
#include <Components/Texture.h>
#include <Components/TextureArray.h>
#include <Components/Mesh.h>

#include <Shaders/Gizmo.h>

#include <Buffers/Sprite.h>
#include <Uniforms/Projection.h>

/*
* Default renderer.
*/

struct Renderer
{
  u32                         mWidth             {};
  u32                         mHeight            {};

  GlProjection                mProjection        {};
  UniformBuffer<GlProjection> mProjectionBuffer  { 1 };

  RenderShader                mRenderGizmo       { SHADER_RENDER_GIZMO_VERTEX, SHADER_RENDER_GIZMO_FRAGMENT };
  Mesh<VertexGizmo>           mMeshGizmo         { 65535, 131070 };

  u32                         mGizmoVertexOffset {};
  u32                         mGizmoElementOffset{};

  Renderer();
  Renderer(u32 width, u32 height);
  virtual ~Renderer();

  void PassGeometry();
  void PassLight();
  void PassGizmo();

  void Begin();
  void Render();
  void End();

  void PushLine(r32v3 const& p0, r32v3 const& p1, r32v4 const& color);
  void PushRect(r32v3 const& position, r32v3 const& size, r32v4 const& color);
};
