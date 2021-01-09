#include <Marx.h>

struct Vertex
{
	float x, y, z;
	float u, v;
};

class ExampleLayer : public Marx::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_camera(-1.6f, 1.6f, -0.9f, 0.9f), m_position(0.0f, 0.0f, 0.0f)
	{
		// Vertex Buffer
		Vertex vertices[] =
		{
			{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f },
			{ -0.5f,  0.5f, 0.0f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f, 0.0f }
		};
		Marx::Reference<Marx::VertexBuffer> pVertexBuffer(Marx::VertexBuffer::create(vertices, sizeof(vertices)));
		// Vertex Layout
		pVertexBuffer->setLayout({
			{ Marx::ShaderDataType::Float3, "A_POSITION" },
			{ Marx::ShaderDataType::Float2, "A_TEXCOORD" }
		});

		// Index Buffer
		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		Marx::Reference<Marx::IndexBuffer> pIndexBuffer(
			Marx::IndexBuffer::create(
				indices,
				sizeof(indices) / sizeof(uint32_t),
				Marx::IndexBuffer::PrimitiveType::TriangleList
			)
		);

		// Shader
		m_pShader = Marx::Shader::create(
			Marx::Shader::loadShaderSrcFromFile("C:\\dev\\Marx\\Marx\\src\\shaders\\textureVS.hlsl"),
			Marx::Shader::loadShaderSrcFromFile("C:\\dev\\Marx\\Marx\\src\\shaders\\texturePS.hlsl")
		);

		// Vertex Array
		m_pVertexArray = Marx::VertexArray::create();
		m_pVertexArray->setVertexBuffer(pVertexBuffer);
		m_pVertexArray->setIndexBuffer(pIndexBuffer);

		m_pTexture = Marx::Texture2D::create("C:\\dev\\Marx\\Sandbox\\assets\\textures\\testLow.png");
		m_pAlphaTexture = Marx::Texture2D::create("C:\\dev\\Marx\\Sandbox\\assets\\textures\\testAlpha.png");

		Marx::RenderCommand::enableBlending(true);
	}
public:
	virtual void onUpdate(Marx::Timestep ts) override
	{
		static constexpr float moveSpeed = 1.0f;
		static constexpr float rotSpeed = DX::XMConvertToRadians(90.0f);
		static float rotation = 0.0f;

		if (Marx::Input::isKeyPressed(MX_KEY_Q))
			rotation += rotSpeed * ts;
		if (Marx::Input::isKeyPressed(MX_KEY_E))
			rotation -= rotSpeed * ts;

		if (Marx::Input::isKeyPressed(MX_KEY_W))
			m_position.y += moveSpeed * ts;
		if (Marx::Input::isKeyPressed(MX_KEY_S))
			m_position.y -= moveSpeed * ts;
		if (Marx::Input::isKeyPressed(MX_KEY_A))
			m_position.x -= moveSpeed * ts;
		if (Marx::Input::isKeyPressed(MX_KEY_D))
			m_position.x += moveSpeed * ts;

		if (Marx::Input::isKeyPressed(MX_KEY_LEFT))
			m_vertexX -= 0.001f;
		if (Marx::Input::isKeyPressed(MX_KEY_RIGHT))
			m_vertexX += 0.001f;
		if (Marx::Input::isKeyPressed(MX_KEY_DOWN))
			m_vertexY -= 0.001f;
		if (Marx::Input::isKeyPressed(MX_KEY_UP))
			m_vertexY += 0.001f;

		DX::XMMATRIX scaleMat = DX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
		DX::XMMATRIX rotationMat = DX::XMMatrixRotationZ(rotation);

		Marx::RenderCommand::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		Marx::RenderCommand::clear();

		Vertex vertex = { m_vertexX, m_vertexY, 0.0f, 0.0f, 0.0f };
		Vertex* pVertex = &vertex;
		uint32_t offset = 0;
		uint32_t count = 1;
		m_pVertexArray->getVertexBuffer()->updatePartial((void**)&pVertex, &offset, &count, 1);

		Marx::Renderer::beginScene(m_camera);
		for (int y = 0; y < 9; ++y)
		{
			for (int x = 0; x < 9; ++x)
			{
				DX::XMMATRIX transformMat = scaleMat * DX::XMMatrixTranslation(x * 0.11f, y * 0.11f, 0.0f) * rotationMat * DX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
				Marx::Renderer::submit(m_pShader, m_pVertexArray, DX::XMMatrixTranspose(transformMat), m_pTexture);
			}
		}
		Marx::Renderer::submit(m_pShader, m_pVertexArray, DX::XMMatrixTranspose(DX::XMMatrixScaling(1.5f, 1.5f, 1.5f)), m_pTexture);
		Marx::Renderer::submit(m_pShader, m_pVertexArray, DX::XMMatrixTranspose(DX::XMMatrixScaling(1.5f, 1.5f, 1.5f)), m_pAlphaTexture);

		Marx::Renderer::endScene();
	}
	virtual void onImGuiRender() override
	{
		ImGui::Begin("Color");
		ImGui::ColorEdit3("Color1", &m_color[0].x);
		ImGui::ColorEdit3("Color2", &m_color[1].x);
		ImGui::End();
	}
	virtual void onEvent(Marx::Event& event) override
	{
		Marx::EventDispatcher dispatcher(event);
		dispatcher.dispatch<Marx::MouseMoveEvent>(MX_BIND_EVENT_METHOD(ExampleLayer::onMouseMove));
		dispatcher.dispatch<Marx::MouseButtonPressEvent>(MX_BIND_EVENT_METHOD(ExampleLayer::onMouseButtonPress));
	}
	bool onMouseMove(Marx::MouseMoveEvent& e)
	{
		m_mouseX = -(e.getX() / (Marx::Application::get()->getWindow()->getWidth() / 2) - 1.0f) * 1.6f;
		m_mouseY = e.getY() / (Marx::Application::get()->getWindow()->getHeight() / 2) - 1.0f;
		return true;
	}
	bool onMouseButtonPress(Marx::MouseButtonPressEvent& e)
	{
		MX_TRACE("MouseButtonPress: {0}", e.getButton());
		return true;
	}
private:
	Marx::Reference<Marx::Shader> m_pShader;
	Marx::Reference<Marx::VertexArray> m_pVertexArray;
	Marx::Reference<Marx::Texture2D> m_pTexture, m_pAlphaTexture;
	DX::XMFLOAT3 m_position;
	Marx::OrthographicCamera m_camera;
	float m_mouseX = 0.0f;
	float m_mouseY = 0.0f;
	DX::XMFLOAT4 m_color[2] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
	float m_vertexX = -0.5f;
	float m_vertexY = -0.5f;
};

class Sandbox : public Marx::Application
{
public:
	Sandbox()
	{
		pushLayer(new ExampleLayer());
	}
	~Sandbox()
	{}
};

Marx::Application* Marx::createApplication()
{
	return new Sandbox();
}