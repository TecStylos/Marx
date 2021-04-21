#include <Marx.h>
#include <MarC.h>

struct Vertex
{
	float x, y, z;
	float u, v;
};

class ExampleLayer : public Marx::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_orthographicCam(-1.6f, 1.6f, -0.9f, 0.9f), m_position(0.0f, 0.0f, 0.0f)
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





		Vertex cubeVertices[] =
		{
			// Front
			{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f },
			{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, -0.5f, 1.0f, 1.0f },
			{  0.5f, -0.5f, -0.5f, 1.0f, 0.0f },

			// Back
			{  0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f, 1.0f, 1.0f },
			{ -0.5f, -0.5f,  0.5f, 1.0f, 0.0f },

			// Top
			{ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f },
			{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f,  0.5f, 1.0f, 1.0f },
			{  0.5f,  0.5f, -0.5f, 1.0f, 0.0f },

			// Bottom
			{ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
			{ -0.5f, -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, -0.5f, 1.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f, 1.0f, 0.0f },

			// Left
			{ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f },
			{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, -0.5f, 1.0f, 1.0f },
			{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f },

			// Right
			{  0.5f, -0.5f, -0.5f, 0.0f, 0.0f },
			{  0.5f,  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f,  0.5f, 1.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f, 1.0f, 0.0f },
		};
		Marx::Reference<Marx::VertexBuffer> pCubeVertexBuffer(Marx::VertexBuffer::create(cubeVertices, sizeof(cubeVertices)));

		pCubeVertexBuffer->setLayout({
			{ Marx::ShaderDataType::Float3, "A_POSITION" },
			{ Marx::ShaderDataType::Float2, "A_TEXCOORD" }
		});

		uint32_t cubeIndices[] = {
			4 * 0 + 0, 4 * 0 + 1, 4 * 0 + 2, 4 * 0 + 2, 4 * 0 + 3, 4 * 0 + 0,
			4 * 1 + 0, 4 * 1 + 1, 4 * 1 + 2, 4 * 1 + 2, 4 * 1 + 3, 4 * 1 + 0,
			4 * 2 + 0, 4 * 2 + 1, 4 * 2 + 2, 4 * 2 + 2, 4 * 2 + 3, 4 * 2 + 0,
			4 * 3 + 0, 4 * 3 + 1, 4 * 3 + 2, 4 * 3 + 2, 4 * 3 + 3, 4 * 3 + 0,
			4 * 4 + 0, 4 * 4 + 1, 4 * 4 + 2, 4 * 4 + 2, 4 * 4 + 3, 4 * 4 + 0,
			4 * 5 + 0, 4 * 5 + 1, 4 * 5 + 2, 4 * 5 + 2, 4 * 5 + 3, 4 * 5 + 0,
		};
		Marx::Reference<Marx::IndexBuffer> pCubeIndexBuffer(
			Marx::IndexBuffer::create(
				cubeIndices,
				sizeof(cubeIndices) / sizeof(uint32_t),
				Marx::IndexBuffer::PrimitiveType::TriangleList
			)
		);

		m_pCubeVertexArray = Marx::VertexArray::create(pCubeVertexBuffer, pCubeIndexBuffer);





		// Vertex Array
		m_pVertexArray = Marx::VertexArray::create(pVertexBuffer, pIndexBuffer);

		// Shader
		m_shaderLib.load("assets/shaders/Texture.hlsl");

		m_pTexture = Marx::Texture2D::create("assets\\textures\\testLow.png");
		m_pAlphaTexture = Marx::Texture2D::create("assets\\textures\\testAlpha.png");

		Marx::RenderCommand::enableBlending(true);

		m_perspectiveCam.setProperties(90.0f, 16.0f / 9.0f, 0.001f, 1000.0f);
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

		DX::XMFLOAT3 camPos = m_orthographicCam.getPosition();
		if (Marx::Input::isKeyPressed(MX_KEY_I))
			camPos.y += moveSpeed * ts;
		if (Marx::Input::isKeyPressed(MX_KEY_K))
			camPos.y -= moveSpeed * ts;
		if (Marx::Input::isKeyPressed(MX_KEY_J))
			camPos.x -= moveSpeed * ts;
		if (Marx::Input::isKeyPressed(MX_KEY_L))
			camPos.x += moveSpeed * ts;
		camPos.z = -1.0f;
		m_orthographicCam.setPosition(camPos);
		m_perspectiveCam.setPosition(camPos);

		DX::XMMATRIX scaleMat = DX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
		DX::XMMATRIX rotationMat = DX::XMMatrixRotationY(rotation);

		Marx::RenderCommand::setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		Marx::RenderCommand::clear();

		{
			Vertex vertex1 = { m_vertexX, m_vertexY, 0.0f, 0.0f, 0.0f };
			Vertex vertex2 = { m_vertexX, -m_vertexY, 0.0f, 0.0f, 1.0f };
			Vertex* pVertex[] = { &vertex1, &vertex2 };
			uint32_t offsets[] = { 0, 1 };
			uint32_t counts[] = { 1, 1 };
			m_pVertexArray->getVertexBuffer()->updatePartial((void**)pVertex, offsets, counts, 2);
		}

		{
			BYTE pixels1[] = {
				0, 0, 0, 255
			};
			BYTE pixels2[] = {
				255, 0, 0, 255,
				0, 255, 0, 255
			};
			BYTE* pPixels[] = { &pixels1[0], &pixels2[0] };
			uint32_t offsetsX[] = { 0, 1 };
			uint32_t offsetsY[] = { 0, 0 };
			uint32_t widths[] = { 1, 1 };
			uint32_t heights[] = { 1, 2 };
			m_pTexture->updatePartial((BYTE**)pPixels, offsetsX, offsetsY, widths, heights, 2);
		}

		if (m_usePerspective)
			Marx::Renderer::beginScene(m_perspectiveCam);
		else
			Marx::Renderer::beginScene(m_orthographicCam);
		Marx::RenderCommand::enableDepthTest(m_usePerspective);

		auto texShader = m_shaderLib.get("Texture");

		/*for (int y = 0; y < 9; ++y)
		{
			for (int x = 0; x < 9; ++x)
			{
				DX::XMMATRIX transformMat = scaleMat * DX::XMMatrixTranslation(x * 0.11f, y * 0.11f, 0.0f) * rotationMat * DX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
				Marx::Renderer::submit(texShader, m_pVertexArray, DX::XMMatrixTranspose(transformMat), m_pTexture);
			}
		}*/
		//Marx::Renderer::submit(texShader, m_pVertexArray, DX::XMMatrixTranspose(DX::XMMatrixScaling(1.5f, 1.5f, 1.5f)), m_pTexture);

		DX::XMMATRIX transformMat = scaleMat * rotationMat * DX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
		Marx::Renderer::submit(texShader, m_pCubeVertexArray, DX::XMMatrixTranspose(transformMat), m_pTexture);
		Marx::Renderer::submit(texShader, m_pVertexArray, DX::XMMatrixTranspose(DX::XMMatrixScaling(1.5f, 1.5f, 1.5f)), m_pAlphaTexture);

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
		dispatcher.dispatch<Marx::KeyPressEvent>(MX_BIND_EVENT_METHOD(ExampleLayer::onKeyPress));
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
	bool onKeyPress(Marx::KeyPressEvent& e)
	{
		if (e.getKeyCode() == MX_KEY_T)
			m_usePerspective = !m_usePerspective;
		return true;
	}
private:
	Marx::ShaderLib m_shaderLib;
	Marx::Reference<Marx::VertexArray> m_pVertexArray;
	Marx::Reference<Marx::VertexArray> m_pCubeVertexArray;
	Marx::Reference<Marx::Texture2D> m_pTexture, m_pAlphaTexture;
	DX::XMFLOAT3 m_position;
	Marx::OrthographicCamera m_orthographicCam;
	Marx::PerspectiveCamera m_perspectiveCam;
	float m_mouseX = 0.0f;
	float m_mouseY = 0.0f;
	DX::XMFLOAT4 m_color[2] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
	float m_vertexX = -0.5f;
	float m_vertexY = -0.5f;
	bool m_usePerspective = false;
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
	/*MarC::HeapMem mem(4096 * 10);
	MarC::Address addr = mem.alloc(9999);
	mem.free(&addr);

	std::string codeStr;
	{
		std::ifstream file("code.marc");
		while (!file.eof())
		{
			codeStr.push_back(file.get());
		}
		file.close();
	}

	MarC::CompileData tl;
	MarC::Tokenizer::Error tokenizeErr = MarC::Tokenizer::tokenize(codeStr, tl);
	for (auto& t : tl.tokens)
	{
		std::cout << ">>> " << t.value << " <<<" << std::endl;
	}
	std::cout << tokenizeErr.toString() << std::endl;
	MarC::ParseTree pt = MarC::Parser::parse(tl);*/

	return new Sandbox();
}