#pragma once

#define ROOT_PATH "C:\\Users\\Michael\\Downloads\\Realm\\"

#include <array>
#include <vector>
#include <map>
#include <tuple>
#include <queue>
#include <variant>
#include <string>
#include <string_view>
#include <cstring>
#include <fstream>
#include <functional>
#include <cstdio>
#include <limits>
#include <algorithm>
#include <utility>
#include <cstdint>
#include <chrono>
#include <cassert>
#include <random>
#include <type_traits>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

/*
* Time debug utilities.
*/

#define _STR(VAL) #VAL
#define STR(VAL) _STR(VAL)

#define SANDBOX_ENGINE_MEASURE_BEGIN(NAME)                                                                     \
auto NAME##Start{ std::chrono::high_resolution_clock::now() };

#define SANDBOX_ENGINE_MEASURE_END(NAME)                                                                       \
auto NAME##End{ std::chrono::high_resolution_clock::now() };                                                   \
auto NAME##Duration{ std::chrono::duration_cast<std::chrono::milliseconds>(NAME##End - NAME##Start).count() }; \
std::printf(STR(NAME) ": %lldms\n", NAME##Duration);

/*
* Type deduction utilities.
*/

template<typename T>
struct Identity
{
  using Type = T;
  using Ptr = T*;
  using CPtr = T const*;
  using Ref = T&;
  using CRef = T const&;
};