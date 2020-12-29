#pragma once
#include <d3d12.h>
#include <wrl/client.h>

namespace Crystal {
	class Resource final
	{
	public:
		Resource(D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_DESC bufferResourceDesc);
		Resource(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState);
		Resource(const Resource&) = delete;
		Resource& operator=(const Resource&) = delete;

		void TransResourceState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state);
		void SetData(void* data, size_t memSize);

		ID3D12Resource* GetRaw() const { return m_Resource.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		D3D12_RESOURCE_STATES m_ResourceState = D3D12_RESOURCE_STATE_COMMON;
	};
}
