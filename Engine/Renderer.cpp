#include <Renderer.h>
#include <ACS.h>

/*
* Default renderer implementation.
*/

Renderer::Renderer()
  : Renderer{ 1280, 720 }
{

}
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
    ComputeBuffer<GlSprite>,
    TextureArray
  >([&](
    Transform* pTransform,
    RenderShader* pRenderShader,
    Mesh<VertexDefault>* pMesh,
    ComputeBuffer<GlSprite>* pSpriteBuffer,
    TextureArray* pTextureArray)
    {
      mProjection.mTransform = TransformTo(pTransform->mPosition, pTransform->mRotation, pTransform->mScale);

      mProjectionBuffer.Set(&mProjection);

      pSpriteBuffer->Map(0);

      pTextureArray->Map(0);

      pRenderShader->Bind();
      pMesh->Bind();

      glDrawElementsInstanced(GL_TRIANGLES, pMesh->mpEbo->mBufferSize, GL_UNSIGNED_INT, nullptr, pSpriteBuffer->mBufferSize);
    });
}
void Renderer::PassLight()
{

}
void Renderer::PassGizmo()
{
  mMeshGizmo.Bind();
  ACS::Dispatch([&](Actor* pActor)
    {
      pActor->OnGizmo();
    });
  mMeshGizmo.Unbind();

  mProjectionBuffer.Map(0);
  mProjection.mTransform = TransformTo({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f });
  mProjectionBuffer.Set(&mProjection);

  mRenderGizmo.Bind();
  mMeshGizmo.Bind();

  glDrawElements(GL_LINES, mGizmoElementOffset, GL_UNSIGNED_INT, nullptr);
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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  PassGizmo();
  glDisable(GL_BLEND);
}
void Renderer::End()
{
  // Clear gizmo data
  mMeshGizmo.Clear();

  mGizmoVertexOffset = 0;
  mGizmoElementOffset = 0;
}

void Renderer::PushLine(r32v3 const& p0, r32v3 const& p1, r32v4 const& color)
{
  VertexGizmo vertices[2]{ { p0, color }, { p1, color } };
  u32 indices[2]{ mGizmoVertexOffset + 0, mGizmoVertexOffset + 1 };

  mMeshGizmo.Set(vertices, indices, mGizmoVertexOffset, mGizmoElementOffset, 2, 2);

  mGizmoVertexOffset += 2;
  mGizmoElementOffset += 2;
}
void Renderer::PushRect(r32v3 const& position, r32v3 const& size, r32v4 const& color)
{
  r32v3 half{ size / 2.f };

  VertexGizmo vertices[8]
  {
    { position + r32v3{ -half.x, -half.y, 0.f }, color },
    { position + r32v3{  half.x, -half.y, 0.f }, color },
    { position + r32v3{ -half.x, -half.y, 0.f }, color },
    { position + r32v3{ -half.x, +half.y, 0.f }, color },
    { position + r32v3{ -half.x, half.y, 0.f }, color },
    { position + r32v3{ half.x, half.y, 0.f }, color },
    { position + r32v3{ half.x, -half.y, 0.f }, color },
    { position + r32v3{ half.x, half.y, 0.f }, color },
  };
  u32 indices[8]
  {
    mGizmoVertexOffset + 0,
    mGizmoVertexOffset + 1,
    mGizmoVertexOffset + 2,
    mGizmoVertexOffset + 3,
    mGizmoVertexOffset + 4,
    mGizmoVertexOffset + 5,
    mGizmoVertexOffset + 6,
    mGizmoVertexOffset + 7,
  };

  mMeshGizmo.Set(vertices, indices, mGizmoVertexOffset, mGizmoElementOffset, 8, 8);

  mGizmoVertexOffset += 8;
  mGizmoElementOffset += 8;
}