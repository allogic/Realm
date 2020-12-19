#include <Actor.h>

#include <Components/Transform.h>

/*
* Transform implementation.
*/

r32m4 Transform::LocalToWorld()
{
  r32m4 transform{ glm::identity<r32m4>() };

  Actor* pNextParent{ mpActor->mpParent };

  while (pNextParent)
  {
    transform *= TransformTo(pNextParent->mpTransform->mPosition, pNextParent->mpTransform->mRotation, pNextParent->mpTransform->mScale);
    pNextParent = pNextParent->mpParent;
  }

  return transform * TransformTo(mPosition, mRotation, mScale);
}