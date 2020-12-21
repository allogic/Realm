#pragma once

#include <Core.h>
#include <Types.h>
#include <Actor.h>
#include <Component.h>

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

#ifdef ENGINE_IMPLEMENTATION
std::map<s8 const*, Object*> sObjectRegistry  {};
u64                          sUniqueTypeCount {};
std::map<u64, u64>           sIdentityRegistry{};
#else
extern std::map<s8 const*, Object*> sObjectRegistry;
extern u64                          sUniqueTypeCount;
extern std::map<u64, u64>           sIdentityRegistry;
#endif

namespace ACS
{
  /*
  * Helper utilities.
  */  

  template<typename C>
  requires std::is_base_of_v<Component, C>
  static u64 ComponentToIndex()
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
  static C* Attach(Actor* pActor, Args&& ... args)
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

      // Link dflt. components with actors
      // in order to not provide constructors for components
      pActor->mpObject->mComponents[componentIndex]->mpActor = pActor;

      return (C*)pActor->mpObject->mComponents[componentIndex];
    }
  }

  template<typename C>
  requires std::is_base_of_v<Component, C>
  static C* AttachShared(Actor* pActor, C* pComponent)
  {
    u64 const componentIndex{ ComponentToIndex<C>() };
    u64 const componentMask{ (u64)1 << componentIndex };

    if (ContainsComponentBits(pActor->mpObject->mMask, componentMask))
    {
      // Should never obtain registered component ptr since it's a shared reference
      assert(1);

      return nullptr;
    }
    else
    {
      pActor->mpObject->mMask |= componentMask;
      pActor->mpObject->mComponents[componentIndex] = pComponent;

      // Do not link shared components with actors

      return (C*)pActor->mpObject->mComponents[componentIndex];
    }
  }

  template<typename C>
  requires std::is_base_of_v<Component, C>
  static C* Find(Actor* pActor)
  {
    u64 const componentIndex{ ComponentToIndex<C>() };
    u64 const componentMask{ (u64)1 << componentIndex };

    if (ContainsComponentBits(pActor->mpObject->mMask, componentMask))
    {
      return (C*)pActor->mpObject->mComponents[componentIndex];
    }

    assert(1);

    return nullptr;
  }

  /*
  * Actor management.
  */

  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  static A* Create(s8 const* pName, Args&& ... args)
  {
    auto const objectIt{ sObjectRegistry.find(pName) };

    if (objectIt == sObjectRegistry.end())
    {
      Object* pObject{ new Object };

      auto const [insertIt, _] { sObjectRegistry.emplace(pName, pObject) };

      // Create actor at least in order to maintain sub-component order initialization 
      pObject->mpActor = new A{ pObject, std::forward<Args>(args) ... };
      pObject->mpActor->mpWindow = Instance<Window>::Get("Window");
      pObject->mpActor->mpRenderer = Instance<Renderer>::Get("Renderer");
      pObject->mpActor->mpName = pName;

      // Attach default components
      pObject->mpActor->mpTransform = Attach<Transform>(pObject->mpActor);

      return (A*)insertIt->second->mpActor;
    }

    return (A*)objectIt->second->mpActor;
  }

  template<typename A, typename ... Args>
  requires std::is_base_of_v<Actor, A>
  static A* CreateChild(Actor* pActor, s8 const* pName, Args&& ... args)
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
  static void Dispatch(std::function<void(Actor*)>&& predicate)
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
  static void DispatchFor(std::function<void(typename Identity<Comps>::Ptr ...)>&& predicate)
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