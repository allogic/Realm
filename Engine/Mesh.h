#pragma once

#include <Core.h>
#include <Types.h>
#include <ACS.h>
#include <GenericBuffer.h>

/*
* Generic mesh.
*/

template<typename T>
struct Mesh : Component
{
  u32                 mVao {};
  VertexBuffer<T>*    mpVbo{};
  ElementBuffer<u32>* mpEbo{};

  Mesh(u32 vertexBufferSize, u32 elementBufferSize);
  virtual ~Mesh();

  void Bind();

  void Set(T* pVertexBuffer, u32* pElementBuffer);
  void Get(T* pVertexBuffer, u32* pElementBuffer);
};

/*
* Generic mesh implementation.
*/

template<typename T>
Mesh<T>::Mesh(u32 vertexBufferSize, u32 elementBufferSize)
{
  glGenVertexArrays(1, &mVao);

  glBindVertexArray(mVao);

  mpVbo = new VertexBuffer<T>{ vertexBufferSize };
  mpEbo = new ElementBuffer<u32>{ elementBufferSize };

  glBindVertexArray(0);
}
template<typename T>
Mesh<T>::~Mesh()
{
  delete mpVbo;
  delete mpEbo;

  glDeleteVertexArrays(1, &mVao);
}

template<typename T>
void Mesh<T>::Bind()
{
  glBindVertexArray(mVao);

  mpVbo->Bind();
  mpEbo->Bind();
}

template<typename T>
void Mesh<T>::Set(T* pVertexBuffer, u32* pElementBuffer)
{
  glBindVertexArray(mVao);

  mpVbo->Set(pVertexBuffer);
  mpEbo->Set(pElementBuffer);

  glBindVertexArray(0);
}
template<typename T>
void Mesh<T>::Get(T* pVertexBuffer, u32* pElementBuffer)
{
  glBindVertexArray(mVao);

  mpVbo->Get(pVertexBuffer);
  mpEbo->Get(pElementBuffer);

  glBindVertexArray(0);
}