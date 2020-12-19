#pragma once

#include <Core.h>
#include <Types.h>
#include <Utility.h>

/*
* Forward decls.
*/

struct Actor;

/*
* Default components.
*/

struct Component
{
  Actor* mpActor{};
};
struct Transform : Component
{
  r32v3 mPosition{};
  r32v3 mRotation{};
  r32v3 mScale   { 1.f, 1.f, 1.f };

  r32m4 LocalToWorld();
};

/*
* Actor component system.
*/

struct Object
{
  Actor*                     mpActor     {};
  u64                        mMask       {};
  std::array<Component*, 64> mComponents {};
  u32                        mDestroyFlag{};
};
struct Actor
{
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

static std::map<s8 const*, Object*> sObjectRegistry  {};
static u64                          sUniqueTypeCount {};
static std::map<u64, u64>           sIdentityRegistry{};

namespace ACS
{
  /*
  * Helper utilities.
  */

  template<typename C>
  requires std::is_base_of_v<Component, C>
  u64 ComponentToIndex()
  {
    auto const identityIt{ sIdentityRegistry.find(typeid(C).hash_code()) };

    if (identityIt == sIdentityRegistry.end())
    {
      auto const [insertIt, _] { sIdentityRegistry.emplace(typeid(C).hash_code(), sUniqueTypeCount++) };
      return insertIt->second;
    }

    return identityIt->second;
  }

  constexpr static u32 ContainsComponentBits(s64 entityMask, s64 componentMask)
  {
    return (~(entityMask ^ componentMask) & componentMask) == componentMask;
  }

  /*
  * Component management.
  */

  template<typename C, typename ... Args>
  requires std::is_base_of_v<Component, C>
  C* Attach(Actor* pActor, Args&& ... args)
  {
    u64 const componentIndex{ ComponentToIndex<C>() };
    u64 const componentMask{ (u64)1 << componentIndex };

    if (ContainsComponentBits(pActor->mpObject->mMask, componentMask))
    {
      return (C*)pActor->mpObject->mComponents[componentIndex];
    }
    else
    {
      pActor->mpObject->mMask |= componentMask;
      pActor->mpObject->mComponents[componentIndex] = new C{ std::forward<Args>(args) ... };

      // link dflt. components with actors
      // in order to not provide constructors for components
      pActor->mpObject->mComponents[componentIndex]->mpActor = pActor;

      return (C*)pActor->mpObject->mComponents[componentIndex];
    }
  }

  template<typename C>
  requires std::is_base_of_v<Component, C>
  C* Find(Actor* pActor)
  {
    u64 const componentIndex{ ComponentToIndex<C>() };
    u64 const componentMask{ (u64)1 << componentIndex };

    if (ContainsComponentBits(pActor->mpObject->mMask, componentMask))
    {
      return (C*)pActor->mpObject->mComponents[componentIndex];
    }

    assert(true);

    return nullptr;
  }

  /*
  * Actor management.
  */

  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  A* Create(s8 const* pName, Args&& ... args)
  {
    auto const objectIt{ sObjectRegistry.find(pName) };

    if (objectIt == sObjectRegistry.end())
    {
      Object* pObject{ new Object };

      auto const [insertIt, _] { sObjectRegistry.emplace(pName, pObject) };

      // create actor at least in order to maintain sub-component order initialization 
      pObject->mpActor = new A{ pObject, std::forward<Args>(args) ... };
      pObject->mpActor->mpName = pName;

      // attach default components
      pObject->mpActor->mpTransform = Attach<Transform>(pObject->mpActor);

      return (A*)insertIt->second->mpActor;
    }

    return (A*)objectIt->second->mpActor;
  }

  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  A* CreateChild(Actor* pActor, s8 const* pName, Args&& ... args)
  {
    A* pChild{ Create<A>(pName, std::forward<Args>(args) ...) };
    pChild->mpParent = pActor;

    pActor->mChilds.emplace_back(pChild);

    return pChild;
  }

  static void UngroupRecursive(Actor* pActor)
  {
    for (auto const& pChild : pActor->mChilds)
    {
      UngroupRecursive(pChild);
    }

    pActor->mpParent = nullptr;
    pActor->mChilds.clear();
  }
  static void Ungroup(Actor* pActor)
  {
    for (auto const& pChild : pActor->mChilds)
    {
      pChild->mpParent = nullptr;
    }

    pActor->mChilds.clear();
  }

  static void DestroyRecursive(Actor* pActor)
  {
    for (auto const& pChild : pActor->mChilds)
    {
      DestroyRecursive(pChild);
    }

    Ungroup(pActor);

    pActor->mpObject->mDestroyFlag = 1;
  }
  static void Destroy(Actor* pActor)
  {
    Ungroup(pActor);

    pActor->mpObject->mDestroyFlag = 1;
  }

  /*
  * Object dispatching.
  */

  template<typename ... Comps>
  requires (std::is_base_of_v<Component, typename Identity<Comps>::Type> && ...)
  void Dispatch(std::function<void(Actor*)>&& predicate)
  {
    for (auto const& [actorName, pObject] : sObjectRegistry)
    {
      if (!pObject->mDestroyFlag)
      {
        predicate(pObject->mpActor);
      }
    }
  }

  template<typename ... Comps>
  requires (std::is_base_of_v<Component, typename Identity<Comps>::Type> && ...)
  void DispatchFor(std::function<void(typename Identity<Comps>::Ptr ...)>&& predicate)
  {
    u64 const componentMask{ (((u64)1 << ComponentToIndex<typename Identity<Comps>::Type>()) | ... | (s64)0) };

    for (auto const& [actorName, pObject] : sObjectRegistry)
    {
      if (!pObject->mDestroyFlag)
      {
        if (ContainsComponentBits(pObject->mMask, componentMask))
        {
          predicate
          (
            (typename Identity<Comps>::Ptr)(pObject->mComponents[ComponentToIndex<typename Identity<Comps>::Type>()])
            ...
          );
        }
      }
    }
  }

  /*
  * Internal state management.
  */

  static void Update()
  {
    for (auto objectIt{ sObjectRegistry.begin() }; objectIt != sObjectRegistry.end();)
    {
      if (objectIt->second->mDestroyFlag)
      {
        for (u32 i{}; i < 64; i++)
        {
          if (ContainsComponentBits(objectIt->second->mMask, (s64)1 << i))
          {
            delete objectIt->second->mComponents[i];
            objectIt->second->mComponents[i] = nullptr;
          }
        }

        delete objectIt->second->mpActor;

        objectIt = sObjectRegistry.erase(objectIt);
      }
      else
      {
        objectIt++;
      }
    }
  }
}

/*
* Component implementation.
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