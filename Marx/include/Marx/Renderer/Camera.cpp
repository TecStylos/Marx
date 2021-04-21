#include "mxpch.h"
#include "Camera.h"

namespace Marx
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_projectionMatrix(DX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, -1.0f, 1.0f))
	{
		recalculateAll();
	}

	void OrthographicCamera::recalculateAll()
	{
		DX::XMMATRIX transform = DX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z) *
			DX::XMMatrixRotationZ(DX::XMConvertToRadians(m_rotation));

		m_viewMatrix = DX::XMMatrixInverse(NULL, transform);
		m_viewProjectionMatrix = m_viewMatrix * m_projectionMatrix;
		m_viewProjectionMatrixTransposed = DX::XMMatrixTranspose(m_viewProjectionMatrix);
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
		m_fovRad = DX::XMConvertToRadians(fovDeg);
		m_aspectRatio = aspectRatio;
		m_nearClip = nearClip;
		m_farClip = farClip;
		m_projectionMatrix = DX::XMMatrixPerspectiveFovLH(m_fovRad, m_aspectRatio, m_nearClip, m_farClip);

		recalculateAll();
	}

	void PerspectiveCamera::recalculateAll()
	{
		DX::XMMATRIX transform = DX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z) *
			DX::XMMatrixRotationRollPitchYaw(DX::XMConvertToRadians(m_rotation.x), DX::XMConvertToRadians(m_rotation.y), DX::XMConvertToRadians(m_rotation.z));

		m_viewMatrix = DX::XMMatrixInverse(NULL, transform);
		m_viewProjectionMatrix = m_viewMatrix * m_projectionMatrix;
		m_viewProjectionMatrixTransposed = DX::XMMatrixTranspose(m_viewProjectionMatrix);
	}
}