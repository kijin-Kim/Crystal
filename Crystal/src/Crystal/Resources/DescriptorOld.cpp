#include "cspch.h"
#include "DescriptorOld.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {
	DescriptorHeapManagerOld* DescriptorObjectOld::s_DescriptorHeapManager = nullptr;
	DescriptorHeapManagerOld::DescriptorHeapManagerOld()
	{
		/*TODO : Page Allocator*/
		/*WORK IN PROGRESS*/
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavDesc = {};
		cbvSrvUavDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvSrvUavDesc.NumDescriptors = m_MaxCBVSRVUAVHeapDescriptorCount;
		cbvSrvUavDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		cbvSrvUavDesc.NodeMask = 0;
		HRESULT hr = device->CreateDescriptorHeap(&cbvSrvUavDesc, IID_PPV_ARGS(&m_CBVSRVUAVHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV,SRV,UAV Descriptor Heap을 생성하는데 실패하였습니다.");
		m_CBVSRVUAVHeapIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
		rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDesc.NumDescriptors = m_MaxRTVHeapDescriptorCount;
		rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDesc.NodeMask = 0;
		hr = device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_RTVHeap));
		CS_FATAL(SUCCEEDED(hr), "RTV Descriptor Heap을 생성하는데 실패하였습니다.");
		m_RTVHeapIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
		dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvDesc.NumDescriptors = m_MaxDSVHeapDescriptorCount;
		dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvDesc.NodeMask = 0;
		hr = device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&m_DSVHeap));
		CS_FATAL(SUCCEEDED(hr), "RTV Descriptor Heap을 생성하는데 실패하였습니다.");
		m_DSVHeapIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	DescriptorOld DescriptorHeapManagerOld::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType)
	{
		switch (descriptorHeapType)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		{
			if (m_CBVSRVUAVHeapDescriptorCount > m_MaxCBVSRVUAVHeapDescriptorCount)
				CS_FATAL(false, "최대 Descriptor 생성 횟수를 초과하였습니다");
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_CBVSRVUAVHeap->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += (m_CBVSRVUAVHeapIncrementSize * m_CBVSRVUAVHeapDescriptorCount++);
			return DescriptorOld(cpuHandle);
		}
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		{
			if (m_RTVHeapDescriptorCount > m_MaxRTVHeapDescriptorCount)
				CS_FATAL(false, "최대 Descriptor 생성 횟수를 초과하였습니다");

			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += (m_RTVHeapIncrementSize * m_RTVHeapDescriptorCount++);
			return DescriptorOld(cpuHandle);
		}
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		{
			if (m_DSVHeapDescriptorCount > m_MaxDSVHeapDescriptorCount)
				CS_FATAL(false, "최대 Descriptor 생성 횟수를 초과하였습니다");
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += (m_DSVHeapIncrementSize * m_DSVHeapDescriptorCount++);
			return DescriptorOld(cpuHandle);
		}
		default:
			CS_FATAL(false, "아직 지원하지 않는 타입입니다.");
			return DescriptorOld();
		}
	}
}