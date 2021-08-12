#include "cspch.h"
#include "CameraComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

BOOST_CLASS_EXPORT(Crystal::CameraComponent)

namespace Crystal {
	CameraComponent::CameraComponent()
	{
		int resolutionWidth = 1920.0f;
		int resolutionHeight = 1080.0f;
		m_ScissorRect = {0, 0, resolutionWidth, resolutionHeight};
		m_Viewport = {
			0.0f, 0.0f, static_cast<float>(resolutionWidth), static_cast<float>(resolutionHeight), 0.0f, 1.0f
		};
	}

	void CameraComponent::RegisterComponent()
	{
		TransformComponent::RegisterComponent();

		auto level = Cast<Level>(Cast<Actor>(GetOuter())->GetOuter());
		level->GetScene()->Cameras.push_back(Cast<CameraComponent>(shared_from_this()));
	}

	void CameraComponent::Update(const float deltaTime)
	{
		TransformComponent::Update(deltaTime);
		computeViewProjection();
	}

	DirectX::XMFLOAT2 CameraComponent::ProjectWorldToCameraSpace(const DirectX::XMFLOAT3& worldPosition)
	{
		auto projected = Crystal::Vector4::Transform({worldPosition.x, worldPosition.y, worldPosition.z, 1.0f}, m_ViewProjection);
		DirectX::XMFLOAT2 cameraSpacePosition = {projected.x, projected.y};
		if (projected.w > 0.0f)
		{
			float invW = 1.0f / projected.w;
			cameraSpacePosition = {cameraSpacePosition.x * invW, cameraSpacePosition.y * invW};
		}

		cameraSpacePosition.x = cameraSpacePosition.x / 2.0f * m_Viewport.Width;
		cameraSpacePosition.y = cameraSpacePosition.y / 2.0f * m_Viewport.Height;

		return cameraSpacePosition;
	}

	void CameraComponent::computeViewProjection()
	{
		const auto& worldTransform = GetWorldTransform();
		const DirectX::XMFLOAT3 worldPosition = {worldTransform._41, worldTransform._42, worldTransform._43};
		DirectX::XMFLOAT3 worldForward = {worldTransform._31, worldTransform._32, worldTransform._33};
		DirectX::XMFLOAT3 worldUp = {worldTransform._21, worldTransform._22, worldTransform._23};

		/// 해결법 Controller가 직접 rotation을 받아 Matrix를 만들기?

		switch (m_ProjectionMode)
		{
		case ECameraProjectionMode::CPM_Persepective:
			m_View = Matrix4x4::LookTo(worldPosition, worldForward, worldUp);
			m_Projection = Matrix4x4::Perspective(DirectX::XMConvertToRadians(m_FieldOfView),
			                                      static_cast<float>(m_Viewport.Width) / static_cast<float>(m_Viewport.
				                                      Height), m_NearPlane, m_FarPlane);
			m_ViewProjection = Matrix4x4::Multiply(m_View, m_Projection);
			break;
		case ECameraProjectionMode::CPM_Orthographic:
			m_View = Matrix4x4::LookTo(worldPosition, worldForward, worldUp);
			m_Projection = Matrix4x4::OrthoGraphic(1920.0f, 1080.0f, m_NearPlane, m_FarPlane);
			m_ViewProjection = Matrix4x4::Multiply(m_View, m_Projection);
			break;
		default:
			CS_FATAL(false, "예상치 못한 Camera Projection Mode.");
			break;
		}
	}
}
