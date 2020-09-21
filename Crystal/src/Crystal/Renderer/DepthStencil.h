#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl/client.h>

namespace Crystal {
	
	class DepthStencil final
	{
	public:
		DepthStencil(int width, int height);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return m_CpuHandle; }
		const DXGI_FORMAT& GetFormat() const { return m_Format; }
		const D3D12_DEPTH_STENCIL_DESC GetDesc() const { return m_DepthStencilDesc; }

	private:
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> s_Heap;
		static UINT s_Count;
		static UINT s_HeapIncrementSize;

		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle = {};

		Microsoft::WRL::ComPtr<ID3D12Resource> m_Buffer = nullptr;

		D3D12_DEPTH_STENCIL_DESC m_DepthStencilDesc = {};
		DXGI_FORMAT m_Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	};
}
