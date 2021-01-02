#pragma once
#include <d3d12.h>
#include <memory>
#include <wrl/client.h>
#include "Resource.h"

namespace Crystal {
	class DepthStencilView final
	{
	public:
		DepthStencilView(ID3D12Resource* rtvBuffer);
		~DepthStencilView();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return m_CpuHandle; }
		const DXGI_FORMAT& GetFormat() const { return m_Format; }
		const D3D12_DEPTH_STENCIL_DESC GetDesc() const { return m_DepthStencilDesc; }

	private:
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> s_Heap;
		static UINT s_Count;
		static UINT s_HeapIncrementSize;

		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle = {};

		std::unique_ptr<Resource> m_Resource = nullptr;

		D3D12_DEPTH_STENCIL_DESC m_DepthStencilDesc = {};
		DXGI_FORMAT m_Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	};
}
