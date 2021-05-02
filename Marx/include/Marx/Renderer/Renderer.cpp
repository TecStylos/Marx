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

	void Renderer::onWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::setViewport(0, 0, width, height);
	}

	void Renderer::submit(const Reference<Shader>& shader, const Reference<VertexArray>& vertexArray, glm::mat4 transform, const Reference<Texture2D> texture)
	{
		transform = glm::transpose(transform);
		shader->updateUniform("c_viewProjection", &s_sceneData->viewProjectionMatrix, Marx::ShaderDataType::Mat4);
		shader->updateUniform("c_modelTransform", &transform, Marx::ShaderDataType::Mat4);
		shader->bind();
		vertexArray->bind();
		texture->bind();

		RenderCommand::drawIndexed(vertexArray);
	}
}