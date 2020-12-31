#include "cspch.h"
#include "CameraComponent.h"

namespace Crystal {
	void CameraComponent::computeViewProjection()
	{
		if (Vector3::Equal(m_Position, m_LookAt))
		{
			CS_ERROR("카메라의 위치와 바라보는 위치가 같습니다");
			return;
		}

	
		switch (m_ProjectionMode)
		{
		case ECameraProjectionMode::CPM_Persepective:
			m_ViewProjection = Matrix4x4::Multiply(Matrix4x4::LookTo(m_Position, m_LookTo, m_Up), 
				Matrix4x4::Perspective(m_FieldOfView, (float)m_Viewport.Width / (float)m_Viewport.Height, m_NearPlane, m_FarPlane));
			m_ViewProjection = Matrix4x4::Transpose(m_ViewProjection);
			
			/*m_ViewProjection = Matrix4x4::Multiply(Matrix4x4::LookTo(m_Position, m_LookTo, m_Up), 
				Matrix4x4::RotationQuaternion(Vector3::QuaternionRollPitchYaw(m_RollPitchYaw)));
			m_ViewProjection = Matrix4x4::Multiply(m_ViewProjection, Matrix4x4::Perspective(m_FieldOfView, (float)m_Viewport.Width / (float)m_Viewport.Height, m_NearPlane, m_FarPlane));
			m_ViewProjection = Matrix4x4::Transpose(m_ViewProjection);*/
			break;
		case ECameraProjectionMode::CPM_Orthographic:
			m_ViewProjection = Matrix4x4::Multiply(Matrix4x4::LookTo(m_Position, m_LookTo, m_Up), 
				Matrix4x4::OrthoGraphic(m_Viewport.Width, m_Viewport.Height, m_NearPlane, m_FarPlane));
			m_ViewProjection = Matrix4x4::Transpose(m_ViewProjection);
			break;
		default:
			CS_ASSERT(false, "예상치 못한 Camera Projection Mode.");
			break;
		}
		m_InverseViewProjection = Matrix4x4::Inverse(m_ViewProjection);
	}

}