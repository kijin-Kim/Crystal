#include "cspch.h"
#include "CameraComponent.h"

namespace Crystal {
	void CameraComponent::computeViewProjection()
	{
		switch (m_ProjectionMode)
		{
		case ECameraProjectionMode::CPM_Persepective:
			m_ViewProjection = Matrix4x4::Multiply(Matrix4x4::LookTo(m_Position, GetForward(), GetUp()), 
				Matrix4x4::Perspective(m_FieldOfView, (float)m_Viewport.Width / (float)m_Viewport.Height, m_NearPlane, m_FarPlane));
			break;
		case ECameraProjectionMode::CPM_Orthographic:
			break;
		default:
			CS_FATAL(false, "예상치 못한 Camera Projection Mode.");
			break;
		}
	}

}