#include "mxpch.h"
#include "Renderer.h"

namespace Marx
{
	Renderer::SceneData* Renderer::s_sceneData = new Renderer::SceneData;

	void Renderer::init()
	{
		RenderCommand::init();
	}

	void Renderer::beginScene(const OrthographicCamera& camera)
	{
		s_sceneData->viewProjectionMatrix = glm::transpose(camera.getViewProjectionMatrix());
	}

	void Renderer::beginScene(const PerspectiveCamera& camera)
	{
		s_sceneData->viewProjectionMatrix = glm::transpose(camera.getViewProjectionMatrix());
	}

	void Renderer::endScene()
	{
	}

	void Renderer::submit(const Reference<Shader>& shader, const Reference<VertexArray>& vertexArray, glm::mat4 transform, const Reference<Texture2D> texture)
	{
		//static Reference<ConstantBuffer> sceneVSConstBuffer = VSConstantBuffer::create(nullptr, sizeof(glm::mat4));
		//static Reference<ConstantBuffer> modelVSConstBuffer = VSConstantBuffer::create(nullptr, sizeof(glm::mat4));

		shader->bind();
		vertexArray->bind();
		texture->bind();

		transform = glm::transpose(transform);

		//sceneVSConstBuffer->update(&s_sceneData->viewProjectionMatrix);
		//sceneVSConstBuffer->bind(0);
		//modelVSConstBuffer->update(&transform);
		//modelVSConstBuffer->bind(1);

		RenderCommand::drawIndexed(vertexArray);
	}
}