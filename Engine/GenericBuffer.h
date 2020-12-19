#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>

/*
* Generic shader buffer.
*/

template<u32 U, typename T>
struct GenericBuffer : Component
{
  u32 mBufferId  {};
  u32 mBufferSize{};

  GenericBuffer(u32 bufferSize);
  virtual ~GenericBuffer();

  void Bind();
  void Map(u32 mountIndex);

  void Set(T* pBufferData);
  void Get(T* pBufferData);
};

template<typename T> using ComputeBuffer = GenericBuffer<GL_SHADER_STORAGE_BUFFER, T>;
template<typename T> using UniformBuffer = GenericBuffer<GL_UNIFORM_BUFFER, T>;
template<typename T> using VertexBuffer  = GenericBuffer<GL_ARRAY_BUFFER, T>;
template<typename T> using ElementBuffer = GenericBuffer<GL_ELEMENT_ARRAY_BUFFER, T>;

/*
* Generic shader buffer implementation.
*/

template<u32 U, typename T>
GenericBuffer<U, T>::GenericBuffer(u32 bufferSize)
  : mBufferSize{ bufferSize }
{
  glGenBuffers(1, &mBufferId);

  assert(mBufferId);

  glBindBuffer(U, mBufferId);
  glBufferStorage(U, mBufferSize * sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

  if constexpr (U == GL_ARRAY_BUFFER)
  {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(sizeof(r32v3)));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(sizeof(r32v3) + sizeof(r32v2)));
  }

  glBindBuffer(U, 0);
}
template<u32 U, typename T>
GenericBuffer<U, T>::~GenericBuffer()
{
  glDeleteBuffers(1, &mBufferId);
}

template<u32 U, typename T>
void GenericBuffer<U, T>::Bind()
{
  glBindBuffer(U, mBufferId);
}
template<u32 U, typename T>
void GenericBuffer<U, T>::Map(u32 mountIndex)
{
  static_assert(U != GL_ARRAY_BUFFER && U != GL_ELEMENT_ARRAY_BUFFER);

  glBindBufferBase(U, mountIndex, mBufferId);
}

template<u32 U, typename T>
void GenericBuffer<U, T>::Set(T* pBufferData)
{
  glBindBuffer(U, mBufferId);
  glBufferSubData(U, 0, mBufferSize * sizeof(T), pBufferData);
  glBindBuffer(U, 0);
}
template<u32 U, typename T>
void GenericBuffer<U, T>::Get(T* pBufferData)
{
  glBindBuffer(U, mBufferId);
  glGetBufferSubData(U, 0, mBufferSize * sizeof(T), pBufferData);
  glBindBuffer(U, 0);
}