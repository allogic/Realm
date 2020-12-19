#pragma once

#include <Components/Transform.h>

/*
* Forward decls.
*/

struct Object;
struct Window;
struct Renderer;

/*
* Actor base.
*/

struct Actor
{
  Window*             mpWindow   {};
  Renderer*           mpRenderer {};

  Actor*              mpParent   {};
  std::vector<Actor*> mChilds    {};

  Transform*          mpTransform{};

  s8 const*           mpName     {};
  Object*             mpObject   {};

  Actor(Object* pObject) : mpObject{ pObject } {}

  virtual void OnUpdate(r32 time, r32 timeDelta)      {}
  virtual void OnUpdateFixed(r32 time, r32 timeDelta) {}
  virtual void OnGizmo()                              {}
};