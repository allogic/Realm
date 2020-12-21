#pragma once

#include <Api.h>

#include <Components/Controller.h>

#include <WorldManager.h>

/*
* Player controller.
*/

struct Player : Actor
{
  WorldManager* mpWorldManager    { ACS::Create<WorldManager>("WorldManager") };

  Camera*       mpCamera          { ACS::Attach<Camera>(this) };
  Controller*   mpController      { ACS::Attach<Controller>(this) };

  r32v2         mTilePosition     {};
  r32v2         mTileSize         {};

  r32v2         mMousePositionDown{};

  Player(Object* pObject);
  virtual ~Player();

  void OnUpdate(r32 time, r32 timeDelta) override;
  void OnUpdateFixed(r32 time, r32 timeDelta) override;
  void OnGizmo() override;
};

/*
* Player controller implementation.
*/

Player::Player(Object* pObject)
  : Actor{ pObject }
{

}
Player::~Player()
{

}

void Player::OnUpdate(r32 time, r32 timeDelta)
{
  // Mouse movement
  if (mpWindow->MouseDown(GLFW_MOUSE_BUTTON_RIGHT))
  {
    mMousePositionDown = mpWindow->MousePosition();
  }
  if (mpWindow->MouseHeld(GLFW_MOUSE_BUTTON_RIGHT))
  {
    r32v2 mousePosition{ mpWindow->MousePosition() };
    r32v2 mousePositionDelta{ mMousePositionDown - mousePosition };
    r32v3 mousePositionDeltaAligned{ -mousePositionDelta.x, mousePositionDelta.y, 0.f };

    if (glm::length(mousePositionDeltaAligned) > glm::epsilon<r32>())
      mpController->mPositionAccel += glm::normalize(mousePositionDeltaAligned) * mpController->mRotationSpeed * timeDelta;
  }

  // Mouse scoll zoom
  mpController->mZoomAccel += -mpWindow->MouseScroll().y * mpController->mZoomSpeed * timeDelta;

  // Partially decrement acceleration over time by itself
  mpController->mPositionAccel += -mpController->mPositionAccel * mpController->mPositionDecay * timeDelta;
  mpController->mRotationAccel += -mpController->mRotationAccel * mpController->mRotationDecay * timeDelta;
  mpController->mZoomAccel += -mpController->mZoomAccel * mpController->mZoomDecay * timeDelta;

  // Screen coords to world
  r32v2 mouseScreen
  {
    2.f * ((mpWindow->MousePosition().x - mpWindow->WindowSize().x / 2.f) / mpWindow->WindowSize().x) * mpCamera->mZoom * mpWindow->AspectRatio(),
    -2.f * ((mpWindow->MousePosition().y - mpWindow->WindowSize().y / 2.f) / mpWindow->WindowSize().y) * mpCamera->mZoom,
  };

  mTileSize = { 1.f, 1.f };
  mTilePosition = { glm::floor(r32v2{ -mpTransform->mPosition } + mouseScreen) };

  // Tile placement
  if (mpWindow->MouseHeld(GLFW_MOUSE_BUTTON_LEFT))
  {
    mpWorldManager->SetSprite(mTilePosition, 11);
  }
}
void Player::OnUpdateFixed(r32 time, r32 timeDelta)
{
  // Handle movement
  mpTransform->mPosition += mpController->mPositionAccel;
  mpTransform->mPosition = glm::clamp(mpTransform->mPosition, { -100.f, -100.f, 0.f }, { 100.f, 100.f, 0.f });

  // Handle zoom
  mpCamera->mZoom += mpController->mZoomAccel;
  mpCamera->mZoom = glm::clamp(mpCamera->mZoom, 1.f, 1000.f);
}
void Player::OnGizmo()
{
  mpRenderer->PushLine({ 0.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, { 1.f, 0.f, 0.f, 1.f });
  mpRenderer->PushLine({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, { 0.f, 1.f, 0.f, 1.f });

  mpRenderer->PushRect(r32v3{ mTilePosition + mTileSize / 2.f, 0.f }, r32v3{ mTileSize, 0.f }, { 1.f, 1.f, 1.f, 1.f });
}