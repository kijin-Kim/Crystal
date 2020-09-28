#include "cspch.h"
#include "ConstantBufferManager.h"
#include "Crystal/Renderer/Renderer.h"


namespace Crystal {


	ConstantBufferManager::ConstantBufferManager()
	{
		auto device = Renderer::Get().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = m_CbvCount;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_CbvPool));
		CS_ASSERT(SUCCEEDED(hr), "CBV Pool을 생성하는데 실패하였습니다.");

		m_CbvPoolCpuHandle = m_CbvPool->GetCPUDescriptorHandleForHeapStart();

		hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(m_BufferSize * m_CbvCount),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ConstantBufferDataPool));
		CS_ASSERT(SUCCEEDED(hr), "Constant Buffer Data Pool을 생성하는데 실패하였습니다.");

		m_ConstantBufferDataPool->Map(0, nullptr, (void**)&m_CpuBasePtr);
	}

	Crystal::ConstantBuffer ConstantBufferManager::CreateConstantBuffer(UINT dataSize /*= 256*/)
	{
		// Need Some Validation

		auto device = Renderer::Get().GetDevice();

		UINT cpuOffsetByte = m_CpuOffsetEnd;

		ConstantBuffer cBuffer = {};
		cBuffer.m_CpuDescriptorHandle = m_CbvPoolCpuHandle;
		cBuffer.m_GpuVirtualAddress = (D3D12_GPU_VIRTUAL_ADDRESS)((char*)m_ConstantBufferDataPool->GetGPUVirtualAddress() + cpuOffsetByte);
		cBuffer.m_CpuDataOffsetPtr = ((char*)m_CpuBasePtr + cpuOffsetByte);
		cBuffer.m_Size = 256;

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = (D3D12_GPU_VIRTUAL_ADDRESS)((char*)m_ConstantBufferDataPool->GetGPUVirtualAddress() + cpuOffsetByte);
		cbvDesc.SizeInBytes = dataSize;
		device->CreateConstantBufferView(&cbvDesc, m_CbvPoolCpuHandle);
	
		m_CbvPoolCpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		m_CpuOffsetEnd += dataSize;

		return cBuffer;
	}

}