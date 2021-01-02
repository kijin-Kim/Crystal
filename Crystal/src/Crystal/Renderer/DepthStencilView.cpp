#include "cspch.h"
#include "DepthStencilView.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DepthStencilView::s_Heap = nullptr;
	UINT DepthStencilView::s_Count = 0;
	UINT DepthStencilView::s_HeapIncrementSize = 0;

	DepthStencilView::DepthStencilView(ID3D12Resource* rtvBuffer)
	{
		auto device = Renderer::Instance().GetDevice();

		HRESULT hr = E_FAIL;
		//#Temp
		if (!s_Heap)
		{
			// #DirectX Depth / Stencil View heap desrciption
			D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
			dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvHeapDesc.NumDescriptors = 1;
			dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			dsvHeapDesc.NodeMask = 0;

			// #DirectX Create Depth / Stencil View heap
			hr = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&s_Heap));
			CS_ASSERT(SUCCEEDED(hr), "DepthStencil Descriptor Heap을 생성하는데 실패하였습니다");
		}

		m_Resource = std::make_unique<Resource>(rtvBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE);

		

		D3D12_CPU_DESCRIPTOR_HANDLE cpuStartHandle = s_Heap->GetCPUDescriptorHandleForHeapStart();
		m_CpuHandle.ptr = cpuStartHandle.ptr + (s_Count * s_HeapIncrementSize);
		// #DirectX Create Depth Stencil View
		device->CreateDepthStencilView(m_Resource->GetRaw(), nullptr, m_CpuHandle);
		s_Count++;

		// #DirectX Depth / Stencil description
		m_DepthStencilDesc.DepthEnable = true;
		m_DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		m_DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		m_DepthStencilDesc.StencilEnable = false;
		m_DepthStencilDesc.StencilReadMask = 0x00;
		m_DepthStencilDesc.StencilWriteMask = 0x00;
		m_DepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		m_DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		m_DepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		m_DepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
		m_DepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		m_DepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		m_DepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		m_DepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	}

	DepthStencilView::~DepthStencilView()
	{
		s_Count--;
	}
}