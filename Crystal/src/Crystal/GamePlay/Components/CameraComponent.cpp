#include "cspch.h"
#include "CameraComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/Core/ApplicationUtility.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

namespace Crystal {
	CameraComponent::CameraComponent()
	{
		
		int resolutionWidth = 1920.0f;
		int resolutionHeight = 1080.0f;
		m_ScissorRect = { 0, 0, resolutionWidth, resolutionHeight };
		m_Viewport = { 0.0f, 0.0f, static_cast<float>(resolutionWidth), static_cast<float>(resolutionHeight), 0.0f, 1.0f };
	}

	void CameraComponent::Update(const float deltaTime)
	{
		TransformComponent::Update(deltaTime);
		computeViewProjection();
	}

	void CameraComponent::computeViewProjection()
	{
		const auto& worldTransform = GetWorldTransform();
		const DirectX::XMFLOAT3 worldPosition = { worldTransform._41, worldTransform._42, worldTransform._43 };
		DirectX::XMFLOAT3 worldForward = { worldTransform._31, worldTransform._32, worldTransform._33 };
		DirectX::XMFLOAT3 worldUp = { worldTransform._21, worldTransform._22, worldTransform._23 };

		/// �ذ�� Controller�� ���� rotation�� �޾� Matrix�� �����?

		switch (m_ProjectionMode)
		{
		case ECameraProjectionMode::CPM_Persepective:
			m_View = Matrix4x4::LookTo(worldPosition, worldForward, worldUp);
			m_Projection = Matrix4x4::Perspective(DirectX::XMConvertToRadians(m_FieldOfView),
				static_cast<float>(m_Viewport.Width) / static_cast<float>(m_Viewport.Height), m_NearPlane, m_FarPlane);
			
			m_ViewProjection = Matrix4x4::Multiply(m_View, m_Projection);
			break;
		case ECameraProjectionMode::CPM_Orthographic:
			break;
		default:
			CS_FATAL(false, "����ġ ���� Camera Projection Mode.");
			break;
		}
	}
}