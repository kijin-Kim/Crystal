#include "cspch.h"
#include "Descriptor.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	DescriptorHeapManager* DescriptorObject::s_DescriptorHeapManager = nullptr;
	DescriptorHeapManager::DescriptorHeapManager()
	{
		/*TODO : Page Allocator*/
		/*WORK IN PROGRESS*/
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavDesc = {};
		cbvSrvUavDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvSrvUavDesc.NumDescriptors = 200;
		cbvSrvUavDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		cbvSrvUavDesc.NodeMask = 0;
		HRESULT hr = device->CreateDescriptorHeap(&cbvSrvUavDesc, IID_PPV_ARGS(&m_CBVSRVUAVHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV,SRV,UAV Descriptor Heap을 생성하는데 실패하였습니다.");
		m_CBVSRVUAVHeapIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
		rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDesc.NumDescriptors = 30;
		rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDesc.NodeMask = 0;
		hr = device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_RTVHeap));
		CS_FATAL(SUCCEEDED(hr), "RTV Descriptor Heap을 생성하는데 실패하였습니다.");
		m_RTVHeapIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
		dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvDesc.NumDescriptors = 30;
		dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvDesc.NodeMask = 0;
		hr = device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&m_DSVHeap));
		CS_FATAL(SUCCEEDED(hr), "RTV Descriptor Heap을 생성하는데 실패하였습니다.");
		m_DSVHeapIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	}



	Descriptor DescriptorHeapManager::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, UINT descriptorCount)
	{
		switch (descriptorHeapType)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		{
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_CBVSRVUAVHeap->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += (m_CBVSRVUAVHeapIncrementSize * m_CBVSRVUAVHeapDescriptorCount++);
			return Descriptor(cpuHandle);
		}
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		{
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += (m_RTVHeapIncrementSize * m_RTVHeapDescriptorCount++);
			return Descriptor(cpuHandle);
		}
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		{
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += (m_DSVHeapIncrementSize * m_DSVHeapDescriptorCount++);
			return Descriptor(cpuHandle);
		}
		default:
			CS_FATAL(false, "아직 지원하지 않는 타입입니다.");
			return Descriptor();
		}
	}

}