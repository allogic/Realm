#pragma once

#include <Core.h>
#include <Types.h>

/*
* Glsl shader factory.
*/

template<typename T, StringLiteral Name>
struct Member
{
  using Type = T;

  constexpr static auto cName = Name.mValue;
};

template<typename T>
struct Registry
{
  static s8 const* Resolve()
  {
    return "test";
  }
};

template<typename ... T>
struct Tuple {};

struct ShaderFactory
{
  template<
    typename ... Uniforms, template <typename ...> typename UniformTuple,
    typename ... Buffers, template <typename ...> typename BufferTuple
  >
  static std::tuple<std::string, std::string> Create(
    UniformTuple<Uniforms ...> const& uniforms,
    BufferTuple<Buffers ...> const& buffers)
  {
    std::string vertexShader{};
    std::string fragmentShader{};

    std::cout << sizeof...(Uniforms) << std::endl;
    std::cout << sizeof...(Buffers) << std::endl;

    std::cout << (Registry::Resolve<typename Identity<Uniforms>::Type>(), ...) << std::endl;

    return { vertexShader, fragmentShader };
  }
};