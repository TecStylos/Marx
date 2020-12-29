#include <Marx.h>

#include "imgui/imgui.h"

class ExampleLayer : public Marx::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{}

	void onUpdate() override
	{}

	virtual void onImGuiRender() override {
	}

	void onEvent(Marx::Event& event) override
	{}
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