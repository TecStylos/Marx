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
		s_sceneData->viewProjectionMatrix = camera.getViewProjectionTransposed();
	}

	void Renderer::endScene()
	{
	}

	void Renderer::submit(const Reference<Shader>& shader, const Reference<VertexArray>& vertexArray, const DX::XMMATRIX& transform, const Reference<Texture2D> texture)
	{
		static Reference<ConstantBuffer> sceneVSConstBuffer = VSConstantBuffer::create(sizeof(DX::XMMATRIX), nullptr);
		static Reference<ConstantBuffer> modelVSConstBuffer = VSConstantBuffer::create(sizeof(DX::XMMATRIX), nullptr);

		shader->bind();
		vertexArray->bind();
		texture->bind();

		sceneVSConstBuffer->update(&s_sceneData->viewProjectionMatrix);
		sceneVSConstBuffer->bind(0);
		modelVSConstBuffer->update(&transform);
		modelVSConstBuffer->bind(1);

		RenderCommand::drawIndexed(vertexArray);
	}
}