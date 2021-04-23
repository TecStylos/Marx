#pragma once

#include "RenderCommand.h"

#include "Camera.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"

#ifdef MX_PLATFORM_WINDOWS
	#include "Marx/Platform/D3D11/D3D11Buffer.h"
#endif

namespace Marx
{

	class Renderer
	{
	public:
		static void init();
	public:
		static void beginScene(const OrthographicCamera& camera);
		static void beginScene(const PerspectiveCamera& camera);
		static void endScene();
	public:
		static void submit(const Reference<Shader>& shader, const Reference<VertexArray>& vertexArray, glm::mat4 transform, const Reference<Texture2D> texture);
	public:
		inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};
		static SceneData* s_sceneData;
	};
}