#include "mxpch.h"
#include "RenderCommand.h"

#include "Marx/Platform/D3D11/D3D11RendererAPI.h"

namespace Marx
{
	Scope<RendererAPI> RenderCommand::s_rendererAPI = std::make_unique<D3D11RendererAPI>();
}