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

			DirectX::XMFLOAT4 pitchQuaternion = Vector4::QuternionRotationAxis(GetRight(), m_RollPitchYaw.y);
			SetLookTo(Vector3::RotateQuaternion(m_LookTo, pitchQuaternion));

			DirectX::XMFLOAT4 yawQuaternion = Vector4::QuternionRotationAxis(m_Up, m_RollPitchYaw.z);
			SetLookTo(Vector3::RotateQuaternion(m_LookTo, yawQuaternion));

			m_RollPitchYaw = { 0.0f, 0.0f, 0.0f };
		}
		void SetWorldPosition(const DirectX::XMFLOAT3& position) { m_Position = position; computeViewProjection(); }
		//		void SetRelativePosition()
		void SetLookAt(const DirectX::XMFLOAT3& lookAt) 
		{ 
			m_LookAt = lookAt; 
			m_LookTo = Vector3::Normalize(Vector3::Subtract(m_LookAt, m_Position)); 
			computeViewProjection(); 
		}
		void SetLookTo(const DirectX::XMFLOAT3& lookTo) 
		{ 
			if (Vector3::LengthSquared(lookTo) > 1.1f) 
				CS_ASSERT(false, "LookTo의 크기가 1보다 큽니다");
			m_LookTo = lookTo;
			m_LookAt = Vector3::Add(m_Position, lookTo);
			computeViewProjection(); 
		}
		void SetUp(const DirectX::XMFLOAT3& up) 
		{ 
			if (Vector3::LengthSquared(up) > 1.1f) 
				CS_ASSERT(false, "Up의 크기가 1보다 큽니다"); 
			m_Up = up; 
			computeViewProjection(); 
		}
		void SetViewport(const D3D12_VIEWPORT& viewport) { m_Viewport = viewport; computeViewProjection(); }
		void SetScissorRect(const D3D12_RECT& scissorRect) { m_ScissorRect = scissorRect;}
		void SetFieldOfView(float fieldOfView) { m_FieldOfView = fieldOfView; computeViewProjection(); }
		void SetNearPlane(float nearPlane) { m_NearPlane = nearPlane; computeViewProjection(); }
		void SetFarPlane(float farPlane) { m_FarPlane = farPlane; computeViewProjection(); }
		void SetProjectionMode(ECameraProjectionMode mode) { m_ProjectionMode = mode; computeViewProjection(); }

		void SetMaxCameraPitch(float pitch) { m_MaxPitch = pitch; }

		void RotateRoll(float roll) 
		{ 
			//SetLookTo(Vector3::RotateQuaternion(m_LookTo, Vector4::QuternionRotationAxis(m_LookTo, roll)));
			m_RollPitchYaw.x += roll; 
		}
		void RotatePitch(float pitch) 
		{
			//SetLookTo(Vector3::RotateQuaternion(m_LookTo, Vector4::QuternionRotationAxis(GetRight(), pitch)));
			m_RollPitchYaw.y += pitch; 
		}
		void RotateYaw(float yaw) 
		{
			//SetLookTo(Vector3::RotateQuaternion(m_LookTo, Vector4::QuternionRotationAxis(m_Up, yaw)));
			m_RollPitchYaw.z += yaw;
		}
		void RotateRollPitchYaw(const DirectX::XMFLOAT3 rollPitchYaw) 
		{ 
			SetLookTo(Vector3::RotateQuaternion(m_LookTo, Vector4::QuaternionRollPitchYaw(rollPitchYaw)));
			m_RollPitchYaw = Vector3::Add(m_RollPitchYaw, rollPitchYaw);
		}

		void SetRoll(float roll) 
		{ 
			SetLookTo(Vector3::RotateQuaternion(m_LookTo, 
				Vector4::QuaternionRollPitchYaw({ roll - m_RollPitchYaw.x, 0.0f, 0.0f })));
			m_RollPitchYaw.x = roll;
		}
		void SetPitch(float pitch)
		{
			SetLookTo(Vector3::RotateQuaternion(m_LookTo,
				Vector4::QuaternionRollPitchYaw({ 0.0f, pitch - m_RollPitchYaw.y, 0.0f })));
			m_RollPitchYaw.y = std::clamp(pitch, -(m_MaxPitch), m_MaxPitch);
		}
		void SetYaw(float yaw)
		{
			SetLookTo(Vector3::RotateQuaternion(m_LookTo,
				Vector4::QuaternionRollPitchYaw({ 0.0f, 0.0f, yaw - m_RollPitchYaw.z})));
			m_RollPitchYaw.z = yaw;
		}

		void SetRollPitchYaw(const DirectX::XMFLOAT3 rollPitchYaw) 
		{ 
			SetLookTo(Vector3::RotateQuaternion(m_LookTo, Vector4::QuaternionRollPitchYaw(Vector3::Subtract(rollPitchYaw, m_RollPitchYaw))));
			m_RollPitchYaw = rollPitchYaw;
		}

		void SetSensitivity(float sensitivity) { m_Sensitivity = sensitivity; }


		const DirectX::XMFLOAT3 GetWorldPosition() const { return m_Position; }
		const DirectX::XMFLOAT3 GetLookAt() const { return m_LookAt; }
		const DirectX::XMFLOAT3 GetLookTo() const { return m_LookTo; }
		const DirectX::XMFLOAT3 GetRight() const { return Vector3::Cross(m_Up,m_LookTo); }
		const DirectX::XMFLOAT4X4& GetViewProjection() const { return m_ViewProjection; }
		const DirectX::XMFLOAT4X4& GetInverseViewProjection() const { return m_InverseViewProjection; }
		const D3D12_VIEWPORT& GetViewport() const { return m_Viewport; }
		const D3D12_RECT& GetScissorRect() const { return m_ScissorRect; }

	private:
		void computeViewProjection();
		
	private:
		DirectX::XMFLOAT3 m_Position = { 0, 10.0f, -1.0f };
		DirectX::XMFLOAT3 m_LookAt = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 m_Up = { 0.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT3 m_LookTo = { 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT3 m_RollPitchYaw = { 0.0f, 0.0f, 0.0f };

		DirectX::XMFLOAT4X4 m_ViewProjection = {};
		DirectX::XMFLOAT4X4 m_InverseViewProjection = {};
		D3D12_VIEWPORT m_Viewport = {};
		D3D12_RECT m_ScissorRect = {};

		float m_FieldOfView = 60.0f;
		float m_NearPlane = 0.1f;
		float m_FarPlane = 1000.0f;
		float m_MaxPitch = 360.0f;
		float m_Sensitivity = 0.05f;
		ECameraProjectionMode m_ProjectionMode = ECameraProjectionMode::CPM_Persepective;
	};
}