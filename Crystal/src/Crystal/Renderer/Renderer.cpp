#include "cspch.h"
#include "Renderer.h"

#include "RenderTarget.h"
#include "DepthStencil.h"

#include "DirectXTex/DirectXTex.h"

namespace Crystal {

	void Renderer::Init(const std::shared_ptr<WindowsWindow>& window)
	{
		m_Window = window;

		HRESULT hr = E_FAIL;

		UINT createFactoryDebugFlags = 0;

#if defined CS_DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		CS_ASSERT(SUCCEEDED(hr), "D3D디버그 인터페이스를 가져오는데 실패하였습니다.");
		debugController->EnableDebugLayer();


		Microsoft::WRL::ComPtr<IDXGIDebug1> pdxgiDebug = NULL;
		DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pdxgiDebug));
		hr = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
		CS_ASSERT(SUCCEEDED(hr), "DXGI디버그 인터페이스를 가져오는데 실패하였습니다.");

		createFactoryDebugFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		// #DirectX Create DXGI factory
		hr = CreateDXGIFactory2(createFactoryDebugFlags, IID_PPV_ARGS(m_Factory.GetAddressOf()));
		CS_ASSERT(SUCCEEDED(hr), "DXGI Factory를 생성하는데 실패하였습니다.");

		// #DirectX Create Grahpics Adapter
		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter = nullptr;
		for (int i = 0; m_Factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
		{
			if (adapter)
			{
				DXGI_ADAPTER_DESC1 adapterDesc;
				adapter->GetDesc1(&adapterDesc);

				// #DirectX Create Device 
				hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));
				CS_ASSERT(SUCCEEDED(hr), "D3D Device를 생성하는데 실패하였습니다.");
				break;
			}
		}


#if defined CS_DEBUG
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
		if (SUCCEEDED(m_Device.As(&infoQueue)))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		}
