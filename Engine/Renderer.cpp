#include <Renderer.h>

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
  , mFrameBuffer{ 1280, 720 }
{
  std::vector<VertexScreen> vertices
  {
    { { -1.f, -1.f, 0.f }, { 0.f, 0.f } },
    { { -1.f,  1.f, 0.f }, { 0.f, 1.f } },
    { {  1.f, -1.f, 0.f }, { 1.f, 0.f } },
    { {  1.f,  1.f, 0.f }, { 1.f, 1.f } },
  };
  std::vector<u32> elements{ 0, 1, 2, 2, 3, 1 };

  mMeshScreen.Set(vertices.data(), elements.data());
}
Renderer::~Renderer()
{

}

void Renderer::PassLight()
{

}
void Renderer::PassPostProcess()
{
  mFrameBuffer.mTextureAlbedo.MapSampler(0);

  mShaderPostProcess.Bind();

  mMeshScreen.Bind();

  glDrawElements(GL_TRIANGLES, mMeshScreen.mpEbo->mBufferSize, GL_UNSIGNED_INT, nullptr);
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

  mShaderRenderGizmo.Bind();
  mMeshGizmo.Bind();

  glDrawElements(GL_LINES, mGizmoElementOffset, GL_UNSIGNED_INT, nullptr);
}
void Renderer::PassImGui()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();

  ImGui::NewFrame();

  Gui::Toolbar(this);

  ACS::Dispatch([=](Actor* pActor)
    {
      pActor->OnImGui();
    });

  ImGui::EndFrame();
  ImGui::Render();

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

  // Geometry pass layer 0
  mFrameBuffer.BindWrite();
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  PassGeometry<0>();
  glDisable(GL_BLEND);
  mFrameBuffer.UnbindWrite();

  // Post-Process pass
  mFrameBuffer.BindRead();
  PassPostProcess();
  mFrameBuffer.UnbindRead();

  // Gizmo pass
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  PassGizmo();
  glDisable(GL_BLEND);

  // ImGui pass
  PassImGui();
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