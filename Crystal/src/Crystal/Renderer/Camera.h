#pragma once
#include <DirectXMath.h>



namespace Crystal {

	// 윈도우의 해상도가 변경될 시 카메라를 다시 만들어야한다.
	class Camera final
	{
	public:
		Camera(int width, int height);

		void SetPosition(const DirectX::XMFLOAT3& position) { m_Position = position; generateViewProjection(); }
		void SetLookAt(const DirectX::XMFLOAT3& lookAt) { m_LookAt = lookAt; generateViewProjection(); }
		void SetViewport(const D3D12_VIEWPORT& viewport) { m_Viewport = viewport; generateViewProjection(); }
		void SetScissorRect(const D3D12_RECT& scissorRect) { m_ScissorRect = scissorRect; }
		
		const DirectX::XMFLOAT3 GetWorldPosition() const{ return m_Position; }
		const DirectX::XMFLOAT4X4& GetViewProjection() const { return m_ViewProjection; }
		const DirectX::XMFLOAT4X4& GetInverseViewProjection() const { return m_InverseViewProjection; }
		const D3D12_VIEWPORT& GetViewport() const { return m_Viewport; }
		const D3D12_RECT& GetScissorRect() const { return m_ScissorRect; }
		
		
	private:
		void generateViewProjection();
	private:
		DirectX::XMFLOAT3 m_Position = { 0, 10.0f, -1.0f};
		DirectX::XMFLOAT3 m_LookAt = { 0, 0, 0};
		DirectX::XMFLOAT4X4 m_ViewProjection = {};
		DirectX::XMFLOAT4X4 m_InverseViewProjection = {};
		D3D12_VIEWPORT m_Viewport = {};
		D3D12_RECT m_ScissorRect = {};
	};
}
