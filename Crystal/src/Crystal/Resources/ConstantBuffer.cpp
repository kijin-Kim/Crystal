#include "cspch.h"
#include "ConstantBuffer.h"

#include "Crystal/Renderer/Renderer.h"
#include "DirectXTex/d3dx12.h"

namespace Crystal {
	ConstantBuffer::ConstantBuffer(int size)
	{
		m_Size = (size + (256 - 1)) & ~(256 - 1); // 256바이트를 기준으로 정렬된 크기.

		Renderer& renderer = Renderer::Instance();
		auto device = renderer.GetDevice();
		HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(m_Size),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_Resource));
		CS_FATAL(SUCCEEDED(hr), "Constant Buffer Data Pool을 생성하는데 실패하였습니다.");

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_Resource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = m_Size;

		m_ConstantBufferView = renderer.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		device->CreateConstantBufferView(&cbvDesc, m_ConstantBufferView.GetDescriptorHandle());

		m_Resource->Map(0, nullptr, (void**)&m_CpuBasePtr);
	}
}