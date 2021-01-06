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
		CS_ASSERT(SUCCEEDED(hr), "텍스쳐 디폴트 버퍼를 생성하는데 실패하였습니다.");
			
	}

	Texture::Texture(const std::string& fileName, D3D12_RESOURCE_FLAGS resourceFlags)
	{
		auto& renderer = Renderer::Instance();
		auto device = renderer.GetDevice();
		auto commandQueue = renderer.GetCommandQueue();

		std::filesystem::path filePath(fileName.c_str());
		CS_ASSERT(std::filesystem::exists(filePath), "%s 파일이 존재하지 않습니다.", filePath.string().c_str());

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
		CS_ASSERT(SUCCEEDED(hr), "%s 텍스쳐를 로드하는데 실패하였습니다.", filePath.string().c_str());


		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		textureDesc.Width = (UINT)metaData.width;
		textureDesc.Height = (UINT)metaData.height;
		textureDesc.DepthOrArraySize = (UINT)metaData.arraySize;
		textureDesc.MipLevels = (UINT)metaData.mipLevels;
		textureDesc.Format = metaData.format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;


		hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&textureDesc, D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&m_Resource));
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
		device->GetCopyableFootprints(&m_Resource->GetDesc(), 0, (UINT)subResources.size(), 0, nullptr, nullptr, nullptr, &requiredSize);

		Microsoft::WRL::ComPtr<ID3D12Resource> textureUploadBuffer = nullptr;
		hr = device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(requiredSize), D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&textureUploadBuffer));
		CS_ASSERT(SUCCEEDED(hr), "텍스쳐 업로드 버퍼를 생성하는데 실패하였습니다.");

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
	}

	Texture::Texture(ID3D12Resource* resource, D3D12_RESOURCE_FLAGS resourceFlags /*= D3D12_RESOURCE_FLAG_NONE*/) :
		m_Resource(resource)
	{
	}

	void Texture::CreateShaderResourceView(DXGI_FORMAT format, D3D12_SRV_DIMENSION srvDimension)
	{
		if (!m_ShaderResourceView.IsNull())
			return;
		/*셰이더 리소스 뷰를 생성합니다.*/
		auto device = Renderer::Instance().GetDevice();
		auto& descHeapManager = DescriptorHeapManager::Instance();


		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.Format = format;
		shaderResourceViewDesc.ViewDimension = srvDimension;
		shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		switch (shaderResourceViewDesc.ViewDimension)
		{
		case D3D12_SRV_DIMENSION_TEXTURE2D:
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			break;
		case D3D12_SRV_DIMENSION_TEXTURECUBE:
			shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
			shaderResourceViewDesc.TextureCube.MipLevels = 1;
			break;
		default:
			CS_ASSERT(false, "지원되지 않는 SRV DIMENSION 입니다");
		}


		m_ShaderResourceView = descHeapManager.AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		device->CreateShaderResourceView(m_Resource.Get(), &shaderResourceViewDesc, m_ShaderResourceView.GetDescriptorHandle());
	}


	void Texture::CreateRenderTargetView(DXGI_FORMAT format, D3D12_RTV_DIMENSION rtvDimension)
	{
		if (!m_RenderTargetView.IsNull())
			return;
		auto device = Renderer::Instance().GetDevice();
		auto& descHeapManager = DescriptorHeapManager::Instance();

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = format;
		rtvDesc.ViewDimension = rtvDimension;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		m_RenderTargetView = descHeapManager.AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
		device->CreateRenderTargetView(m_Resource.Get(), &rtvDesc, m_RenderTargetView.GetDescriptorHandle());
	}

	void Texture::CreateDepthStencilView(DXGI_FORMAT format, D3D12_DSV_DIMENSION dsvDimension)
	{
		if (!m_DepthStencilView.IsNull())
			return;
		auto device = Renderer::Instance().GetDevice();
		auto& descHeapManager = DescriptorHeapManager::Instance();

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = format;
		dsvDesc.ViewDimension = dsvDimension;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.Texture2D.MipSlice = 0;

		m_DepthStencilView = descHeapManager.AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
		device->CreateDepthStencilView(m_Resource.Get(), &dsvDesc, m_DepthStencilView.GetDescriptorHandle());
	}

	DescriptorHeapManager::DescriptorHeapManager()
	{
		/*TODO : Page Allocator*/
		/*WORK IN PROGRESS*/
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavDesc = {};
		cbvSrvUavDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvSrvUavDesc.NumDescriptors = 100;
		cbvSrvUavDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		cbvSrvUavDesc.NodeMask = 0;
		HRESULT hr = device->CreateDescriptorHeap(&cbvSrvUavDesc, IID_PPV_ARGS(&m_CBVSRVUAVHeap));
		CS_ASSERT(SUCCEEDED(hr), "CBV,SRV,UAV Descriptor Heap을 생성하는데 실패하였습니다.");
		m_CBVSRVUAVHeapIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
		rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvDesc.NumDescriptors = 30;
		rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		rtvDesc.NodeMask = 0;
		hr = device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_RTVHeap));
		CS_ASSERT(SUCCEEDED(hr), "RTV Descriptor Heap을 생성하는데 실패하였습니다.");
		m_RTVHeapIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
		dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvDesc.NumDescriptors = 30;
		dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvDesc.NodeMask = 0;
		hr = device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&m_DSVHeap));
		CS_ASSERT(SUCCEEDED(hr), "RTV Descriptor Heap을 생성하는데 실패하였습니다.");
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
			CS_ASSERT(false, "아직 지원하지 않는 타입입니다.");
			return Descriptor();
		}
	}



}