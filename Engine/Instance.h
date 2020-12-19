#pragma once

#include <Core.h>
#include <Types.h>

/*
* Type instance database.
*/

template<typename T>
struct Instance
{
  inline static std::map<s8 const*, T*> sInstanceDatabase{};

  template<typename ... Args>
  inline static T* Get(s8 const* key, Args&& ... args)
  {
    auto const instanceIt{ sInstanceDatabase.find(key) };

    if (instanceIt == sInstanceDatabase.end())
    {
      auto const [insertIt, _] { sInstanceDatabase.emplace(key, new T{ std::forward<Args>(args) ... }) };

      return insertIt->second;
    }

    return instanceIt->second;
  }
};