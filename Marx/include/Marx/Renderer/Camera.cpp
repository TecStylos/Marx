#include "mxpch.h"
#include "Camera.h"

namespace Marx
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_projectionMatrix(glm::ortho(left, right, bottom, top))
	{
		recalculateAll();
	}

	void OrthographicCamera::recalculateAll()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0.0f, 0.0f, 1.0f));

		m_viewMatrix = glm::transpose(transform);
		m_viewProjectionMatrix = m_viewMatrix * m_projectionMatrix;
		m_viewProjectionMatrixTransposed = glm::transpose(m_viewProjectionMatrix);
	}

	PerspectiveCamera::PerspectiveCamera()
	{
		setProperties(90.0f, 1.0f, 0.01f, 1000.0f);
	}

	PerspectiveCamera::PerspectiveCamera(float fovDeg, float aspectRatio, float nearClip, float farClip)
	{
		setProperties(fovDeg, aspectRatio, nearClip, farClip);
	}

	void PerspectiveCamera::setProperties(float fovDeg, float aspectRatio, float nearClip, float farClip)
	{
		m_fovDeg = fovDeg;
		m_aspectRatio = aspectRatio;
		m_nearClip = nearClip;
		m_farClip = farClip;
		m_projectionMatrix = glm::perspective(m_fovDeg, m_aspectRatio, m_nearClip, m_farClip);

		recalculateAll();
	}

	void PerspectiveCamera::recalculateAll()
	{
		glm::quat orientation = glm::quat(glm::vec3(m_rotation.y, m_rotation.z, m_rotation.x));
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);

		m_viewMatrix = glm::inverse(transform);
		m_viewProjectionMatrix = m_viewMatrix * m_projectionMatrix;
		m_viewProjectionMatrixTransposed = glm::transpose(m_viewProjectionMatrix);
	}
}