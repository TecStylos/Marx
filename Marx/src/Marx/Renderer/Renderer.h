#pragma once

#include "RenderCommand.h"

#include "Camera.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Marx/Platform/D3D11/D3D11Buffer.h"

namespace Marx
{

	class Renderer
	{
	public:
		static void beginScene(const OrthographicCamera& camera);
		static void endScene();
	public:
		static void submit(const Reference<Shader>& shader, const Reference<VertexArray>& vertexArray, const DX::XMMATRIX& transform, DX::XMFLOAT4 color);
	public:
		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	private:
		struct SceneData
		{
			DX::XMMATRIX viewProjectionMatrix;
		};
		static SceneData* s_sceneData;
	};
}