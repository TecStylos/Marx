#include "ControllerVisualizer.h"

struct ControllerVertex
{
	float x, y;
	float brightness;
	int partID;
};

ControllerVisualizer::ControllerVisualizer()
	: m_orthoCam(-1.6f, 1.6f, -0.9f, 0.9f)
{
	ControllerVertex vertices[] = {
		{ 0.0f, 0.0f, 1.0f, 0 }, // Button A
		{ 0.0f, 0.1f, 1.0f, 0 },
		{ 0.1f, 0.1f, 1.0f, 0 },
		{ 0.1f, 0.0f, 1.0f, 0 },

		{ 0.1f, 0.1f, 1.0f, 1 }, // Button B
		{ 0.1f, 0.2f, 1.0f, 1 },
		{ 0.2f, 0.2f, 1.0f, 1 },
		{ 0.2f, 0.1f, 1.0f, 1 },

		{ -0.1f, 0.1f, 1.0f, 2 }, // Button X
		{ -0.1f, 0.2f, 1.0f, 2 },
		{ 0.0f, 0.2f, 1.0f, 2 },
		{ 0.0f, 0.1f, 1.0f, 2 },

		{ 0.0f, 0.2f, 1.0f, 3 }, // Button Y
		{ 0.0f, 0.3f, 1.0f, 3 },
		{ 0.1f, 0.3f, 1.0f, 3 },
		{ 0.1f, 0.2f, 1.0f, 3 },

		{ -0.3f, 0.1f, 1.0f, 4 }, // Button Start
		{ -0.3f, 0.2f, 1.0f, 4 },
		{ -0.2f, 0.2f, 1.0f, 4 },
		{ -0.2f, 0.1f, 1.0f, 4 },

		{ -0.5f, 0.1f, 1.0f, 5 }, // Button Back
		{ -0.5f, 0.2f, 1.0f, 5 },
		{ -0.4f, 0.2f, 1.0f, 5 },
		{ -0.4f, 0.1f, 1.0f, 5 },
	};
	uint32_t vBuffSize = sizeof(vertices);
	auto pVertexBuffer = Marx::VertexBuffer::create(vertices, vBuffSize);

	pVertexBuffer->setLayout({
		{ Marx::ShaderDataType::Float2, "A_POSITION" },
		{ Marx::ShaderDataType::Float1, "A_BRIGHTNESS" },
		{ Marx::ShaderDataType::Int1, "A_PARTID" }
	});

	uint32_t indices[] = {
		 4 * 0 + 0, 4 * 0 + 1, 4 * 0 + 2, // Button A
		 4 * 0 + 0, 4 * 0 + 2, 4 * 0 + 3,

		 4 * 1 + 0, 4 * 1 + 1, 4 * 1 + 2, // Button B
		 4 * 1 + 0, 4 * 1 + 2, 4 * 1 + 3,

		 4 * 2 + 0, 4 * 2 + 1, 4 * 2 + 2, // Button X
		 4 * 2 + 0, 4 * 2 + 2, 4 * 2 + 3,

		 4 * 3 + 0, 4 * 3 + 1, 4 * 3 + 2, // Button Y
		 4 * 3 + 0, 4 * 3 + 2, 4 * 3 + 3,

		 4 * 4 + 0, 4 * 4 + 1, 4 * 4 + 2, // Button Start
		 4 * 4 + 0, 4 * 4 + 2, 4 * 4 + 3,

		 4 * 5 + 0, 4 * 5 + 1, 4 * 5 + 2, // Button Back
		 4 * 5 + 0, 4 * 5 + 2, 4 * 5 + 3,
	};
	uint32_t iBuffNum = sizeof(indices) / sizeof(uint32_t);
	auto pIndexBuffer = Marx::IndexBuffer::create(
		indices,
		iBuffNum,
		Marx::IndexBuffer::PrimitiveType::TriangleList
	);

	m_model = Marx::VertexArray::create(pVertexBuffer, pIndexBuffer);

	m_shader = Marx::Shader::create("assets/shaders/ControllerVisualizer.hlsl");
}

void ControllerVisualizer::onUpdate()
{
	Marx::Renderer::beginScene(m_orthoCam);

	float scaling = 1.0f;
	glm::vec3 pos(0.0f, 0.0f, 0.0f);

	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scaling, scaling, scaling));
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scaleMat;

	Marx::Renderer::submit(m_shader, m_model, transform, nullptr);

	Marx::Renderer::endScene();
}

bool ControllerVisualizer::onControllerConnect(Marx::ControllerConnectEvent& e)
{
	bool correct = e.getID() == m_cid;

	if (correct)
		m_connected = true;

	return correct;
}

bool ControllerVisualizer::onControllerDisconnect(Marx::ControllerDisconnectEvent& e)
{
	bool correct = e.getID() == m_cid;

	if (correct)
		m_connected = false;

	return correct;
}

bool ControllerVisualizer::onControllerButtonPress(Marx::ControllerButtonPressEvent& e)
{
	bool correct = e.getID() == m_cid;

	static constexpr float value = 1.0f;

	if (correct)
		m_shader->updateUniform(
			"c_conButton_" + 
			Marx::ControllerButtonString(e.getButton()),
			&value,
			Marx::ShaderDataType::Float1
		);

	return correct;
}

bool ControllerVisualizer::onControllerButtonRelease(Marx::ControllerButtonReleaseEvent& e)
{
	bool correct = e.getID() == m_cid;

	static constexpr bool value = 0.0f;

	if (correct)
		m_shader->updateUniform(
			"c_conButton_" +
			Marx::ControllerButtonString(e.getButton()),
			&value,
			Marx::ShaderDataType::Float1
		);

	return correct;
}

bool ControllerVisualizer::onControllerStickMove(Marx::ControllerStickMoveEvent& e)
{
	bool correct = e.getID() == m_cid;

	auto state = e.getState();

	if (correct)
		m_shader->updateUniform(
			"c_conStick_" + Marx::ControllerStickString(e.getStick()),
			&state,
			Marx::ShaderDataType::Float2
		);

	return correct;
}

bool ControllerVisualizer::onControllerTriggerMove(Marx::ControllerTriggerMoveEvent& e)
{
	bool correct = e.getID() == m_cid;

	auto delta = e.getDelta();

	if (correct)
		m_shader->updateUniform(
			"c_conTrigger_" + Marx::ControllerTriggerString(e.getTrigger()),
			&delta,
			Marx::ShaderDataType::Float1
		);

	return correct;
}