#pragma once

//#include <Marx.h>
//
//class ExampleLayer : public Marx::Layer
//{
//public:
//	ExampleLayer();
//public:
//	virtual void onUpdate(Marx::Timestep ts) override;
//	virtual void onImGuiRender() override;
//	virtual void onEvent(Marx::Event& event) override;
//	bool onMouseMove(Marx::MouseMoveEvent& e);
//	bool onMouseButtonPress(Marx::MouseButtonPressEvent& e);
//private:
//	Marx::Reference<Marx::Shader> m_pShader;
//	Marx::Reference<Marx::VertexArray> m_pVertexArray;
//	DX::XMFLOAT3 m_position;
//	Marx::OrthographicCamera m_camera;
//	float m_mouseX = 0.0f;
//	float m_mouseY = 0.0f;
//	DX::XMFLOAT4 m_color[2] = { {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f} };
//	float m_vertexX = -0.5f;
//};