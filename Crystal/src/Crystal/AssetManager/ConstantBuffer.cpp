#include "cspch.h"
#include "ConstantBuffer.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {
	/* ����� m_BufferSize ( 256 / 512 / 1024 Bytes) �� Constant Buffer View�� �̸� �ٸ� CBV_SRV_UAV ���� ����� �����ϴ�.
	�̷��� Constant Buffer View�� ���ҽ� ���ε� ������ ���� ���������ο� ���ε��� CBV_SRV_UAV ����
	����(Copy Descriptor)�Ǿ� ���������� ���ε� �˴ϴ�. Constant Buffer View�� �̸� ����� ���� ������ ������ ��������
	Constant Buffer View�� �����ϹǷ� ������ 256, 512, 1024 Byte�� Constant Buffer View�� ����� ConstantBufferManager�� �ʿ��մϴ�.*/
	ConstantBufferPool::ConstantBufferPool(int alignmentSize, int maxCbvCount)
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = m_MaxCbvCount;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_CbvHeap));
		CS_ASSERT(SUCCEEDED(hr), "CBV Pool�� �����ϴµ� �����Ͽ����ϴ�.");

		m_CbvHeapCpuHandle = m_CbvHeap->GetCPUDescriptorHandleForHeapStart();

		hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(m_BufferAlignedSize * m_MaxCbvCount),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ConstantBufferData));
		CS_ASSERT(SUCCEEDED(hr), "Constant Buffer Data Pool�� �����ϴµ� �����Ͽ����ϴ�.");

		m_ConstantBufferData->Map(0, nullptr, (void**)&m_CpuBasePtr);

		/* m_BufferSize�� Constant Buffer View�� m_CbvCount��ŭ ����ϴ�.*/
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

	/* �̸� ����� ���� Constant Buffer View�� ���ǰ� ���� ���� Constant Buffer View�� �����մϴ�.*/
	/* TODO : Constant Buffer View�� ��� ���θ� �Ǵ��ϰ�, ������ �ʴ� Constant Buffer View�� �����޴� ����.*/
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
		int alignedSize = (size + (256 - 1)) & ~(256 - 1); // 256����Ʈ�� �������� ���ĵ� ũ��.
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
			CS_ASSERT(false, "%d�������� ConstantBuffer�� ���� �������� �ʽ��ϴ�.");
			break;
		}

		return cBuffer;
	}
}