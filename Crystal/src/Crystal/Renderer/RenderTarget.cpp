#include "cspch.h"
#include "Renderer.h"
#include "RenderTarget.h"


namespace Crystal {

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> RenderTarget::s_Heap = nullptr;
	UINT RenderTarget::s_Count = 0;
	UINT RenderTarget::s_HeapIncrementSize = 0;

	RenderTarget::RenderTarget(ID3D12Resource* rtvBuffer)
	{
		auto device = Renderer::Get().GetDevice();

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


		m_BlendDesc.BlendEnable = false;
		m_BlendDesc.LogicOpEnable = false;
		m_BlendDesc.SrcBlend = D3D12_BLEND_ONE;
		m_BlendDesc.DestBlend = D3D12_BLEND_ZERO;
		m_BlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		m_BlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		m_BlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		m_BlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		m_BlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		m_BlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		// #Temp
		m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	}

	RenderTarget::~RenderTarget()
	{
		s_Count--;
	}

}