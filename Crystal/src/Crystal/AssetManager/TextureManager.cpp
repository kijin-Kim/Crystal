#include "cspch.h"
#include "TextureManager.h"
#include "Crystal/Renderer/Renderer.h"
#include "DirectXTex/DirectXTex.h"

namespace Crystal {

	TextureManager::TextureManager()
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = m_TexturePoolSize;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_TexturesHeap));
		CS_ASSERT(SUCCEEDED(hr), "Fail to Create TexturePool");

		m_TexturePoolCpuHandle = m_TexturesHeap->GetCPUDescriptorHandleForHeapStart();
	}



	void TextureManager::Load(const std::vector<std::string>& filepaths, D3D12_SRV_DIMENSION srvDimension, const std::string& textureAlias /*= ""*/)
	{
		Texture texture;
		texture.CpuHandle = m_TexturePoolCpuHandle;
		texture.Count = (UINT)filepaths.size();

		TextureData textureData;
		textureData.Buffers.resize(filepaths.size());
		textureData.texture = texture;
		
		auto& renderer = Renderer::Instance();
		auto device = renderer.GetDevice();
		auto commandQueue = renderer.GetCommandQueue();


		for(int i =0; i<filepaths.size(); i++)
		{
			std::string alias = textureAlias == "" ? filepaths[i] : textureAlias;

			std::filesystem::path filePath(filepaths[i].c_str());
			CS_ASSERT(std::filesystem::exists(filePath), "%s 파일이 존재하지 않습니다.", filePath.string().c_str());

			DirectX::TexMetadata metaData;
			DirectX::ScratchImage scratchImage;
			HRESULT hr = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metaData, scratchImage);
			CS_ASSERT(SUCCEEDED(hr), "%s 텍스쳐를 로드하는데 실패하였습니다.", filePath.string().c_str());

			metaData.format = DirectX::MakeSRGB(metaData.format);

			hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Tex2D(metaData.format, (UINT64)metaData.width, (UINT)metaData.height, (UINT16)metaData.arraySize, (UINT16)metaData.mipLevels), D3D12_RESOURCE_STATE_COMMON,
				nullptr, IID_PPV_ARGS(&textureData.Buffers[i]));
			CS_ASSERT(SUCCEEDED(hr), "텍스쳐 디폴트 버퍼를 생성하는데 실패하였습니다.");

			std::vector<D3D12_SUBRESOURCE_DATA> subResources(scratchImage.GetImageCount());
			const DirectX::Image* image = scratchImage.GetImages();
			for (int i = 0; i < scratchImage.GetImageCount(); i++)
			{
				auto& subResource = subResources[i];
				subResource.RowPitch = image[i].rowPitch;
				subResource.SlicePitch = image[i].slicePitch;
				subResource.pData = image[i].pixels;
			}

			UINT64 requiredSize = 0;
			device->GetCopyableFootprints(&textureData.Buffers[i]->GetDesc(), 0, (UINT)subResources.size(), 0, nullptr, nullptr, nullptr, &requiredSize);

			Microsoft::WRL::ComPtr<ID3D12Resource> textureUploadBuffer = nullptr;
			hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(requiredSize), D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, IID_PPV_ARGS(&textureUploadBuffer));
			CS_ASSERT(SUCCEEDED(hr), "텍스쳐 업로드 버퍼를 생성하는데 실패하였습니다.");

			auto cmdList = commandQueue->GetCommandList();
			UpdateSubresources(cmdList.Get(), textureData.Buffers[i].Get(), textureUploadBuffer.Get(), 0, 0, (UINT)subResources.size(), subResources.data());

			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = textureData.Buffers[i].Get();
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			cmdList->ResourceBarrier(1, &resourceBarrier);

			commandQueue->Execute(cmdList);
			commandQueue->Flush();

			D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
			shaderResourceViewDesc.Format = metaData.format;
			shaderResourceViewDesc.ViewDimension = srvDimension;
			shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;

			device->CreateShaderResourceView(textureData.Buffers[i].Get(), &shaderResourceViewDesc, m_TexturePoolCpuHandle);
			m_TexturePoolCpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			m_TexturePool[alias] = textureData;
		}
		
	}


	const Texture& TextureManager::GetTexture(const std::string& textureAlias) const
	{
		if (m_TexturePool.find(textureAlias) != m_TexturePool.end())
			return m_TexturePool[textureAlias].texture;
		else
		{
			CS_ASSERT(false, "%s 텍스쳐가 존재하지 않습니다. 먼저 로드해주세요.", textureAlias.c_str());
			auto nullTexture = Texture();
			return nullTexture;
		}
	}

}