#pragma once

// For use by Marx applications

#include "Marx/Application.h"
#include "Marx/Layer.h"

// Core
#include "Marx/Core/Log.h"
#include "Marx/Core/Profiler.h"
#include "Marx/Core/Timestep.h"

// Input
#include "Marx/Input/MouseCodes.h"
#include "Marx/Input/KeyCodes.h"
#include "Marx/Input.h"
#include "Marx/Input/ControllerManager.h"

// ImGui
#include "imgui/imgui.h"

#include "Marx/ImGui/ImGuiLayer.h"

// Renderer
#include "Marx/Renderer/Buffer.h"
#include "Marx/Renderer/Camera.h"
#include "Marx/Renderer/GraphicsContext.h"
#include "Marx/Renderer/RenderCommand.h"
#include "Marx/Renderer/Renderer.h"
#include "Marx/Renderer/RendererAPI.h"
#include "Marx/Renderer/Shader.h"
#include "Marx/Renderer/VertexArray.h"
#include "Marx/Renderer/Texture.h"

// --- Entry Point ---
#include "Marx/EntryPoint.h"

#include "mxpch.h"