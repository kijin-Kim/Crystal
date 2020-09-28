#include "cspch.h"
#include "Camera.h"

namespace Crystal {


	Camera::Camera(int width, int height)
	{
		// #DirectX Viewport
		m_Viewport.TopLeftX = 0;
		m_Viewport.TopLeftY = 0;
		m_Viewport.Width = (FLOAT)width;
		m_Viewport.Height = (FLOAT)height;
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;

		// #DirectX Scissor Rect
		m_ScissorRect.left = 0;
		m_ScissorRect.top = 0;
		m_ScissorRect.right = width;
		m_ScissorRect.bottom = height;

		XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
		XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());


		generateView();
		generateProjection();
	}

	void Camera::OnEvent(Event& event)
	{
		//PlayerInputComponent->BindAxis('W', this, &Camera::MoveForward);
		
	}

	void Camera::generateView()
	{
		DirectX::XMVECTOR position = XMLoadFloat3(&m_Position);
		DirectX::XMVECTOR lookAt = XMLoadFloat3(&m_LookAt);
		if (DirectX::XMVector3Equal(position, lookAt))
		{
			CS_ERROR("카메라의 위치와 바라보는 위치가 같습니다");
			return;
		}

		XMStoreFloat4x4(&m_View, DirectX::XMMatrixLookAtLH(position, lookAt, DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
		XMStoreFloat4x4(&m_View, XMMatrixTranspose(XMLoadFloat4x4(&m_View)));
	}

	void Camera::generateProjection()
	{
		XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.0f), (float)m_Viewport.Width / (float)m_Viewport.Height, 0.1f, 10000.0f));
		XMStoreFloat4x4(&m_Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_Projection)));
	}

}