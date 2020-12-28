#pragma once
#include "TransformComponent.h"

namespace Crystal {

	enum class ECameraProjectionMode 
	{
		CPM_Persepective,
		CPM_Orthographic
	};

	/* 게임내에서의 카메라를 나타내는 클래스입니다 */
	class CameraComponent : public TransformComponent
	{
	public:
		CameraComponent() = default;
		virtual ~CameraComponent() = default;

		virtual void Update(float deltaTime) override;

		void SetWorldPosition(const DirectX::XMFLOAT3& position) { m_Position = position; }
//		void SetRelativePosition()
		void SetLookAt(const DirectX::XMFLOAT3& lookAt) { m_LookAt = lookAt;}
		void SetViewport(const D3D12_VIEWPORT& viewport) { m_Viewport = viewport; }
		void SetScissorRect(const D3D12_RECT& scissorRect) { m_ScissorRect = scissorRect; }

		void SetFieldOfView(float fieldOfView) { m_FieldOfView = fieldOfView; }
		void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane; }
		void SetFarPlane(float farPlane) { m_FarPlane = farPlane; }

		void SetProjectionMode(ECameraProjectionMode mode) { m_ProjectionMode = mode; }

		const DirectX::XMFLOAT3 GetWorldPosition() const { return m_Position; }
		const DirectX::XMFLOAT4X4& GetViewProjection() const { return m_ViewProjection; }
		const DirectX::XMFLOAT4X4& GetInverseViewProjection() const { return m_InverseViewProjection; }
		const D3D12_VIEWPORT& GetViewport() const { return m_Viewport; }
		const D3D12_RECT& GetScissorRect() const { return m_ScissorRect; }
	private:
		DirectX::XMFLOAT3 m_Position = { 0, 10.0f, -1.0f };
		DirectX::XMFLOAT3 m_LookAt = { 0, 0, 0 };
		DirectX::XMFLOAT4X4 m_ViewProjection = {};
		DirectX::XMFLOAT4X4 m_InverseViewProjection = {};
		D3D12_VIEWPORT m_Viewport = {};
		D3D12_RECT m_ScissorRect = {};

		float m_FieldOfView = 60.0f;
		float m_NearPlane = 0.1f;
		float m_FarPlane = 1000.0f;

		ECameraProjectionMode m_ProjectionMode = ECameraProjectionMode::CPM_Persepective;
	};

}