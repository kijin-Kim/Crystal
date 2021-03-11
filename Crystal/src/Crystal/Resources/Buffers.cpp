#include "cspch.h"
#include "Buffers.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Renderer/CommandList.h"

namespace Crystal {
	VertexBuffer::VertexBuffer(void* data, UINT strideInByte, UINT count)
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_RESOURCE_DESC bufferResourceDesc = {};
		bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferResourceDesc.Alignment = 0;
		bufferResourceDesc.Width = static_cast<UINT64>(strideInByte) * count;
		bufferResourceDesc.Height = 1;
		bufferResourceDesc.DepthOrArraySize = 1;
		bufferResourceDesc.MipLevels = 1;
		bufferResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferResourceDesc.SampleDesc.Count = 1;
		bufferResourceDesc.SampleDesc.Quality = 0;
		bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &bufferResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_DestinationBuffer));
		CS_FATAL(SUCCEEDED(hr), "목표 버퍼를 생성하는데 실패하였습니다");

		if (data)
		{
			HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &bufferResourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_IntermediateBuffer));
			CS_FATAL(SUCCEEDED(hr), "중간 버퍼를 생성하는데 실패하였습니다");

			auto commandQueue = Renderer::Instance().GetCommandQueue();
			auto cmdList = commandQueue->GetCommandList();

			D3D12_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pData = data;
			subResourceData.RowPitch = static_cast<UINT64>(strideInByte) * count;
			subResourceData.SlicePitch = subResourceData.RowPitch;
			UpdateSubresources(cmdList.Get(), m_DestinationBuffer.Get(), m_IntermediateBuffer.Get(), 0, 0, 1, &subResourceData);

			commandQueue->Execute(cmdList);
			commandQueue->Flush();
		}

		m_VertxBufferView.BufferLocation = m_DestinationBuffer->GetGPUVirtualAddress();
		m_VertxBufferView.StrideInBytes = strideInByte;
		m_VertxBufferView.SizeInBytes = strideInByte * count;
	}

	IndexBuffer::IndexBuffer(void* data, UINT sizeInbyte, UINT count) :
		m_Count(count)
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_RESOURCE_DESC bufferResourceDesc = {};
		bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferResourceDesc.Alignment = 0;
		bufferResourceDesc.Width = sizeInbyte;
		bufferResourceDesc.Height = 1;
		bufferResourceDesc.DepthOrArraySize = 1;
		bufferResourceDesc.MipLevels = 1;
		bufferResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferResourceDesc.SampleDesc.Count = 1;
		bufferResourceDesc.SampleDesc.Quality = 0;
		bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &bufferResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_DestinationBuffer));
		CS_FATAL(SUCCEEDED(hr), "목표 버퍼를 생성하는데 실패하였습니다");

		if (data)
		{
			HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &bufferResourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_IntermediateBuffer));
			CS_FATAL(SUCCEEDED(hr), "중간 버퍼를 생성하는데 실패하였습니다");

			auto commandQueue = Renderer::Instance().GetCommandQueue();
			auto cmdList = commandQueue->GetCommandList();

			D3D12_SUBRESOURCE_DATA subResourceData = {};
			subResourceData.pData = data;
			subResourceData.RowPitch = sizeInbyte;
			subResourceData.SlicePitch = subResourceData.RowPitch;
			UpdateSubresources(cmdList.Get(), m_DestinationBuffer.Get(), m_IntermediateBuffer.Get(), 0, 0, 1, &subResourceData);

			commandQueue->Execute(cmdList);
			commandQueue->Flush();
		}

		m_IndexBufferView.BufferLocation = m_DestinationBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.SizeInBytes = sizeInbyte;
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}
}