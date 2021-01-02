#pragma once
#include <d3d12.h>
#include <wrl/client.h>

namespace Crystal {
	
	/*���ҽ��� ���¸� �߰��ϴ� �����̳� �Դϴ�.*/
	class Resource final
	{
	public:
		Resource(D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_DESC bufferResourceDesc);
		Resource(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState);
		Resource(const Resource&) = delete;
		Resource& operator=(const Resource&) = delete;

		ID3D12Resource* GetRaw() const { return m_Resource.Get(); }
		D3D12_RESOURCE_STATES GetResourceState() const { return m_ResourceState; }
		void SetResourceState(D3D12_RESOURCE_STATES state) { m_ResourceState = state; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		D3D12_RESOURCE_STATES m_ResourceState = D3D12_RESOURCE_STATE_COMMON;

		
	};
}
