#pragma once
#include <d3d12.h>
#include "Resource.h"

namespace Crystal {

	class RenderTarget
	{
	public:
		RenderTarget(ID3D12Resource* rtvBuffer);
		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;

		static const ID3D12DescriptorHeap* GetDescriptorHeap() { return s_Heap.Get(); }

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return m_CpuHandle; }
		const D3D12_RENDER_TARGET_BLEND_DESC& GetBlendDesc() const { return m_BlendDesc; }
		const DXGI_FORMAT& GetFormat() const { return m_Format; }
		ID3D12Resource* GetRaw() const { return m_Resource.get()->GetRaw(); }
		
		void TransResourceState(ID3D12GraphicsCommandList2* commandList, D3D12_RESOURCE_STATES state) const { m_Resource->TransResourceState(commandList, state); }

	private:
		static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> s_Heap;
		static UINT s_Count;
		static UINT s_HeapIncrementSize;

		std::unique_ptr<Resource> m_Resource = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandle = {};

		D3D12_RENDER_TARGET_BLEND_DESC m_BlendDesc = {};
		DXGI_FORMAT m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	};

}