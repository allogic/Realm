#pragma once

#include <Core.h>
#include <Types.h>
#include <Instance.h>
#include <ACS.h>
#include <Camera.h>
#include <Vertex.h>
#include <Mesh.h>
#include <GenericBuffer.h>
#include <RenderShader.h>
#include <Utility.h>
#include <Texture.h>
#include <TextureArray.h>

#include <Buffers/Transform.h>
#include <Uniforms/Projection.h>

/*
* Default renderer.
*/

struct Renderer
{
  u32                         mWidth           {};
  u32                         mHeight          {};
  GlProjection                mProjection      {};
  UniformBuffer<GlProjection> mProjectionBuffer{ 1 };

  Renderer(u32 width, u32 height);
  virtual ~Renderer();

  void PassGeometry();
  void PassLight();
  void PassGizmo();

  void Begin();
  void Render();
  void End();
};

/*
* Default renderer implementation.
*/

Renderer::Renderer(u32 width, u32 height)
  : mWidth{ width }
  , mHeight{ height }
{

}
Renderer::~Renderer()
{

}

void Renderer::PassGeometry()
{
  mProjectionBuffer.Map(0);

  ACS::DispatchFor<
    Transform,
    RenderShader,
    Mesh<VertexDefault>,
    ComputeBuffer<GlTransform>,
    TextureArray
  >([&](
    Transform* pTransform,
    RenderShader* pRenderShader,
    Mesh<VertexDefault>* pMesh,
    ComputeBuffer<GlTransform>* pComputeBuffer,
    TextureArray* pTextureArray)
    {
      mProjection.mTransform = TransformTo(pTransform->mPosition, pTransform->mRotation, pTransform->mScale);

      mProjectionBuffer.Set(&mProjection);

      pComputeBuffer->Map(0);

      pTextureArray->Map(0);

      pRenderShader->Bind();
      pMesh->Bind();

      glDrawElementsInstanced(GL_TRIANGLES, pMesh->mpEbo->mBufferSize, GL_UNSIGNED_INT, nullptr, pComputeBuffer->mBufferSize);
    });
}
void Renderer::PassLight()
{

}
void Renderer::PassGizmo()
{

}

void Renderer::Begin()
{
  // Update camera uniforms
  ACS::DispatchFor<Transform, Camera>([&](Transform* pTransform, Camera* pCamera)
    {
      r32m4 worldTransform{ pTransform->LocalToWorld() };
      r32m4 localTransform{ TransformTo(pTransform->mPosition, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f }) };

      r32 aspect{ (r32)mWidth / (r32)mHeight };

      glViewport(0, 0, mWidth, mHeight);

      mProjection.mProjection = glm::ortho(-pCamera->mZoom * aspect, pCamera->mZoom * aspect, -pCamera->mZoom, pCamera->mZoom);
      mProjection.mView = localTransform;

      mProjectionBuffer.Set(&mProjection);
    });
}
void Renderer::Render()
{
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Geometry pass
  PassGeometry();

  // Light pass
  PassLight();

  // Gizmo pass
  PassGizmo();
}
void Renderer::End()
{

}