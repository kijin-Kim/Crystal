#pragma once
#include <DirectXMath.h>
#include "Crystal/Core/Events.h"



namespace Crystal {

	// �������� �ػ󵵰� ����� �� ī�޶� �ٽ� �������Ѵ�.
	class Camera
	{
	public:
		Camera(int width, int height);
		~Camera() {};

		void OnEvent(Event& event);

		void SetPosition(const DirectX::XMFLOAT3& position) { m_Position = position; generateViewProjection(); }
		void SetLookAt(const DirectX::XMFLOAT3& lookAt) { m_LookAt = lookAt; generateViewProjection(); }
		void SetViewport(const D3D12_VIEWPORT& viewport) { m_Viewport = viewport; }
		void SetScissorRect(const D3D12_RECT& scissorRect) { m_ScissorRect = scissorRect; }
		
		const DirectX::XMFLOAT3 GetWorldPosition() const{ return m_Position; }
		const DirectX::XMFLOAT4X4& GetViewProjection() const { return m_ViewProjMat; }
		const D3D12_VIEWPORT& GetViewport() const { return m_Viewport; }
		const D3D12_RECT& GetScissorRect() const { return m_ScissorRect; }
		
		
	private:
		void generateViewProjection();
		
		
	private:
		DirectX::XMFLOAT3 m_Position = { 0, 10.0f, -1.0f};
		DirectX::XMFLOAT3 m_LookAt = { 0, 0, 0};
		DirectX::XMFLOAT4X4 m_ViewProjMat = {};
		D3D12_VIEWPORT m_Viewport = {};
		D3D12_RECT m_ScissorRect = {};
	};
}
