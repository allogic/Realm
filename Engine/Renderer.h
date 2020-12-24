#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>
#include <Instance.h>
#include <Vertex.h>
#include <Utility.h>
#include <FrameBuffer.h>
#include <Gui.h>

#include <Components/Camera.h>
#include <Components/Mesh.h>
#include <Components/GenericBuffer.h>
#include <Components/RenderLayer.h>
#include <Components/RenderShader.h>
#include <Components/Texture.h>
#include <Components/TextureArray.h>
#include <Components/Mesh.h>

#include <Shaders/Gizmo.h>
#include <Shaders/Screen.h>

#include <Buffers/GlSprite.h>

#include <Uniforms/GlProjection.h>

/*
* Default renderer.
*/

struct Renderer
{
  u32                         mWidth             {};
  u32                         mHeight            {};

  FrameBuffer                 mFrameBuffer;

  GlProjection                mProjection        {};
  UniformBuffer<GlProjection> mProjectionBuffer  { 1 };

  RenderShader                mShaderRenderGizmo { SHADER_RENDER_GIZMO_VERTEX, SHADER_RENDER_GIZMO_FRAGMENT };
  Mesh<VertexGizmo>           mMeshGizmo         { 131070, 262140 };

  RenderShader                mShaderPostProcess { SHADER_RENDER_POST_PROCESS_VERTEX, SHADER_RENDER_POST_PROCESS_FRAGMENT };
  Mesh<VertexScreen>          mMeshScreen        { 4, 6 };

  u32                         mGizmoVertexOffset {};
  u32                         mGizmoElementOffset{};

  Renderer();
  Renderer(u32 width, u32 height);
  virtual ~Renderer();

  template<u32 Layer>
  void PassGeometry();

  void PassLight();
  void PassPostProcess();
  void PassGizmo();
  void PassImGui();

  void Begin();
  void Render();
  void End();

  void PushLine(r32v3 const& p0, r32v3 const& p1, r32v4 const& color);
  void PushRect(r32v3 const& position, r32v3 const& size, r32v4 const& color);
};

/*
* Default renderer implementation.
*/

template<u32 Layer>
void Renderer::PassGeometry()
{
  mProjectionBuffer.Map(0);

  ACS::DispatchFor<
    Transform,
    RenderLayer<Layer>,
    RenderShader,
    Mesh<VertexDefault>,
    ComputeBuffer<GlSprite>,
    TextureArray
  >([&](
    Transform* pTransform,
    RenderLayer<Layer>* pRenderLayer,
    RenderShader* pRenderShader,
    Mesh<VertexDefault>* pMesh,
    ComputeBuffer<GlSprite>* pSpriteBuffer,
    TextureArray* pTextureArray)
    {
      mProjection.mTransform = TransformTo(pTransform->mPosition, pTransform->mRotation, pTransform->mScale);

      mProjectionBuffer.Set(&mProjection);

      pSpriteBuffer->Map(0);

      pTextureArray->MapSampler(0);

      pRenderShader->Bind();
      pMesh->Bind();

      glDrawElementsInstanced(GL_TRIANGLES, pMesh->mpEbo->mBufferSize, GL_UNSIGNED_INT, nullptr, pSpriteBuffer->mBufferSize);
    });
}