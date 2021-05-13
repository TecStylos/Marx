#pragma once

#define MX_NO_ENTRY_POINT
#include <Marx.h>

class ControllerVisualizer
{
public:
	ControllerVisualizer();
public:
	void setControllerID(Marx::ControllerID cid) { m_cid = cid; }
	void onUpdate();
public:
	bool onControllerConnect(Marx::ControllerConnectEvent& e);
	bool onControllerDisconnect(Marx::ControllerDisconnectEvent& e);
	bool onControllerButtonPress(Marx::ControllerButtonPressEvent& e);
	bool onControllerButtonRelease(Marx::ControllerButtonReleaseEvent& e);
	bool onControllerStickMove(Marx::ControllerStickMoveEvent& e);
	bool onControllerTriggerMove(Marx::ControllerTriggerMoveEvent& e);
	bool onWindowResize(Marx::WindowResizeEvent& e);
private:
	Marx::ControllerID m_cid = 0;
	Marx::OrthographicCamera m_orthoCam;
	Marx::Reference<Marx::VertexArray> m_model;
	Marx::Reference<Marx::Shader> m_shader;
};