#endif

		m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);

		m_CommandQueue = std::make_shared<CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);

		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////

		// #DirectX Swap Chain Description
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = m_Window->GetWidth();
		swapChainDesc.Height = m_Window->GetHeight();
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		//swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// #DirectX Swap Chain FullScreen Description
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC  swapChainFullscreenDesc;
		swapChainFullscreenDesc.RefreshRate.Numerator = 60;
		swapChainFullscreenDesc.RefreshRate.Denominator = 1;
		swapChainFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainFullscreenDesc.Windowed = TRUE;


		// #DirectX Create Swap Chain https://gamedev.stackexchange.com/questions/149822/direct3d-12-cant-create-a-swap-chain
		hr = m_Factory->CreateSwapChainForHwnd(m_CommandQueue->GetRaw(), m_Window->GetWindowHandle(), &swapChainDesc,
			&swapChainFullscreenDesc, nullptr, m_SwapChain.GetAddressOf());
		CS_ASSERT(SUCCEEDED(hr), "Swap Chain을 생성하는데 실패하였습니다");

		Renderer::Get().GetFactory()->MakeWindowAssociation(m_Window->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);


		for (int i = 0; i < 2; i++)
		{
			ID3D12Resource* rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_RenderTargets.push_back(std::make_unique<RenderTarget>(rtvBuffer));
			SAFE_RELEASE(rtvBuffer);
		}

		m_DepthStencil = std::make_unique<DepthStencil>(m_Window->GetWidth(), m_Window->GetHeight());


		hr = m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(256),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ConstantBufferResource));
		CS_ASSERT(SUCCEEDED(hr), "");

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 2;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		hr = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_CommonDescriptorHeap));
		CS_ASSERT(SUCCEEDED(hr), "");

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_ConstantBufferResource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = 256;
		m_Device->CreateConstantBufferView(&cbvDesc, m_CommonDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		//-------------
		std::filesystem::path filePath("assets/textures/Megaphone_01_8-bit_Diffuse.png");
		CS_ASSERT(std::filesystem::exists(filePath), "%s 파일이 존재하지 않습니다.", filePath.string().c_str());

		DirectX::TexMetadata metaData;
		DirectX::ScratchImage scratchImage;
		hr = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metaData, scratchImage);
		CS_ASSERT(SUCCEEDED(hr), "%s 텍스쳐를 로드하는데 실패하였습니다.", filePath.string().c_str());

		metaData.format = DirectX::MakeSRGB(metaData.format);

		hr = m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(metaData.format, (UINT64)metaData.width, (UINT)metaData.height, (UINT16)metaData.arraySize, (UINT16)metaData.mipLevels), D3D12_RESOURCE_STATE_COMMON,
			nullptr, IID_PPV_ARGS(&m_TextureBuffer));
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
		m_Device->GetCopyableFootprints(&m_TextureBuffer->GetDesc(), 0, (UINT)subResources.size(), 0, nullptr, nullptr, nullptr, &requiredSize);

		Microsoft::WRL::ComPtr<ID3D12Resource> textureUploadBuffer = nullptr;
		hr = m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(requiredSize), D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&textureUploadBuffer));
		CS_ASSERT(SUCCEEDED(hr), "텍스쳐 업로드 버퍼를 생성하는데 실패하였습니다.");

		auto cmdList = m_CommandQueue->GetCommandList();
		UpdateSubresources(cmdList.Get(), m_TextureBuffer.Get(), textureUploadBuffer.Get(), 0, 0, (UINT)subResources.size(), subResources.data());

		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_TextureBuffer.Get();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		cmdList->ResourceBarrier(1, &resourceBarrier);

		m_CommandQueue->Execute(cmdList);
		m_CommandQueue->Flush();

		D3D12_CPU_DESCRIPTOR_HANDLE nextHandle = m_CommonDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		nextHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.Format = metaData.format;
		shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		m_Device->CreateShaderResourceView(m_TextureBuffer.Get(), &shaderResourceViewDesc, nextHandle);


		D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
		samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		samplerHeapDesc.NumDescriptors = 1;
		samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		samplerHeapDesc.NodeMask = 0;

		hr = m_Device->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_SamplerDescriptorHeap));
		CS_ASSERT(SUCCEEDED(hr), "샘플러 힙을 생성하는데 실패하였습니다.");


		D3D12_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		samplerDesc.BorderColor;
		samplerDesc.MinLOD = 0.0f;
		samplerDesc.MaxLOD = 0.0f;

		m_Device->CreateSampler(&samplerDesc, m_SamplerDescriptorHeap->GetCPUDescriptorHandleForHeapStart());


		

		m_ShaderLibrary->Load("assets/shaders/BlinnPhongShader", "BlinnPhongShader");

		///////////////////////////////////////////////////
		///////////////////// SHADER //////////////////////
		///////////////////////////////////////////////////

		CD3DX12_DESCRIPTOR_RANGE1 commonDescriptorHeapRanges[2] = {};
		commonDescriptorHeapRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		commonDescriptorHeapRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_DESCRIPTOR_RANGE1 samplerDescriptorHeapRanges[1] = {};
		samplerDescriptorHeapRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);


		CD3DX12_ROOT_PARAMETER1 rootParameter[2];
		rootParameter[0].InitAsDescriptorTable(_countof(commonDescriptorHeapRanges), commonDescriptorHeapRanges);
		rootParameter[1].InitAsDescriptorTable(_countof(samplerDescriptorHeapRanges), samplerDescriptorHeapRanges);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootsigDesc(_countof(rootParameter), rootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		m_RootSignature = std::make_unique<RootSignature>(rootsigDesc);


		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopology;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;

		pipelineStateStream.RootSignature = m_RootSignature->GetRaw();
		pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
		pipelineStateStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		auto& shaderDatablobs = m_ShaderLibrary->GetShader("BlinnPhongShader")->GetRaw();
		pipelineStateStream.VS = { shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(), shaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
		pipelineStateStream.PS = { shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(), shaderDatablobs[ShaderType::Pixel]->GetBufferSize() };

		pipelineStateStream.DSVFormat = m_DepthStencil->GetFormat();
		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = m_RenderTargets[0]->GetFormat();
		pipelineStateStream.RTVFormats = rtvFormat;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };
		m_GraphicsPipeline = std::make_unique<GraphicsPipeline>(&pipelineStateStreamDesc);


		//////////////////////////////////////////////////////////////////////////////////////////////////////
		

		m_Camera = std::make_unique<Camera>(1366, 768);
		m_Camera->SetPosition(DirectX::XMFLOAT3(0, 100.0f, 500.0f));

		/*m_MainWorld->AddLevel(m_Level);
		m_Level->SpawnActor(m_Actor);
		m_MainWorld->DestroyActor(m_Actor);*/

		model = new Model("assets/models/Megaphone_01.fbx");
	}

	void Renderer::Render()
	{
		//////////////////////////////////////////
		///// UPDATE /////////////////////////////
		//////////////////////////////////////////


		timer.Tick();
		static float angleA = 0.0f;
		angleA += 1.0f * timer.DeltaTime();

		XMStoreFloat4x4(&m_WorldMat, DirectX::XMMatrixIdentity());
		XMStoreFloat4x4(&m_WorldMat, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMat),
			DirectX::XMMatrixRotationRollPitchYaw(0.0f, angleA, 0.0f)));
		XMStoreFloat4x4(&m_WorldMat, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMat)));

		m_ConstantBufferData.World = m_WorldMat;
		m_ConstantBufferData.ViewProj = m_Camera->GetViewProjection();
		m_ConstantBufferData.LightPositionInWorld = DirectX::XMFLOAT3(0.0f, 200.0f, 200.0f);
		m_ConstantBufferData.CameraPositionInWorld = m_Camera->GetWorldPosition();

		D3D12_RANGE readRange = { 0,0 };
		UINT8* bufferBegin = nullptr;
		m_ConstantBufferResource->Map(0, &readRange, (void**)&bufferBegin);
		memcpy(bufferBegin, (void*)&m_ConstantBufferData, sizeof(m_ConstantBufferData));
		m_ConstantBufferResource->Unmap(0, nullptr);


		//////////////////////////////////////////
		///// RENDER /////////////////////////////
		//////////////////////////////////////////

	
		auto cmdList = m_CommandQueue->GetCommandList();

		DirectX::XMFLOAT3 lightPosition = DirectX::XMFLOAT3(200.0f, 200.0f, 200.0f);
		float clearColor[] = { 135.0f / 255.0f, 206.0f / 255.0f  , 250.0f / 255.0f, 1.0f };

		cmdList->SetPipelineState(m_GraphicsPipeline.get()->GetRaw());
		cmdList->SetGraphicsRootSignature(m_RootSignature->GetRaw());

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_CommonDescriptorHeap.Get(), m_SamplerDescriptorHeap.Get() };
		cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		cmdList->SetGraphicsRootDescriptorTable(0, m_CommonDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		cmdList->SetGraphicsRootDescriptorTable(1, m_SamplerDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->RSSetViewports(1, &m_Camera->GetViewport());
		cmdList->RSSetScissorRects(1, &m_Camera->GetScissorRect());
		cmdList->OMSetRenderTargets(1, &m_RenderTargets[m_RtvIndex]->GetCpuHandle(), TRUE, &m_DepthStencil->GetCpuHandle());

		m_RenderTargets[m_RtvIndex]->TransResourceState(cmdList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		cmdList->ClearRenderTargetView(m_RenderTargets[m_RtvIndex]->GetCpuHandle(), clearColor, 0, nullptr);
		cmdList->ClearDepthStencilView(m_DepthStencil->GetCpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		model->Render(cmdList);

		m_RenderTargets[m_RtvIndex]->TransResourceState(cmdList.Get(), D3D12_RESOURCE_STATE_PRESENT);

		m_CommandQueue->Execute(cmdList);
		m_CommandQueue->Flush();

		DXGI_PRESENT_PARAMETERS presentParameters;
		presentParameters.DirtyRectsCount = 0;
		presentParameters.pDirtyRects = NULL;
		presentParameters.pScrollRect = NULL;
		presentParameters.pScrollOffset = NULL;
		m_SwapChain->Present1(1, 0, &presentParameters);

		m_RtvIndex++;
		m_RtvIndex = m_RtvIndex % 2;
	}
}