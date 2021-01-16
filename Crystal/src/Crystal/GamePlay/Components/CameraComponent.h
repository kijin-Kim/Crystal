#pragma once
#include "TransformComponent.h"
#include "Crystal/Math/Math.h"

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

		virtual void Update(float deltaTime) override
		{
			TransformComponent::Update(deltaTime);

			computeViewProjection();
		}
		void SetWorldPosition(const DirectX::XMFLOAT3& position) { m_Position = position; }
		void SetViewport(const D3D12_VIEWPORT& viewport) { m_Viewport = viewport;}
		void SetScissorRect(const D3D12_RECT& scissorRect) { m_ScissorRect = scissorRect;}
		void SetFieldOfView(float fieldOfView) { m_FieldOfView = fieldOfView; }
		void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane;}
		void SetFarPlane(float farPlane) { m_FarPlane = farPlane;}
		void SetProjectionMode(ECameraProjectionMode mode) { m_ProjectionMode = mode;}
		void SetMaxCameraPitch(float pitch) { m_MaxPitch = pitch; }

		void RotateRoll(float roll) { m_RollPitchYaw.x += roll; }
		void RotatePitch(float pitch) { m_RollPitchYaw.y += pitch; m_RollPitchYaw.y = std::clamp(m_RollPitchYaw.y, -m_MaxPitch, m_MaxPitch); }
		void RotateYaw(float yaw) { m_RollPitchYaw.z += yaw; }
		void RotateRollPitchYaw(const DirectX::XMFLOAT3& rollPitchYaw) 
		{ 
			m_RollPitchYaw = Vector3::Add(m_RollPitchYaw, rollPitchYaw); 
			m_RollPitchYaw.y = std::clamp(m_RollPitchYaw.y, -m_MaxPitch, m_MaxPitch);
		}

		const DirectX::XMFLOAT3 GetWorldPosition() const { return m_Position; }
		const DirectX::XMFLOAT3 GetRight() const { return Vector3::RotateQuaternion({ 1.0f, 0.0f, 0.0f }, GetOrientation()); }
		const DirectX::XMFLOAT3 GetUp() const { return Vector3::RotateQuaternion({ 0.0f, 1.0f, 0.0f }, GetOrientation()); }
		const DirectX::XMFLOAT3 GetForward() const { return Vector3::RotateQuaternion({ 0.0f, 0.0f, 1.0f }, GetOrientation()); }
		const DirectX::XMFLOAT4 GetOrientation() const { return Vector4::QuaternionRollPitchYaw(m_RollPitchYaw); }

		const D3D12_VIEWPORT& GetViewport() const { return m_Viewport; }
		const D3D12_RECT& GetScissorRect() const { return m_ScissorRect; }

		/*VP와 InverseVP의 값은 매 프레임마다 Update가 불린 직후 어떠한 속성도 바뀌지 않았을 때만 보장됩니다.*/
		const DirectX::XMFLOAT4X4& GetViewProjection() const { return m_ViewProjection; }

	private:
		void computeViewProjection();
		
	private:
		DirectX::XMFLOAT3 m_Position = { 0, 10.0f, -1.0f };
		DirectX::XMFLOAT3 m_RollPitchYaw = { 0.0f, 0.0f, 0.0f };

		DirectX::XMFLOAT4X4 m_ViewProjection = {};
		D3D12_VIEWPORT m_Viewport = {};
		D3D12_RECT m_ScissorRect = {};

		float m_FieldOfView = 60.0f;
		float m_NearPlane = 0.1f;
		float m_FarPlane = 1000.0f;
		float m_MaxPitch = 90.0f;

		ECameraProjectionMode m_ProjectionMode = ECameraProjectionMode::CPM_Persepective;
	};
}