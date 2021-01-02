#include "cspch.h"
#include "Resource.h"
#include "Renderer.h"

namespace Crystal {
	Resource::Resource(D3D12_RESOURCE_STATES initialState, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_DESC bufferResourceDesc) :
		m_ResourceState(initialState)
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = heapType;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProperties.CreationNodeMask = 0;
		heapProperties.VisibleNodeMask = 0;

		HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &bufferResourceDesc,
			initialState, nullptr, IID_PPV_ARGS(&m_Resource));
		CS_ASSERT(SUCCEEDED(hr), "리소스를 만드는데 실패하였습니다");
	}

	Resource::Resource(ID3D12Resource* resource, D3D12_RESOURCE_STATES initialState) :
		m_Resource(resource),
		m_ResourceState(initialState)
	{
	}

}