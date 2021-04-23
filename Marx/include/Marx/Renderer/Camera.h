#pragma once

//#include "Marx/Platform/DX11/DX11Includes.h"

namespace Marx
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
	public:
		void setPosition(const glm::vec3& position) { m_position = position; recalculateAll(); }
		void setRotation(float rotDeg) { m_rotDeg = rotDeg; recalculateAll(); }
	public:
		const glm::vec3& getPosition() const { return m_position; }
		float getRotation() const { return m_rotDeg; }
		const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
		const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return m_viewProjectionMatrix; }
	private:
		void recalculateAll();
	private:
		glm::vec3 m_position;
		float m_rotDeg = 0.0f;
	private:
		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewProjectionMatrix;
	};

	class PerspectiveCamera
	{
	public:
		PerspectiveCamera();
		PerspectiveCamera(float fovDeg, float aspectRatio, float nearClip, float farClip);
	public:
		void setPosition(const glm::vec3& position) { m_position = position; recalculateAll(); }
		void setRotation(const glm::vec3& rotation) { m_rotation = rotation; recalculateAll(); }
		void setProperties(float fovDeg, float aspectRatio, float nearClip, float farClip);
	public:
		const glm::vec3& getPosition() const { return m_position; }
		const glm::vec3& getRotation() const { return m_rotation; }
		const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
		const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return m_viewProjectionMatrix; }
	private:
		void recalculateAll();
	private:
		glm::vec3 m_position;
		glm::vec3 m_rotation;
		float m_fovDeg;
		float m_aspectRatio;
		float m_nearClip;
		float m_farClip;
	private:
		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewProjectionMatrix;
	};
}