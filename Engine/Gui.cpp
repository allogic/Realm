#include <Gui.h>

/*
* Engine debug gui implementation.
*/

void Gui::Toolbar(Renderer* pRenderer)
{
  //ImGui::ShowDemoWindow();

  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("Renderer"))
    {
      if (ImGui::BeginMenu("FrameBuffers"))
      {
        ImGui::Image((void*)(std::intptr_t)pRenderer->mFrameBuffer.mTextureAlbedo.mTextureId, { 352, 240 }, { 0, 1 }, { 1, 0 });

        ImGui::EndMenu();
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}