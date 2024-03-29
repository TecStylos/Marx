#if defined(MX_DEBUG) && !defined(MX_ENABLE_PROFILING)
#define MX_ENABLE_PROFILING
#endif
#include <Marx.h>

#include "ControllerVisualizer.h"

struct Vertex
{
	float x, y, z;
	float u, v;
	float l, m, n;
};

class ExampleLayer : public Marx::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_orthographicCam(-1.6f, 1.6f, -0.9f, 0.9f), m_position(0.0f, 0.0f, 0.0f)
	{
		MX_PROFILE_FUNCTION();

		// Vertex Buffer
		Vertex vertices[] =
		{
			{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f }
		};
		Marx::Reference<Marx::VertexBuffer> pVertexBuffer(Marx::VertexBuffer::create(vertices, sizeof(vertices)));

		// Vertex Layout
		pVertexBuffer->setLayout({
			{ Marx::ShaderDataType::Float3, "A_POSITION" },
			{ Marx::ShaderDataType::Float2, "A_TEXCOORD" },
			{ Marx::ShaderDataType::Float3, "A_NORMAL" }
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
			{ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{  0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },

			// Back
			{  0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f },
			{  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f },
			{ -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f },
			{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f },

			// Top
			{ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f },
			{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
			{  0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },

			// Bottom
			{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f },
			{ -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f },
			{  0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f },
			{  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f },

			// Left
			{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },
			{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f },
			{ -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f },
			{ -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f },

			// Right
			{  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f },
			{  0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f },
			{  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		};
		Marx::Reference<Marx::VertexBuffer> pCubeVertexBuffer(Marx::VertexBuffer::create(cubeVertices, sizeof(cubeVertices)));

		pCubeVertexBuffer->setLayout({
			{ Marx::ShaderDataType::Float3, "A_POSITION" },
			{ Marx::ShaderDataType::Float2, "A_TEXCOORD" },
			{ Marx::ShaderDataType::Float3, "A_NORMAL" }
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
		m_shaderLib.load("assets/shaders/PhongTexture", true);

		m_pTexture = Marx::Texture2D::create("assets/textures/testLow.png");
		m_pAlphaTexture = Marx::Texture2D::create("assets/textures/testAlpha.png");

		Marx::RenderCommand::enableBlending(true);

		m_perspectiveCam.setProperties(90.0f, 16.0f / 9.0f, 0.001f, 1000.0f);
		m_orthographicCam.setPosition(glm::vec3(0.0f, 0.0f, 1.0f));

		m_controller = Marx::ControllerManager::getController(Marx::ControllerManager::createController());
		m_conVisualizer.setControllerID(m_controller->getID());
	}
	~ExampleLayer()
	{
		Marx::ControllerManager::destroyController(m_controller->getID());
		MX_PROFILER_END_SESSION();
	}
public:
	virtual void onUpdate(Marx::Timestep ts) override
	{
		MX_PROFILE_FUNCTION();

		static constexpr float moveSpeed = 1.0f;
		static constexpr float rotSpeed = 90.0f;
		static float rotation = 0.0f;

		if (Marx::Input::isKeyPressed(Marx::Key::Q))
			rotation += rotSpeed * ts;
		if (Marx::Input::isKeyPressed(Marx::Key::E))
			rotation -= rotSpeed * ts;

		if (Marx::Input::isKeyPressed(Marx::Key::W))
			m_position.y += moveSpeed * ts;
		if (Marx::Input::isKeyPressed(Marx::Key::S))
			m_position.y -= moveSpeed * ts;
		if (Marx::Input::isKeyPressed(Marx::Key::A))
			m_position.x -= moveSpeed * ts;
		if (Marx::Input::isKeyPressed(Marx::Key::D))
			m_position.x += moveSpeed * ts;

		if (Marx::Input::isKeyPressed(Marx::Key::Left))
			m_vertexX -= 0.001f;
		if (Marx::Input::isKeyPressed(Marx::Key::Right))
			m_vertexX += 0.001f;
		if (Marx::Input::isKeyPressed(Marx::Key::Down))
			m_vertexY -= 0.001f;
		if (Marx::Input::isKeyPressed(Marx::Key::Up))
			m_vertexY += 0.001f;

		glm::vec3 camPos = m_orthographicCam.getPosition();
		if (Marx::Input::isKeyPressed(Marx::Key::I))
			camPos.y += moveSpeed * ts;
		if (Marx::Input::isKeyPressed(Marx::Key::K))
			camPos.y -= moveSpeed * ts;
		if (Marx::Input::isKeyPressed(Marx::Key::J))
			camPos.x -= moveSpeed * ts;
		if (Marx::Input::isKeyPressed(Marx::Key::L))
			camPos.x += moveSpeed * ts;
		if (Marx::Input::isKeyPressed(Marx::Key::O))
			camPos.z -= moveSpeed * ts;
		if (Marx::Input::isKeyPressed(Marx::Key::U))
			camPos.z += moveSpeed * ts;

		auto& stick = m_controller->stickState(Marx::ControllerStick::Left);
		camPos.x += stick.x * ts;
		camPos.y += stick.y * ts;
		camPos.z += m_controller->triggerState(Marx::ControllerTrigger::Left) * ts;
		camPos.z -= m_controller->triggerState(Marx::ControllerTrigger::Right) * ts;

		m_orthographicCam.setPosition(camPos);
		m_perspectiveCam.setPosition(camPos);

		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
		glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 1.0f, 0.0f));

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
			uint8_t pixels1[] = {
				0, 0, 0, 255
			};
			uint8_t pixels2[] = {
				255, 0, 0, 255,
				0, 255, 0, 255
			};
			uint8_t* pPixels[] = { &pixels1[0], &pixels2[0] };
			uint32_t offsetsX[] = { 0, 1 };
			uint32_t offsetsY[] = { 0, 0 };
			uint32_t widths[] = { 1, 1 };
			uint32_t heights[] = { 1, 2 };
			m_pTexture->updatePartial((uint8_t**)pPixels, offsetsX, offsetsY, widths, heights, 2);
		}

		if (m_usePerspective)
			Marx::Renderer::beginScene(m_perspectiveCam);
		else
			Marx::Renderer::beginScene(m_orthographicCam);
		Marx::RenderCommand::enableDepthTest(m_usePerspective);

		auto texShader = m_shaderLib.get("PhongTexture");

		glm::vec3 lightPos(0.0f, 0.0f, 2.0f);

		texShader->updateUniform("c_ambientColor", &m_ambientColor, Marx::ShaderDataType::Float3);
		texShader->updateUniform("c_lightPos", &lightPos, Marx::ShaderDataType::Float3);
		texShader->updateUniform("c_diffuseColor", &m_diffuseColor, Marx::ShaderDataType::Float3);
		texShader->updateUniform("c_diffuseIntensity", &m_diffuseIntensity, Marx::ShaderDataType::Float1);
		texShader->updateUniform("c_attConst", &m_attConst, Marx::ShaderDataType::Float1);
		texShader->updateUniform("c_attLin", &m_attLin, Marx::ShaderDataType::Float1);
		texShader->updateUniform("c_attQuad", &m_attQuad, Marx::ShaderDataType::Float1);
		texShader->updateUniform("c_specularIntensity", &m_specularIntensity, Marx::ShaderDataType::Float1);
		texShader->updateUniform("c_specularPower", &m_specularPower, Marx::ShaderDataType::Float1);

		for (uint32_t x = 0; x < 10; ++x)
		{
			for (uint32_t y = 0; y < 10; ++y)
			{
				glm::vec3 pos = m_position;
				pos.x += (float)x * 0.75;
				pos.y += (float)y * 0.75;
				glm::mat4 transformMat = glm::translate(glm::mat4(1.0f), pos) * rotationMat * scaleMat;
				Marx::Renderer::submit(texShader, m_pCubeVertexArray, transformMat, m_pTexture);
			}
		}
		Marx::Renderer::submit(texShader, m_pVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f)), m_pAlphaTexture);

		Marx::Renderer::endScene();

		m_conVisualizer.onUpdate();
	}
	virtual void onImGuiRender() override
	{
		MX_PROFILE_FUNCTION();

		ImGui::Begin("Color");
		ImGui::ColorEdit3("Ambient color", &m_ambientColor.r);
		ImGui::ColorEdit3("Light color", &m_diffuseColor.r);
		ImGui::SliderFloat("Diffuse Intensity", &m_diffuseIntensity, 0.0f, 1.0f, "%.2f", 1.0f);
		ImGui::SliderFloat("Specular Intensity", &m_specularIntensity, 0.0f, 1.0f, "%.2f", 1.0f);
		ImGui::SliderFloat("Specular Power", &m_specularPower, 0.0f, 1.0f, "%.2f", 1.0f);
		ImGui::SliderFloat("Attenuation Const", &m_attConst, 0.0f, 1.0f, "%.2f", 1.0f);
		ImGui::SliderFloat("Attenuation Linear", &m_attLin, 0.0f, 1.0f, "%.2f", 1.0f);
		ImGui::SliderFloat("Attenuation Quadratic", &m_attQuad, 0.0f, 1.0f, "%.2f", 1.0f);
		ImGui::End();
	}
	virtual void onEvent(Marx::Event& event) override
	{
		Marx::EventDispatcher dispatcher(event);

		dispatcher.dispatch<Marx::ControllerConnectEvent>(MX_BIND_EVENT_METHOD_OBJ(&m_conVisualizer, ControllerVisualizer::onControllerConnect));
		dispatcher.dispatch<Marx::ControllerDisconnectEvent>(MX_BIND_EVENT_METHOD_OBJ(&m_conVisualizer, ControllerVisualizer::onControllerDisconnect));
		dispatcher.dispatch<Marx::ControllerButtonPressEvent>(MX_BIND_EVENT_METHOD_OBJ(&m_conVisualizer, ControllerVisualizer::onControllerButtonPress));
		dispatcher.dispatch<Marx::ControllerButtonReleaseEvent>(MX_BIND_EVENT_METHOD_OBJ(&m_conVisualizer, ControllerVisualizer::onControllerButtonRelease));
		dispatcher.dispatch<Marx::ControllerStickMoveEvent>(MX_BIND_EVENT_METHOD_OBJ(&m_conVisualizer, ControllerVisualizer::onControllerStickMove));
		dispatcher.dispatch<Marx::ControllerTriggerMoveEvent>(MX_BIND_EVENT_METHOD_OBJ(&m_conVisualizer, ControllerVisualizer::onControllerTriggerMove));
		dispatcher.dispatch<Marx::WindowResizeEvent>(MX_BIND_EVENT_METHOD_OBJ(&m_conVisualizer, ControllerVisualizer::onWindowResize));

		dispatcher.dispatch<Marx::ControllerButtonPressEvent>(MX_BIND_EVENT_METHOD(ExampleLayer::onControllerButtonPress));
		dispatcher.dispatch<Marx::MouseMoveEvent>(MX_BIND_EVENT_METHOD(ExampleLayer::onMouseMove));
		dispatcher.dispatch<Marx::KeyPressEvent>(MX_BIND_EVENT_METHOD(ExampleLayer::onKeyPress));
		dispatcher.dispatch<Marx::WindowResizeEvent>(MX_BIND_EVENT_METHOD(ExampleLayer::onWindowResize));
	}
	bool onMouseMove(Marx::MouseMoveEvent& e)
	{
		m_mouseX = -(e.getX() / (Marx::Application::get()->getWindow()->getWidth() / 2) - 1.0f) * 1.6f;
		m_mouseY = e.getY() / (Marx::Application::get()->getWindow()->getHeight() / 2) - 1.0f;
		return true;
	}
	bool onKeyPress(Marx::KeyPressEvent& e)
	{
		if (e.getKeyCode() == Marx::Key::T)
			m_usePerspective = !m_usePerspective;
		return true;
	}
	bool onWindowResize(Marx::WindowResizeEvent& e)
	{
		m_perspectiveCam.setProperties(90.0f, (float)e.getWidth() / (float)e.getHeight(), 0.001f, 1000.0f);
		return true;
	}
	bool onControllerButtonPress(Marx::ControllerButtonPressEvent& e)
	{
		if (e.getButton() == Marx::ControllerButton::Start)
			m_usePerspective = !m_usePerspective;
		return true;
	}
private:
	Marx::ShaderLib m_shaderLib;
	Marx::Reference<Marx::VertexArray> m_pVertexArray;
	Marx::Reference<Marx::VertexArray> m_pCubeVertexArray;
	Marx::Reference<Marx::Texture2D> m_pTexture, m_pAlphaTexture;
	glm::vec3 m_position;

	glm::vec3 m_diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 m_ambientColor = glm::vec3(0.05f, 0.05f, 0.05f);
	float m_diffuseIntensity = 0.1f;
	float m_attConst = 1.0f;
	float m_attLin = 0.045f;
	float m_attQuad = 0.0075f;
	float m_specularIntensity = 1.0f;
	float m_specularPower = 0.9f;

	Marx::OrthographicCamera m_orthographicCam;
	Marx::PerspectiveCamera m_perspectiveCam;
	float m_mouseX = 0.0f;
	float m_mouseY = 0.0f;
	glm::vec4 m_color[2] = { glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) };
	float m_vertexX = -0.5f;
	float m_vertexY = -0.5f;
	bool m_usePerspective = false;

	Marx::Reference<Marx::Controller> m_controller;
	ControllerVisualizer m_conVisualizer;
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

MX_DEFAULT_CREATE_APPLICATION(Sandbox)