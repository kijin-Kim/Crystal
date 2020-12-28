#include "cspch.h"
#include "CameraComponent.h"

namespace Crystal {


	void CameraComponent::Update(float deltaTime)
	{
		TransformComponent::Update(deltaTime);

		DirectX::XMVECTOR position = XMLoadFloat3(&m_Position);
		DirectX::XMVECTOR lookAt = XMLoadFloat3(&m_LookAt);
		if (DirectX::XMVector3Equal(position, lookAt))
		{
			CS_ERROR("카메라의 위치와 바라보는 위치가 같습니다");
			return;
		}

		DirectX::XMMATRIX viewProjection = DirectX::XMMatrixIdentity();
		switch (m_ProjectionMode)
		{
		case ECameraProjectionMode::CPM_Persepective:
			viewProjection = XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixLookAtLH(position, lookAt, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
				DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_FieldOfView), (float)m_Viewport.Width / (float)m_Viewport.Height, m_NearPlane, m_FarPlane)));
			break;
		case ECameraProjectionMode::CPM_Orthographic:
			viewProjection = XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixLookAtLH(position, lookAt, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
				DirectX::XMMatrixOrthographicLH(m_Viewport.Width, m_Viewport.Height, m_NearPlane, m_FarPlane)));
			break;
		default:
			CS_ASSERT(false,"예상치 못한 Camera Projection Mode.");
			break;
		}

		XMStoreFloat4x4(&m_ViewProjection, viewProjection);
		XMStoreFloat4x4(&m_InverseViewProjection, XMMatrixTranspose(XMMatrixInverse(nullptr, viewProjection)));
		
	}

}