#include "ControllerVisualizer.h"

struct ControllerVertex
{
	float x, y;
	float brightness;
	int partID;
};

ControllerVisualizer::ControllerVisualizer()
	: m_orthoCam(0.0f, 1.6f / 0.9f, 0.0f, 1.0f)
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

		{ -0.6f, -0.1f, 1.0f, 6 }, // DPAD Up
		{ -0.6f, 0.0f, 1.0f, 6 },
		{ -0.5f, 0.0f, 1.0f, 6 },
		{ -0.5f, -0.1f, 1.0f, 6 },

		{ -0.6f, -0.3f, 1.0f, 7 }, // DPAD Down
		{ -0.6f, -0.2f, 1.0f, 7 },
		{ -0.5f, -0.2f, 1.0f, 7 },
		{ -0.5f, -0.3f, 1.0f, 7 },

		{ -0.7f, -0.2f, 1.0f, 8 }, // DPAD Left
		{ -0.7f, -0.1f, 1.0f, 8 },
		{ -0.6f, -0.1f, 1.0f, 8 },
		{ -0.6f, -0.2f, 1.0f, 8 },

		{ -0.5f, -0.2f, 1.0f, 9 }, // DPAD Right
		{ -0.5f, -0.1f, 1.0f, 9 },
		{ -0.4f, -0.1f, 1.0f, 9 },
		{ -0.4f, -0.2f, 1.0f, 9 },

		{ -0.8f, 0.1f, 1.0f, 10 }, // Thumb Left
		{ -0.8f, 0.2f, 1.0f, 10 },
		{ -0.7f, 0.2f, 1.0f, 10 },
		{ -0.7f, 0.1f, 1.0f, 10 },

		{ -0.2f, -0.2f, 1.0f, 11 }, // Thumb Right
		{ -0.2f, -0.1f, 1.0f, 11 },
		{ -0.1f, -0.1f, 1.0f, 11 },
		{ -0.1f, -0.2f, 1.0f, 11 },

		{ -0.9f, 0.35f, 1.0f, 12 }, // Shoulder Left
		{ -0.9f, 0.4f, 1.0f, 12 },
		{ -0.6f, 0.4f, 0.0f, 12 },
		{ -0.6f, 0.35f, 0.0f, 12 },

		{ -0.1f, 0.35f, 0.0f, 13 }, // Shoulder Right
		{ -0.1f, 0.4f, 0.0f, 13 },
		{ 0.2f, 0.4f, 1.0f, 13 },
		{ 0.2f, 0.35f, 1.0f, 13 },

		{ -0.9f, 0.4f, 1.0f, 14 }, // Trigger Left
		{ -0.9f, 0.45f, 1.0f, 14 },
		{ -0.6f, 0.45f, 0.0f, 14 },
		{ -0.6f, 0.4f, 0.0f, 14 },

		{ -0.1f, 0.4f, 0.0f, 15 }, // Trigger Right
		{ -0.1f, 0.45f, 0.0f, 15 },
		{ 0.2f, 0.45f, 1.0f, 15 },
		{ 0.2f, 0.4f, 1.0f, 15 },
	};
	uint32_t vBuffSize = sizeof(vertices);
	auto pVertexBuffer = Marx::VertexBuffer::create(vertices, vBuffSize);

	pVertexBuffer->setLayout({
		{ Marx::ShaderDataType::Float2, "A_POSITION" },
		{ Marx::ShaderDataType::Float1, "A_BRIGHTNESS" },
		{ Marx::ShaderDataType::Int1, "A_PARTID" }
	});

	constexpr uint32_t nIndices = sizeof(vertices) / sizeof(ControllerVertex) / 4 * 6;
	uint32_t indices[nIndices];
	for (uint32_t i = 0; i < nIndices; i += 6)
	{
		indices[i + 0] = 4 * (i / 6) + 0;
		indices[i + 1] = 4 * (i / 6) + 1;
		indices[i + 2] = 4 * (i / 6) + 2;
		indices[i + 3] = 4 * (i / 6) + 0;
		indices[i + 4] = 4 * (i / 6) + 2;
		indices[i + 5] = 4 * (i / 6) + 3;
	}
	auto pIndexBuffer = Marx::IndexBuffer::create(
		indices,
		nIndices,
		Marx::IndexBuffer::PrimitiveType::TriangleList
	);

	m_model = Marx::VertexArray::create(pVertexBuffer, pIndexBuffer);

	m_shader = Marx::Shader::create("assets/shaders/ControllerVisualizer.hlsl");
}

void ControllerVisualizer::onUpdate()
{
	Marx::Renderer::beginScene(m_orthoCam);

	float scaling = 0.3f;
	glm::vec3 pos(0.9f, 0.3f, 0.0f);

	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scaling, scaling, scaling));
	glm::mat4 transform = scaleMat * glm::translate(glm::mat4(1.0f), pos);

	Marx::Renderer::submit(m_shader, m_model, transform, nullptr);

	Marx::Renderer::endScene();
}

bool ControllerVisualizer::onControllerConnect(Marx::ControllerConnectEvent& e)
{
	bool correct = e.getID() == m_cid;

	if (correct)
		m_connected = true;

	return false;
}

bool ControllerVisualizer::onControllerDisconnect(Marx::ControllerDisconnectEvent& e)
{
	bool correct = e.getID() == m_cid;

	if (correct)
		m_connected = false;

	return false;
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

	return false;
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

	return false;
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

	return false;
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

	return false;
}

bool ControllerVisualizer::onWindowResize(Marx::WindowResizeEvent& e)
{
	m_orthoCam = Marx::OrthographicCamera(0.0f, (float)e.getWidth() / (float)e.getHeight(), 0.0f, 1.0f);
	return false;
}
