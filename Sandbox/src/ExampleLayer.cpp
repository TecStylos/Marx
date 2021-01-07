#include "ExampleLayer.h"

//ExampleLayer::ExampleLayer()
//	: Layer("Example"), m_camera(-1.6f, 1.6f, -0.9f, 0.9f), m_position(0.0f, 0.0f, 0.0f)
//{
//	// Vertex Buffer
//	float vertices[] =
//	{
//		-0.5f, -0.5f, 0.0f, 1.0f,
//		-0.5f,  0.5f, 0.0f, 1.0f,
//		 0.5f,  0.5f, 0.0f, 1.0f,
//		 0.5f, -0.5f, 0.0f, 1.0f
//	};
//	Marx::Reference<Marx::VertexBuffer> pVertexBuffer(Marx::VertexBuffer::create(vertices, sizeof(vertices)));
//	// Vertex Layout
//	pVertexBuffer->setLayout({
//		{ Marx::ShaderDataType::Float4, "POSITION" }
//		});
//
//	// Index Buffer
//	uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
//	Marx::Reference<Marx::IndexBuffer> pIndexBuffer(
//		Marx::IndexBuffer::create(
//			indices,
//			sizeof(indices) / sizeof(uint32_t),
//			Marx::IndexBuffer::PrimitiveType::TriangleList
//		)
//	);
//
//	// Shader
//	m_pShader.reset(
//		Marx::Shader::create(
//			Marx::Shader::loadShaderSrcFromFile("C:\\dev\\Marx\\Marx\\src\\shaders\\flatColorVS.hlsl"),
//			Marx::Shader::loadShaderSrcFromFile("C:\\dev\\Marx\\Marx\\src\\shaders\\flatColorPS.hlsl")
//		)
//	);
//
//	// Vertex Array
//	m_pVertexArray.reset(Marx::VertexArray::create());
//	m_pVertexArray->setVertexBuffer(pVertexBuffer);
//	m_pVertexArray->setIndexBuffer(pIndexBuffer);
//}
//
//void ExampleLayer::onUpdate(Marx::Timestep ts)
//{
//	static constexpr float moveSpeed = 1.0f;
//	static constexpr float rotSpeed = DX::XMConvertToRadians(90.0f);
//	static float rotation = 0.0f;
//
//	if (Marx::Input::isKeyPressed(MX_KEY_Q))
//		rotation += rotSpeed * ts;
//	if (Marx::Input::isKeyPressed(MX_KEY_E))
//		rotation -= rotSpeed * ts;
//
//	if (Marx::Input::isKeyPressed(MX_KEY_W))
//		m_position.y += moveSpeed * ts;
//	if (Marx::Input::isKeyPressed(MX_KEY_S))
//		m_position.y -= moveSpeed * ts;
//	if (Marx::Input::isKeyPressed(MX_KEY_A))adad
//		m_position.x -= moveSpeed * ts;
//	if (Marx::Input::isKeyPressed(MX_KEY_D))
//		m_position.x += moveSpeed * ts;
//
//	if (Marx::Input::isKeyPressed(MX_KEY_LEFT))
//		m_vertexX -= 0.001f;
//	if (Marx::Input::isKeyPressed(MX_KEY_RIGHT))
//		m_vertexX += 0.001f;
//
//	DX::XMMATRIX scaleMat = DX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
//	DX::XMMATRIX rotationMat = DX::XMMatrixRotationZ(rotation);
//
//	Marx::RenderCommand::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//	Marx::RenderCommand::clear();
//
//	float vertex[] = { m_vertexX, -0.5f, 0.0f, 1.0f };
//	m_pVertexArray->getVertexBuffer()->updatePartial(&vertex, 0, 1);
//
//	Marx::Renderer::beginScene(m_camera);
//	for (int y = 0; y < 9; ++y)
//	{
//		for (int x = 0; x < 9; ++x)
//		{
//			DX::XMMATRIX transformMat = scaleMat * DX::XMMatrixTranslation(x * 0.11f, y * 0.11f, 0.0f) * rotationMat * DX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
//			Marx::Renderer::submit(m_pShader, m_pVertexArray, DX::XMMatrixTranspose(transformMat), m_color[(y * 9 + x) % 2]);
//		}
//	}
//	Marx::Renderer::endScene();
//}
//
//void ExampleLayer::onImGuiRender()
//{
//	ImGui::Begin("Color");
//	ImGui::ColorEdit3("Color1", &m_color[0].x);
//	ImGui::ColorEdit3("Color2", &m_color[1].x);
//	ImGui::End();
//}
//
//void ExampleLayer::onEvent(Marx::Event& event)
//{
//	Marx::EventDispatcher dispatcher(event);
//	dispatcher.dispatch<Marx::MouseMoveEvent>(MX_BIND_EVENT_METHOD(ExampleLayer::onMouseMove));
//	dispatcher.dispatch<Marx::MouseButtonPressEvent>(MX_BIND_EVENT_METHOD(ExampleLayer::onMouseButtonPress));
//}
//
//bool ExampleLayer::onMouseMove(Marx::MouseMoveEvent& e)
//{
//	m_mouseX = -(e.getX() / (Marx::Application::get()->getWindow()->getWidth() / 2) - 1.0f) * 1.6f;
//	m_mouseY = e.getY() / (Marx::Application::get()->getWindow()->getHeight() / 2) - 1.0f;
//	return true;
//}
//
//bool ExampleLayer::onMouseButtonPress(Marx::MouseButtonPressEvent& e)
//{
//	MX_TRACE("MouseButtonPress: {0}", e.getButton());
//	return true;
//}