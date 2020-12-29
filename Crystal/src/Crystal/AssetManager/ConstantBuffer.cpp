#include "cspch.h"
#include "ConstantBuffer.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {
	/* 사이즈가 m_BufferSize ( 256 / 512 / 1024 Bytes) 인 Constant Buffer View를 미리 다른 CBV_SRV_UAV 힙에 만들어 놓습니다.
	이러한 Constant Buffer View는 리소스 바인딩 과정시 현재 파이프라인에 바인딩된 CBV_SRV_UAV 힙에
	복사(Copy Descriptor)되어 최종적으로 바인딩 됩니다. Constant Buffer View를 미리 만들어 놓기 때문에 고정된 사이즈의
	Constant Buffer View만 제공하므로 각각의 256, 512, 1024 Byte의 Constant Buffer View를 만드는 ConstantBufferManager가 필요합니다.*/
	ConstantBufferPool::ConstantBufferPool(int alignmentSize, int maxCbvCount)
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = m_MaxCbvCount;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_CbvHeap));
		CS_ASSERT(SUCCEEDED(hr), "CBV Pool을 생성하는데 실패하였습니다.");

		m_CbvHeapCpuHandle = m_CbvHeap->GetCPUDescriptorHandleForHeapStart();

		hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(m_BufferAlignedSize * m_MaxCbvCount),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ConstantBufferData));
		CS_ASSERT(SUCCEEDED(hr), "Constant Buffer Data Pool을 생성하는데 실패하였습니다.");

		m_ConstantBufferData->Map(0, nullptr, (void**)&m_CpuBasePtr);

		/* m_BufferSize의 Constant Buffer View를 m_CbvCount만큼 만듭니다.*/
		UINT cpuOffsetByte = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE cbvPoolCpuHandle = m_CbvHeap->GetCPUDescriptorHandleForHeapStart();
		for (unsigned int i = 0; i < m_MaxCbvCount; i++)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
			cbvDesc.BufferLocation = (D3D12_GPU_VIRTUAL_ADDRESS)((char*)m_ConstantBufferData->GetGPUVirtualAddress() + cpuOffsetByte);
			cbvDesc.SizeInBytes = m_BufferAlignedSize;
			device->CreateConstantBufferView(&cbvDesc, cbvPoolCpuHandle);
			cbvPoolCpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			cpuOffsetByte += m_BufferAlignedSize;
		}
	}

	/* 미리 만들어 놓은 Constant Buffer View중 사용되고 있지 않은 Constant Buffer View를 리턴합니다.*/
	/* TODO : Constant Buffer View의 사용 여부를 판단하고, 사용되지 않는 Constant Buffer View를 돌려받는 로직.*/
	ConstantBuffer ConstantBufferPool::CreateConstantBuffer()
	{
		auto device = Renderer::Instance().GetDevice();

		ConstantBuffer cBuffer = {};
		cBuffer.m_CpuDescriptorHandle = m_CbvHeapCpuHandle;
		cBuffer.m_GpuVirtualAddress = (D3D12_GPU_VIRTUAL_ADDRESS)((char*)m_ConstantBufferData->GetGPUVirtualAddress() + m_CpuOffsetEnd);
		cBuffer.m_CpuDataOffsetPtr = ((char*)m_CpuBasePtr + m_CpuOffsetEnd);
		cBuffer.m_Size = m_BufferAlignedSize;

		m_CbvHeapCpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		m_CpuOffsetEnd += m_BufferAlignedSize;
		return cBuffer;
	}

	ConstantBufferPoolManager::ConstantBufferPoolManager()
	{
		m_256AlignedBufferPool = ConstantBufferPool(256, 1000);
		m_512AlignedBufferPool = ConstantBufferPool(512, 1000);
		m_1024AlignedBufferPool = ConstantBufferPool(1024, 1000);
	}

	ConstantBuffer ConstantBufferPoolManager::GetConstantBuffer(int size)
	{
		int alignedSize = (size + (256 - 1)) & ~(256 - 1); // 256바이트를 기준으로 정렬된 크기.
		ConstantBuffer cBuffer = {};
		switch (alignedSize)
		{
		case 256:
			cBuffer = m_256AlignedBufferPool.CreateConstantBuffer();
			break;
		case 512:
			cBuffer = m_512AlignedBufferPool.CreateConstantBuffer();
			break;
		case 1024:
			cBuffer = m_1024AlignedBufferPool.CreateConstantBuffer();
			break;
		default:
			CS_ASSERT(false, "%d사이즈의 ConstantBuffer는 현재 지원하지 않습니다.");
			break;
		}

		return cBuffer;
	}
}