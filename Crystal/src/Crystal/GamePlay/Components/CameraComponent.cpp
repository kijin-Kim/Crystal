#include "cspch.h"
#include "CameraComponent.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	CameraComponent::CameraComponent(const std::string& name) : TransformComponent(name)
	{
		Renderer& renderer = Renderer::Instance();
		int resolutionWidth = renderer.GetResolutionWidth();
		int resolutionHeight = renderer.GetResolutionHeight();
		m_ScissorRect = { 0, 0, resolutionWidth, resolutionHeight };
		m_Viewport = { 0.0f, 0.0f, static_cast<float>(resolutionWidth), static_cast<float>(resolutionHeight), 0.0f, 1.0f };
	}

	void CameraComponent::computeViewProjection()
	{
		const auto worldTransform = GetWorldTransform();
		const DirectX::XMFLOAT3 worldPosition = { worldTransform._41, worldTransform._42, worldTransform._43 };
		const DirectX::XMFLOAT3 worldForward = { worldTransform._31, worldTransform._32, worldTransform._33 };
		const DirectX::XMFLOAT3 worldUp = { worldTransform._21, worldTransform._22, worldTransform._23 };
		
		
	
		switch (m_ProjectionMode)
		{
		case ECameraProjectionMode::CPM_Persepective:
			m_ViewProjection = Matrix4x4::Multiply(Matrix4x4::LookTo(worldPosition, worldForward, worldUp), 
				Matrix4x4::Perspective(m_FieldOfView, static_cast<float>(m_Viewport.Width) / static_cast<float>(m_Viewport.Height), m_NearPlane, m_FarPlane));
			break;
		case ECameraProjectionMode::CPM_Orthographic:
			break;
		default:
			CS_FATAL(false, "예상치 못한 Camera Projection Mode.");
			break;
		}
	}

}