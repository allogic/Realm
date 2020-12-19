#pragma once

#include <Api.h>

#include <Components/Controller.h>

/*
* Player controller.
*/

struct Player : Actor
{
  Window*     mpWindow          { Instance<Window>::Get("Window") };

  Camera*     mpCamera          { ACS::Attach<Camera>(this) };
  Controller* mpController      { ACS::Attach<Controller>(this) };

  r32v2       mMousePositionDown{};

  Player(Object* pObject);
  virtual ~Player();

  void OnUpdate(r32 time, r32 timeDelta) override;
  void OnUpdateFixed(r32 time, r32 timeDelta) override;
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