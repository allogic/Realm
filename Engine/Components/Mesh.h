#pragma once

#include <Core.h>
#include <Types.h>
#include <Component.h>

#include <Components/GenericBuffer.h>

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
  void Unbind();
  void Clear();

  void Set(T* pVertexBuffer, u32* pElementBuffer);
  void Set(T* pVertexBuffer, u32* pElementBuffer, u32 vertexOffset, u32 elementOffset, u32 vertexSize, u32 elementSize);

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

  mpVbo->Bind();
  switch (T::sType)
  {
    case eVertexTypeDefault:
    {
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(0));
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(sizeof(r32v3)));
      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(sizeof(r32v3) + sizeof(r32v2)));
      break;
    }
    case eVertexTypeGizmo:
    {
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(0));
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(sizeof(r32v3)));
      break;
    }
    case eVertexTypeScreen:
    {
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(0));
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(sizeof(r32v3)));
      break;
    }
  }
  mpVbo->Unbind();

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
void Mesh<T>::Unbind()
{
  mpVbo->Unbind();
  mpEbo->Unbind();

  glBindVertexArray(0);
}
template<typename T>
void Mesh<T>::Clear()
{
  glBindVertexArray(mVao);

  mpVbo->Clear();
  mpEbo->Clear();

  glBindVertexArray(0);
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
void Mesh<T>::Set(T* pVertexBuffer, u32* pElementBuffer, u32 vertexOffset, u32 elementOffset, u32 vertexSize, u32 elementSize)
{
  glBindVertexArray(mVao);

  mpVbo->Set(pVertexBuffer, vertexOffset, vertexSize);
  mpEbo->Set(pElementBuffer, elementOffset, elementSize);

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