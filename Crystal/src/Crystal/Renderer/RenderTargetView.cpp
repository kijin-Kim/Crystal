#include "cspch.h"
#include "Renderer.h"
#include "RenderTargetView.h"


namespace Crystal {

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RenderTargetView::s_Heap = nullptr;
	UINT RenderTargetView::s_Count = 0;
	UINT RenderTargetView::s_HeapIncrementSize = 0;

	RenderTargetView::RenderTargetView(ID3D12Resource* rtvBuffer)
	{
		auto device = Renderer::Instance().GetDevice();

		HRESULT hr = E_FAIL;

		if (!s_Heap)
		{
			s_HeapIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.NumDescriptors = 2;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			rtvHeapDesc.NodeMask = 0;

			//#임시 임시로 디폴트 렌더타겟을 생성
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

			hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&s_Heap));
			CS_ASSERT(SUCCEEDED(hr), "RenderTarget Descriptor Heap을 생성하는데 실패하였습니다.");
		}

		m_Resource = std::make_unique<Resource>(rtvBuffer, D3D12_RESOURCE_STATE_PRESENT);


		D3D12_CPU_DESCRIPTOR_HANDLE cpuStartHandle = s_Heap->GetCPUDescriptorHandleForHeapStart();
		m_CpuHandle.ptr = cpuStartHandle.ptr + (s_Count * s_HeapIncrementSize);
		device->CreateRenderTargetView(rtvBuffer, nullptr, m_CpuHandle);
		s_Count++;


		// #Temp
		m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	}

	RenderTargetView::~RenderTargetView()
	{
		s_Count--;
	}

}