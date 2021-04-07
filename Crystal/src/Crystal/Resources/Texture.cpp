#include "cspch.h"
#include "Texture.h"
#include "Crystal/Renderer/Renderer.h"
#include "DirectXTex/DirectXTex.h"

namespace Crystal {
	Texture::Texture(int width, int height, int depth, int mipLevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES initialStates)
	{
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Width = (UINT)width;
		textureDesc.Height = (UINT)height;
		textureDesc.DepthOrArraySize = (UINT)depth;
		textureDesc.MipLevels = (UINT)mipLevels;
		textureDesc.Format = format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Flags = resourceFlags;

		auto device = Renderer::Instance().GetDevice();
		HRESULT hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&textureDesc, initialStates,
			nullptr, IID_PPV_ARGS(&m_Resource));
		CS_FATAL(SUCCEEDED(hr), "텍스쳐 디폴트 버퍼를 생성하는데 실패하였습니다.");
	}

	Texture::Texture(const std::string& fileName, D3D12_RESOURCE_FLAGS resourceFlags)
	{
		CS_INFO("%s 텍스쳐 불러오는 중...", fileName.c_str());
		auto& renderer = Renderer::Instance();
		auto device = renderer.GetDevice();
		auto commandQueue = renderer.GetCommandQueue();

		std::filesystem::path filePath(fileName.c_str());
		CS_FATAL(std::filesystem::exists(filePath), "%s 파일이 존재하지 않습니다.", filePath.string().c_str());

		DirectX::TexMetadata metaData;
		DirectX::ScratchImage scratchImage;
		HRESULT hr;

		if (filePath.extension() == ".dds")
		{
			hr = LoadFromDDSFile(filePath.wstring().c_str(), DirectX::DDS_FLAGS_FORCE_RGB, &metaData, scratchImage);
		}
		else if (filePath.extension() == ".hdr")
		{
			hr = LoadFromHDRFile(filePath.wstring().c_str(), &metaData, scratchImage);
		}
		else if (filePath.extension() == ".tga")
		{
			hr = LoadFromTGAFile(filePath.wstring().c_str(), &metaData, scratchImage);
		}
		else
		{
			hr = LoadFromWICFile(filePath.wstring().c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metaData, scratchImage);
		}
		CS_FATAL(SUCCEEDED(hr), "%s 텍스쳐를 로드하는데 실패하였습니다.", filePath.string().c_str());

		DirectX::ScratchImage mipChain;
		hr = GenerateMipMaps(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, 0, mipChain);
		CS_FATAL(SUCCEEDED(hr), "%s 텍스쳐의 밉 체인을 생성하는데 실패하였습니다.", filePath.string().c_str());

		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Width = (UINT)metaData.width;
		textureDesc.Height = (UINT)metaData.height;
		textureDesc.DepthOrArraySize = (UINT)metaData.arraySize;
		textureDesc.MipLevels = (UINT)mipChain.GetMetadata().mipLevels;
		textureDesc.Format = metaData.format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&textureDesc, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&m_Resource));
		CS_FATAL(SUCCEEDED(hr), "텍스쳐 디폴트 버퍼를 생성하는데 실패하였습니다.");

		std::vector<D3D12_SUBRESOURCE_DATA> subResources(mipChain.GetImageCount());
		const DirectX::Image* image = mipChain.GetImages();
		for (int i = 0; i < mipChain.GetImageCount(); i++)
		{
			auto& subResource = subResources[i];
			subResource.RowPitch = image[i].rowPitch;
			subResource.SlicePitch = image[i].slicePitch;
			subResource.pData = image[i].pixels;
		}

		UINT64 requiredSize = 0;
		device->GetCopyableFootprints(&m_Resource->GetDesc(), 0, (UINT)subResources.size(), 0, nullptr, nullptr, nullptr, &requiredSize);

		Microsoft::WRL::ComPtr<ID3D12Resource> textureUploadBuffer = nullptr;
		hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(requiredSize), D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&textureUploadBuffer));
		CS_FATAL(SUCCEEDED(hr), "텍스쳐 업로드 버퍼를 생성하는데 실패하였습니다.");

		auto cmdList = commandQueue->GetCommandList();
		UpdateSubresources(cmdList.Get(), m_Resource.Get(), textureUploadBuffer.Get(), 0, 0, (UINT)subResources.size(), subResources.data());

		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_Resource.Get();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		cmdList->ResourceBarrier(1, &resourceBarrier);

		commandQueue->Execute(cmdList);
		commandQueue->Flush();

		CS_INFO("%s 텍스쳐 불러오기 완료", fileName.c_str());
	}

	Texture::Texture(ID3D12Resource* resource, D3D12_RESOURCE_FLAGS resourceFlags /*= D3D12_RESOURCE_FLAG_NONE*/) :
		m_Resource(resource)
	{
	}

	void Texture::CreateShaderResourceView(DXGI_FORMAT format, D3D12_SRV_DIMENSION srvDimension)
	{
		if (!m_ShaderResourceView.IsNull())
		{
			CS_WARN("현재 타입의 리소스뷰가 이미 존재합니다");
			return;
		}
		/*셰이더 리소스 뷰를 생성합니다.*/
		auto device = Renderer::Instance().GetDevice();

		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.Format = format;
		shaderResourceViewDesc.ViewDimension = srvDimension;
		shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		switch (shaderResourceViewDesc.ViewDimension)
		{
		case D3D12_SRV_DIMENSION_TEXTURE2D:
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = m_Resource->GetDesc().MipLevels;
			break;
		case D3D12_SRV_DIMENSION_TEXTURECUBE:
			shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
			shaderResourceViewDesc.TextureCube.MipLevels = m_Resource->GetDesc().MipLevels;
			break;
		default:
			CS_FATAL(false, "지원되지 않는 SRV DIMENSION 입니다");
		}

		m_ShaderResourceView = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		device->CreateShaderResourceView(m_Resource.Get(), &shaderResourceViewDesc, m_ShaderResourceView.GetDescriptorHandle());
	}

	void Texture::CreateUnorderedAccessView(DXGI_FORMAT format, D3D12_UAV_DIMENSION uavDimension)
	{
		if (!m_UnorderedAccessView.IsNull())
		{
			CS_WARN("현재 타입의 리소스뷰가 이미 존재합니다");
			return;
		}
		auto device = Renderer::Instance().GetDevice();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = uavDimension;
		switch (uavDimension)
		{
		case D3D12_UAV_DIMENSION_TEXTURE2D:
			uavDesc.Texture2D.MipSlice = 0;
			break;
		case D3D12_UAV_DIMENSION_TEXTURE2DARRAY:
			uavDesc.Texture2DArray.ArraySize = m_Resource->GetDesc().DepthOrArraySize;
			uavDesc.Texture2DArray.FirstArraySlice = 0;
			uavDesc.Texture2DArray.MipSlice = 0;
			uavDesc.Texture2DArray.PlaneSlice = 0;
			break;
		default:
			break;
		}

		m_UnorderedAccessView = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		device->CreateUnorderedAccessView(m_Resource.Get(), nullptr, &uavDesc, m_UnorderedAccessView.GetDescriptorHandle());
	}

	void Texture::CreateRenderTargetView(DXGI_FORMAT format, D3D12_RTV_DIMENSION rtvDimension)
	{
		if (!m_RenderTargetView.IsNull())
		{
			CS_WARN("현재 타입의 리소스뷰가 이미 존재합니다");
			return;
		}
		auto device = Renderer::Instance().GetDevice();

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = format;
		rtvDesc.ViewDimension = rtvDimension;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		m_RenderTargetView = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		device->CreateRenderTargetView(m_Resource.Get(), &rtvDesc, m_RenderTargetView.GetDescriptorHandle());
	}

	void Texture::CreateDepthStencilView(DXGI_FORMAT format, D3D12_DSV_DIMENSION dsvDimension)
	{
		if (!m_DepthStencilView.IsNull())
		{
			CS_WARN("현재 타입의 리소스뷰가 이미 존재합니다");
			return;
		}
		auto device = Renderer::Instance().GetDevice();

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = format;
		dsvDesc.ViewDimension = dsvDimension;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.Texture2D.MipSlice = 0;

		m_DepthStencilView = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		device->CreateDepthStencilView(m_Resource.Get(), &dsvDesc, m_DepthStencilView.GetDescriptorHandle());
	}
}