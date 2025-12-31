#include "cspch.h"
#include "Buffer.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Math/Math.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "DirectXTex/d3dx12.h"

namespace Crystal {
	Buffer::Buffer(void* data, UINT sizeInByte, UINT count, bool bAlign256, bool bAsDynamic)
		: m_ElementCount(count),
		m_Size(sizeInByte),
		m_bIsDynamic(bAsDynamic)
	{
		
		if (bAlign256)
		{
			m_Size = Align(m_Size, 256);
		}

		auto device = Device::Instance().GetD3DDevice();


		D3D12_RESOURCE_DESC bufferResourceDesc = {};
		bufferResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferResourceDesc.Alignment = 0;
		bufferResourceDesc.Width = m_Size;
		bufferResourceDesc.Height = 1;
		bufferResourceDesc.DepthOrArraySize = 1;
		bufferResourceDesc.MipLevels = 1;
		bufferResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferResourceDesc.SampleDesc.Count = 1;
		bufferResourceDesc.SampleDesc.Quality = 0;
		bufferResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (bAsDynamic)
		{

			HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &bufferResourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_Resource));
			// CS_FATAL(SUCCEEDED(hr), "목표 버퍼를 생성하는데 실패하였습니다");

			m_Resource->Map(0, nullptr, (void**)&m_CpuBasePtr);

			if(data)
				SetData(data, 0, m_Size);
		}
		else
		{
			HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &bufferResourceDesc,
				D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_Resource));
			// CS_FATAL(SUCCEEDED(hr), "목표 버퍼를 생성하는데 실패하였습니다");

			if (data)
			{
				Microsoft::WRL::ComPtr<ID3D12Resource> intermediateBuffer = nullptr;

				HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &bufferResourceDesc,
					D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&intermediateBuffer));
				// CS_FATAL(SUCCEEDED(hr), "중간 버퍼를 생성하는데 실패하였습니다");

				//auto commandQueue = Renderer::Instance().GetCommandQueue();
				auto commandQueue = Device::Instance().GetCommandQueue();
				auto cmdList = commandQueue->GetCommandList();

				D3D12_SUBRESOURCE_DATA subResourceData = {};
				subResourceData.pData = data;
				subResourceData.RowPitch = sizeInByte;
				subResourceData.SlicePitch = subResourceData.RowPitch;
				UpdateSubresources(cmdList.Get(), m_Resource.Get(), intermediateBuffer.Get(), 0, 0, 1, &subResourceData);

				commandQueue->Execute(cmdList);
				commandQueue->Flush();
			}
		}
	}

	void Buffer::SetData(void* data, uint32_t offset, uint32_t size)
	{
		memcpy(m_CpuBasePtr + offset, data, size);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Buffer::AsConstantBufferView()
	{
		if (m_ConstantBufferView.IsNull())
		{
			CreateConstantBufferView();
		}
		return m_ConstantBufferView.GetDescriptorHandle();
	}

	D3D12_VERTEX_BUFFER_VIEW Buffer::AsVertexBufferView(UINT stride) const
	{
		return { m_Resource->GetGPUVirtualAddress(), m_Size, stride };
	}

	D3D12_INDEX_BUFFER_VIEW Buffer::AsIndexBufferView() const
	{
		return {m_Resource->GetGPUVirtualAddress(), m_Size, DXGI_FORMAT_R32_UINT};
	}

	D3D12_GPU_VIRTUAL_ADDRESS Buffer::GetGPUVirtualAddress() const
	{
		return m_Resource->GetGPUVirtualAddress();
	}

	void Buffer::CreateConstantBufferView()
	{
		if (!m_ConstantBufferView.IsNull())
		{
			// CS_WARN("현재 타입의 리소스뷰가 이미 존재합니다");
			return;
		}

		auto& device = Device::Instance();
	
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_Resource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = m_Size;

		m_ConstantBufferView = device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		device.GetD3DDevice()->CreateConstantBufferView(&cbvDesc, m_ConstantBufferView.GetDescriptorHandle());

	}

	uint32_t Buffer::GetElementCount() const
	{
		return m_ElementCount;
	}

	uint32_t Buffer::GetSize() const
	{
		return m_Size;
	}

	//=============================================================================
	

	Shared<Buffer> BufferManager::GetConstantBuffer(void* data, UINT requestedSize)
	{
		auto it = m_AvailableConstantBuffers.find(Align(requestedSize, 256));

		if (it == m_AvailableConstantBuffers.end())
		{
			// CS_DEBUG_INFO("Create New ConstantBuffer");
			auto newBuffer = CreateShared<Buffer>(data, requestedSize, 0, true, true);
			m_UsedConstantBuffers.push_back(newBuffer);
			return newBuffer;
		}

		const auto constantBuffer = it->second;

		m_AvailableConstantBuffers.erase(it);
		m_UsedConstantBuffers.push_back(constantBuffer);

		if(data)
		{
			constantBuffer->SetData(data, 0, requestedSize);
		}
		
		return constantBuffer;
	}

	Shared<Buffer> BufferManager::GetBuffer(void* data, UINT requestedSize, UINT count, bool bAsDynamic)
	{
		
		auto it = m_AvailableBuffers.begin();
		while(it != m_AvailableBuffers.end())
		{
			if(it->first.bIsDynmaic == bAsDynamic && it->first.Size >= requestedSize) // find first match size
			{
				it->second->SetElementCount(count);
				if(data)
				{
					it->second->SetData(data, 0, requestedSize);
				}
				auto buffer = it->second;
				
				m_AvailableBuffers.erase(it);
				m_UsedBuffers.push_back(buffer);

				return buffer;
			}
			++it;
		}

		// CS_DEBUG_INFO("Create New Buffer");
		const auto newBuffer = CreateShared<Buffer>(data, requestedSize, count, false, bAsDynamic);
		m_UsedBuffers.push_back(newBuffer);
		return newBuffer;
	}

	void BufferManager::Flush()
	{
		if(!m_UsedConstantBuffers.empty())
		{
			for(int i=0; i< m_UsedConstantBuffers.size();i++)
			{
				m_AvailableConstantBuffers.insert({ m_UsedConstantBuffers[i]->GetSize(), m_UsedConstantBuffers[i] });
			}
			m_UsedConstantBuffers.clear();
		}


		if (!m_UsedBuffers.empty())
		{
			for (int i = 0; i < m_UsedBuffers.size(); i++)
			{
				m_AvailableBuffers.insert({ { m_UsedBuffers[i]->GetSize(), m_UsedBuffers[i]->GetIsDynamic() }, m_UsedBuffers[i]});
			}
			m_UsedBuffers.clear();
		}
	}

}
