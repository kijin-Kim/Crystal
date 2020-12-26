#include "cspch.h"
#include "DepthStencil.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DepthStencil::s_Heap = nullptr;
	UINT DepthStencil::s_Count = 0;
	UINT DepthStencil::s_HeapIncrementSize = 0;

	DepthStencil::DepthStencil(int width, int height)
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
		

		// #DirectX Depth / Stencil View heap properties 
		D3D12_HEAP_PROPERTIES dsvHeapProperties = {};
		dsvHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		dsvHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		dsvHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		dsvHeapProperties.CreationNodeMask = 1;
		dsvHeapProperties.VisibleNodeMask = 1;


		// #DirectX Depth / Stencil View description
		D3D12_RESOURCE_DESC dsvResourcDesc = {};
		dsvResourcDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsvResourcDesc.Alignment = 0;
		dsvResourcDesc.Width = width;
		dsvResourcDesc.Height = height;
		dsvResourcDesc.DepthOrArraySize = 1;
		dsvResourcDesc.MipLevels = 0;
		dsvResourcDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvResourcDesc.SampleDesc.Count = 1;
		dsvResourcDesc.SampleDesc.Quality = 0;
		dsvResourcDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsvResourcDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


		// #DirectX Depth / Stencil View Clear Value
		D3D12_CLEAR_VALUE dsvClearValue = {};
		dsvClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvClearValue.DepthStencil.Depth = 1.0f;
		dsvClearValue.DepthStencil.Stencil = 0;

		// #DirectX Create Depth / Stencil Buffer
		hr = device->CreateCommittedResource(&dsvHeapProperties, D3D12_HEAP_FLAG_NONE, &dsvResourcDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, &dsvClearValue, IID_PPV_ARGS(&m_Buffer));
		CS_ASSERT(SUCCEEDED(hr), "DepthStencil 리소스를 만드는데 실패하였습니다");

		D3D12_CPU_DESCRIPTOR_HANDLE cpuStartHandle = s_Heap->GetCPUDescriptorHandleForHeapStart();
		m_CpuHandle.ptr = cpuStartHandle.ptr + (s_Count * s_HeapIncrementSize);
		// #DirectX Create Depth Stencil View
		device->CreateDepthStencilView(m_Buffer.Get(), nullptr, m_CpuHandle);
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

	DepthStencil::~DepthStencil()
	{
		s_Count--;
	}

}