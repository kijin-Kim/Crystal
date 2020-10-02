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

	void Resource::TransResourceState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES state)
	{
		if (m_ResourceState == state)
			return;

		D3D12_RESOURCE_BARRIER vertexBufferBarrier = {};
		vertexBufferBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		vertexBufferBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		vertexBufferBarrier.Transition.pResource = m_Resource.Get();
		vertexBufferBarrier.Transition.StateBefore = m_ResourceState;
		vertexBufferBarrier.Transition.StateAfter = state;
		vertexBufferBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		commandList->ResourceBarrier(1, &vertexBufferBarrier);

		m_ResourceState = state;
	}

	void Resource::SetData(void* data, size_t memSize)
	{
		D3D12_RANGE readRange = { 0,0 };
		UINT8* bufferBegin = nullptr;
		m_Resource->Map(0, &readRange, (void**)&bufferBegin);
		memcpy(bufferBegin, data, memSize);
		m_Resource->Unmap(0, nullptr);
	}

}