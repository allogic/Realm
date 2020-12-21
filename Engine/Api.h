#pragma once

#ifdef ENGINE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include <Core.h>
#include <Types.h>
#include <Instance.h>
#include <Window.h>
#include <ACS.h>
#include <Vertex.h>
#include <Renderer.h>
#include <Utility.h>

#include <Components/Camera.h>
#include <Components/ComputeShader.h>
#include <Components/GenericBuffer.h>
#include <Components/Mesh.h>
#include <Components/RenderShader.h>
#include <Components/Shader.h>
#include <Components/Texture.h>
#include <Components/TextureArray.h>
#include <Components/Transform.h>

#include <Shaders/Utilities.h>
#include <Shaders/Sprite.h>
#include <Shaders/Gizmo.h>

#include <Buffers/GlSprite.h>

#include <Uniforms/GlProjection.h>