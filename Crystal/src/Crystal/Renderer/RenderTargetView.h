#pragma once
#include <d3d12.h>
#include "Resource.h"

namespace Crystal {

	class RenderTargetView final
	{
	public:
		RenderTargetView(ID3D12Resource* rtvBuffer);
		~RenderTargetView();
		RenderTargetView(const RenderTargetView&) = delete;
		RenderTargetView& operator=(const RenderTargetView&) = delete;

		static const ID3D12DescriptorHeap* GetDescriptorHeap() { return s_Heap.Get(); }

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return m_CpuHandle; }
		const DXGI_FORMAT& GetFormat() const { return m_Format; }
		Resource* GetResource() const { return m_Resource.get(); }


	private:
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> s_Heap;
		static UINT s_Count;
		static UINT s_HeapIncrementSize;

		std::unique_ptr<Resource> m_Resource = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle = {};
		DXGI_FORMAT m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	};

}