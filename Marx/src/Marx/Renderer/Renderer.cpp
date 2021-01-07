#include "mxpch.h"
#include "Renderer.h"

namespace Marx
{
	Renderer::SceneData* Renderer::s_sceneData = new Renderer::SceneData;

	void Renderer::beginScene(const OrthographicCamera& camera)
	{
		s_sceneData->viewProjectionMatrix = camera.getViewProjectionTransposed();
	}

	void Renderer::endScene()
	{
	}

	void Renderer::submit(const Reference<Shader>& shader, const Reference<VertexArray>& vertexArray, const DX::XMMATRIX& transform, DX::XMFLOAT4 color)
	{
		static ConstantBuffer* sceneVSConstBuffer = VSConstantBuffer::create(0, sizeof(DX::XMMATRIX), nullptr);
		static ConstantBuffer* modelVSConstBuffer = VSConstantBuffer::create(1, sizeof(DX::XMMATRIX), nullptr);
		static ConstantBuffer* modelPSConstBuffer = PSConstantBuffer::create(0, sizeof(DX::XMFLOAT4), nullptr);

		shader->bind();
		vertexArray->bind();

		sceneVSConstBuffer->update(&s_sceneData->viewProjectionMatrix);
		sceneVSConstBuffer->bind();
		modelVSConstBuffer->update(&transform);
		modelVSConstBuffer->bind();
		modelPSConstBuffer->update(&color);
		modelPSConstBuffer->bind();

		RenderCommand::drawIndexed(vertexArray);
	}
}