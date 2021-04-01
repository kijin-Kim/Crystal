#include "cspch.h"
#include "Renderer.h"

#include "DirectXTex/DirectXTex.h"
#include <iostream>

#include "Crystal/Resources/ShaderManager.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/Resources/Material.h"

#include "imgui/backends/imgui_impl_dx12.h"
#include "imgui/backends/imgui_impl_win32.h"
#include "Crystal/Core/ApplicationUtility.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {
	void Renderer::Init(WindowsWindow* window)
	{
		m_Window = window;

		CreateDevice();
		CreateRenderTargetViewFromSwapChain();
		CreateDepthStencilView();

		auto& resourceManager = Crystal::ResourceManager::Instance();

		//============================================================================================
		auto panoTextureWeak = resourceManager.CreateTextureFromFile(
			"assets/textures/cubemaps/T_Cube_Skybox_1.hdr", "HDRI_Skybox_Space");
		if (auto panoTexture = panoTextureWeak.lock())
		{
			panoTexture->CreateShaderResourceView(panoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		}
		
		auto cubemapWeak = resourceManager.CreateTexture(2048, 2048, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, "Cube_Skybox_Space");
		if (auto cubemap = cubemapWeak.lock())
		{
			cubemap->CreateUnorderedAccessView(cubemap->GetResource()->GetDesc().Format,
				D3D12_UAV_DIMENSION_TEXTURE2DARRAY);
			cubemap->CreateShaderResourceView(cubemap->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURECUBE);
		}

		auto irradianceMapWeak = resourceManager.CreateTexture(32, 32, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, "Cube_Skybox_Space_Irradiance");
		if (auto irradianceMap = irradianceMapWeak.lock())
		{
			irradianceMap->CreateUnorderedAccessView(irradianceMap->GetResource()->GetDesc().Format,
				D3D12_UAV_DIMENSION_TEXTURE2DARRAY);
			irradianceMap->CreateShaderResourceView(irradianceMap->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURECUBE);
		}


		// TODO : TEMP
		m_PanoTexture = resourceManager.GetTexture("HDRI_Skybox_Space").lock();
		m_CubemapTexture = resourceManager.GetTexture("Cube_Skybox_Space").lock();
		m_IrradiancemapTexture = resourceManager.GetTexture("Cube_Skybox_Space_Irradiance").lock();
		//============================================================================================

		resourceManager.CreateShaderFromFile("assets/shaders/PBRShader_Static.hlsl", "PBRShader_Static");
		resourceManager.CreateShaderFromFile("assets/shaders/PBRShader_Skeletal.hlsl", "PBRShader_Skeletal");
		resourceManager.CreateShaderFromFile("assets/shaders/SkyboxShader.hlsl", "Cubemap");
		resourceManager.CreateShaderFromFile("assets/shaders/EquirectangularToCube.hlsl", "PanoToCubemap");
		resourceManager.CreateShaderFromFile("assets/shaders/DiffuseIrradianceSampling.hlsl", "DiffuseIrradianceSampling");
		resourceManager.CreateShaderFromFile("assets/shaders/SpecularIrradianceSampling.hlsl", "SpecularIrradianceSampling");
		resourceManager.CreateShaderFromFile("assets/shaders/SimpleColorShader.hlsl", "SimpleColorShader");


		
		{
			resourceManager.GetShader("PBRShader_Static")->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				});


			RootParameter perFrame = {};
			RootParameter perObject = {};
			RootParameter perDraw = {};

			perFrame.CbvCount = 1;
			perFrame.SrvCount = 1;

			perObject.CbvCount = 1;

			perDraw.CbvCount = 1;
			perDraw.SrvCount = 4;


			RootSignature rootsig = { perFrame, perObject, perDraw, { CD3DX12_STATIC_SAMPLER_DESC(0) }};
			resourceManager.GetShader("PBRShader_Static")->SetRootSignature(rootsig);
		}


		{
			resourceManager.GetShader("SimpleColorShader")->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				});

			RootParameter perFrame = {};
			RootParameter perObject = {};
			RootParameter perDraw = {};

			perFrame.CbvCount = 1;

			perObject.CbvCount = 1;

			resourceManager.GetShader("SimpleColorShader")->SetRootSignature({ perFrame, perObject, perDraw });
		}



		/*Pipeline을 만듭니다.*/
		m_LightingPipeline = std::make_unique<LightingStaticPipeline>("PBRLightingPipeline", 
			resourceManager.GetShader("PBRShader_Static"));
		m_WireframePipeline = std::make_unique<LinePipeline>("LinePipline", 
			resourceManager.GetShader("SimpleColorShader"));
		m_CubemapPipeline = std::make_unique<CubemapPipeline>("CubemapPipeline", 
			resourceManager.GetShader("Cubemap"));
		m_PanoToCubemapPipeline = std::make_unique<PanoToCubemapPipeline>("PanoToCubemapPipeline", 
			resourceManager.GetShader("PanoToCubemap"));
		m_DiffIrradSamplingPipeline = std::make_unique<DiffIrradSamplingPipeline>("DiffIrradSamplingPipeline", 
			resourceManager.GetShader("DiffuseIrradianceSampling"));

		/// COMPUTE
		auto commandList = m_CommandQueue->GetCommandList();

		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		
		resourceBarrier.Transition.pResource = m_CubemapTexture->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		PanoToCubemapPipeline::PanoToCubemapPipelineInputs panoToCubemapPipelineInputs = {};
		panoToCubemapPipelineInputs.SourceTexture = m_PanoTexture.get();
		panoToCubemapPipelineInputs.DestinationTexture = m_CubemapTexture.get();
		m_PanoToCubemapPipeline->Record(commandList, &panoToCubemapPipelineInputs);

		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		commandList->ResourceBarrier(1, &resourceBarrier);

		resourceBarrier.Transition.pResource = m_IrradiancemapTexture->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		DiffIrradSamplingPipeline::DiffIrradSamplingPipelineInputs diffIrradSamplingPipelineInputs = {};
		diffIrradSamplingPipelineInputs.SourceTexture = m_CubemapTexture.get();
		diffIrradSamplingPipelineInputs.DestinationTexture = m_IrradiancemapTexture.get();
		m_DiffIrradSamplingPipeline->Record(commandList, &diffIrradSamplingPipelineInputs);

		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		m_CommandQueue->Execute(commandList);
		CS_INFO("HDRI로부터 Cubemap 생성중...");
		m_CommandQueue->Flush();
		CS_INFO("HDRI로부터 Cubemap 생성 완료");

		/*ImGui 구현*/
		{
			D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
			descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			descriptorHeapDesc.NumDescriptors = 1;
			descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			descriptorHeapDesc.NodeMask = 0;

			HRESULT hr = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_ImGuiHeap));
			CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");
			// Setup Platform/Renderer backends
			ImGui_ImplDX12_Init(m_Device.Get(), 2,
				DXGI_FORMAT_R8G8B8A8_UNORM, m_ImGuiHeap.Get(),
				m_ImGuiHeap->GetCPUDescriptorHandleForHeapStart(),
				m_ImGuiHeap->GetGPUDescriptorHandleForHeapStart());
		}
	}

	void Renderer::Render()
	{
		ChangeResolution(1920, 1080);
		ChangeDisplayMode();


		auto commandList = m_CommandQueue->GetCommandList();

		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_RenderTargetTextures[m_RtvIndex]->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		const auto mainCamera = ApplicationUtility::GetPlayerController().GetMainCamera().lock();

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->RSSetViewports(1, &mainCamera->GetViewport());
		commandList->RSSetScissorRects(1, &mainCamera->GetScissorRect());
		commandList->OMSetRenderTargets(1, &m_RenderTargetTextures[m_RtvIndex]->GetRenderTargetView(), TRUE,
			&m_DepthStencilBufferTexture->GetDepthStencilView());

		float clearColorValue[4] = { 1.0f, 0.0f, 1.0f, 0.0f };
		const auto clearDepthValue = 1.0f;
		const auto clearStencilValue = 0.0f;
		commandList->ClearRenderTargetView(m_RenderTargetTextures[m_RtvIndex]->GetRenderTargetView(),
			clearColorValue, 0, nullptr);
		commandList->ClearDepthStencilView(m_DepthStencilBufferTexture->GetDepthStencilView(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearDepthValue, clearStencilValue, 0, nullptr);

		LightingStaticPipeline::LightingPipelineInputs lightingPipelineInputs = {};
		lightingPipelineInputs.SkeletalMeshComponents = &m_SkeletalMeshComponents;
		lightingPipelineInputs.StaticMeshComponents = &m_StaticMeshComponents;
		lightingPipelineInputs.Camera = mainCamera.get();
		lightingPipelineInputs.IrradiancemapTexture = m_IrradiancemapTexture.get();
		m_LightingPipeline->Record(commandList, &lightingPipelineInputs);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		LinePipeline::LinePipelineInputs wireframePipelineInputs = {};
		wireframePipelineInputs.Camera = mainCamera.get();
		wireframePipelineInputs.CollisionComponents = &m_CollisionComponents;
		m_WireframePipeline->Record(commandList, &wireframePipelineInputs);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		CubemapPipeline::CubemapPipelineInputs cubemapPipelineInputs = {};
		cubemapPipelineInputs.Camera = mainCamera.get();
		cubemapPipelineInputs.CubemapTexture = m_CubemapTexture.get();
		m_CubemapPipeline->Record(commandList, &cubemapPipelineInputs);

		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_RenderTargetTextures[m_RtvIndex]->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		m_CommandQueue->Execute(commandList);

		DXGI_PRESENT_PARAMETERS presentParameters;
		presentParameters.DirtyRectsCount = 0;
		presentParameters.pDirtyRects = nullptr;
		presentParameters.pScrollRect = nullptr;
		presentParameters.pScrollOffset = nullptr;
		m_SwapChain->Present1(1, 0, &presentParameters);

		m_CommandQueue->Flush();

		m_RtvIndex++;
		m_RtvIndex = m_RtvIndex % 2;
	}

	void Renderer::ChangeResolution(int width, int height)
	{
		// 1. Clear Reference of RenderTarget
		// 2. Destruct DepthStencil
		// 3. Resize
		// 4. Set Camera's Viewport And ScissorRect
		// 5. Make Sure Current Index of RenderTarget is BackBuffer
		if (m_ResWidth == width && m_ResHeight == height)
			return;

		CS_INFO("해상도를 변경하는 중...");

		m_RtvIndex = 0;

		m_ResWidth = width;
		m_ResHeight = height;

		DXGI_MODE_DESC targetParam = {};
		targetParam.Width = width;
		targetParam.Height = height;
		targetParam.RefreshRate.Numerator = 60;
		targetParam.RefreshRate.Denominator = 1;
		targetParam.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		targetParam.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		targetParam.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		HRESULT hr = m_SwapChain->ResizeTarget(&targetParam);
		CS_FATAL(SUCCEEDED(hr), "타겟을 Resize하는데 실패하였습니다.");

		for (auto& rt : m_RenderTargetTextures)
		{
			rt.reset();
		}

		hr = m_SwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_FATAL(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");

		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_RenderTargetTextures[i] = std::make_unique<Texture>(rtvBuffer.Get(), D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_RenderTargetTextures[i]->CreateRenderTargetView(m_RenderTargetTextures[i]->GetResource()->GetDesc().Format,
				D3D12_RTV_DIMENSION_TEXTURE2D);
		}
		m_DepthStencilBufferTexture = std::make_unique<Texture>(m_ResWidth, m_ResHeight, 1, 0,
			DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_DepthStencilBufferTexture->CreateDepthStencilView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_DSV_DIMENSION_TEXTURE2D);

		auto cameraComponent = ApplicationUtility::GetPlayerController().GetMainCamera().lock();
		cameraComponent->SetViewport({ 0,0, (FLOAT)width, (FLOAT)height, 0.0f, 1.0f });
		cameraComponent->SetScissorRect({ 0, 0, width, height });
		m_Window->SetWidth(targetParam.Width);
		m_Window->SetHeight(targetParam.Height);

		CS_INFO("해상도를 변경 완료.");
		CS_INFO("현재 해상도 : %d X %d", targetParam.Width, targetParam.Height);
	}

	void Renderer::CreateDevice()
	{
		HRESULT hr = E_FAIL;
		UINT createFactoryDebugFlags = 0;

#if defined CS_DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		CS_FATAL(SUCCEEDED(hr), "D3D디버그 인터페이스를 가져오는데 실패하였습니다.");
		debugController->EnableDebugLayer();
		createFactoryDebugFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		// #DirectX Create DXGI factory
		hr = CreateDXGIFactory2(createFactoryDebugFlags, IID_PPV_ARGS(m_Factory.GetAddressOf()));
		CS_FATAL(SUCCEEDED(hr), "DXGI Factory를 생성하는데 실패하였습니다.");

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
				m_Device->SetName(L"D3D12 Device");
				CS_FATAL(SUCCEEDED(hr), "D3D Device를 생성하는데 실패하였습니다.");

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

			D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };

			D3D12_MESSAGE_ID denyIDs[] = {
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE
			};

			D3D12_INFO_QUEUE_FILTER newFilter = {};
			newFilter.DenyList.NumSeverities = _countof(severities);
			newFilter.DenyList.pSeverityList = severities;
			newFilter.DenyList.NumIDs = _countof(denyIDs);
			newFilter.DenyList.pIDList = denyIDs;
			infoQueue->PushStorageFilter(&newFilter);
		}
#endif
		m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);

		m_CommandQueue = std::make_shared<CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	}

	void Renderer::RegisterRenderComponent(PrimitiveComponent* component)
	{
		
		// TODO : Material 별로 나누어야함
		const auto rcType = component->GetPrimitiveComponentType();
		switch (rcType)
		{
		case PrimitiveComponent::EPrimitiveComponentType::StaticMesh:
			m_StaticMeshComponents.push_back(static_cast<StaticMeshComponent*>(component));
			break;
		case PrimitiveComponent::EPrimitiveComponentType::SkeletalMesh:
			m_SkeletalMeshComponents.push_back(static_cast<SkeletalMeshComponent*>(component));
			break;
		case PrimitiveComponent::EPrimitiveComponentType::Collision: [[Fallthrough]]
		case PrimitiveComponent::EPrimitiveComponentType::Ray: [[Fallthrough]]
		case PrimitiveComponent::EPrimitiveComponentType::BoundingBox: [[Fallthrough]]
		case PrimitiveComponent::EPrimitiveComponentType::BoundingOrientedBox: [[Fallthrough]]
		case PrimitiveComponent::EPrimitiveComponentType::BoundingSphere:
			m_CollisionComponents.push_back(static_cast<CollisionComponent*>(component));
			break;
		default:
			CS_FATAL(false, "잘못된 PrimitiveComponent Type입니다.");
			break;
		}
	}

	void Renderer::CreateRenderTargetViewFromSwapChain()
	{
		// #DirectX Swap Chain Description
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = m_ResWidth;
		swapChainDesc.Height = m_ResHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// #DirectX Swap Chain FullScreen Description
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC  swapChainFullscreenDesc;
		swapChainFullscreenDesc.RefreshRate.Numerator = 60;
		swapChainFullscreenDesc.RefreshRate.Denominator = 1;
		swapChainFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainFullscreenDesc.Windowed = true;

		// #DirectX Create Swap Chain https://gamedev.stackexchange.com/questions/149822/direct3d-12-cant-create-a-swap-chain
		HRESULT hr = m_Factory->CreateSwapChainForHwnd(m_CommandQueue->GetRaw(), m_Window->GetWindowHandle(), &swapChainDesc,
			&swapChainFullscreenDesc, nullptr, m_SwapChain.GetAddressOf());
		CS_FATAL(SUCCEEDED(hr), "Swap Chain을 생성하는데 실패하였습니다");
		Renderer::Instance().GetFactory()->MakeWindowAssociation(m_Window->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);

		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_RenderTargetTextures[i] = std::make_unique<Texture>(rtvBuffer.Get(), D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_RenderTargetTextures[i]->CreateRenderTargetView(m_RenderTargetTextures[i]->GetResource()->GetDesc().Format,
				D3D12_RTV_DIMENSION_TEXTURE2D);
		}
	}

	void Renderer::CreateDepthStencilView()
	{
		m_DepthStencilBufferTexture = std::make_unique<Texture>(m_ResWidth, m_ResHeight, 1, 0,
			DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
			D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_DepthStencilBufferTexture->CreateDepthStencilView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_DSV_DIMENSION_TEXTURE2D);
	}

	void Renderer::ChangeDisplayMode()
	{
		BOOL bIsFullScreen = false;
		m_SwapChain->GetFullscreenState(&bIsFullScreen, nullptr);
		if (m_bIsFullScreen == (bool)bIsFullScreen)
			return;

		CS_INFO("디스플레이 모드를 변환중...");

		HRESULT hr = m_SwapChain->SetFullscreenState(m_bIsFullScreen, nullptr);
		CS_FATAL(SUCCEEDED(hr), "디스플레이모드를 변환하는데 실패하였습니다.");

		for (auto& rt : m_RenderTargetTextures)
		{
			rt.reset();
		}

		hr = m_SwapChain->ResizeBuffers(2, m_ResWidth, m_ResHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_FATAL(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");
		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_RenderTargetTextures[i] = std::make_unique<Texture>(rtvBuffer.Get(), D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_RenderTargetTextures[i]->CreateRenderTargetView(m_RenderTargetTextures[i]->GetResource()->GetDesc().Format,
				D3D12_RTV_DIMENSION_TEXTURE2D);
		}

		DXGI_MODE_DESC targetParam = {};
		targetParam.Width = m_ResWidth;
		targetParam.Height = m_ResHeight;
		targetParam.RefreshRate.Numerator = 0;
		targetParam.RefreshRate.Denominator = 1;
		targetParam.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		targetParam.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		targetParam.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		hr = m_SwapChain->ResizeTarget(&targetParam);
		CS_FATAL(SUCCEEDED(hr), "타겟을 Resize하는데 실패하였습니다.");

		m_RtvIndex = 0;

		CS_INFO("디스플레이 모드를 변환 완료.");
		if (m_bIsFullScreen)
			CS_INFO("현재 디스플레이 모드 : 전체화면 모드");
		else
			CS_INFO("현재 디스플레이 모드 : 창 모드");
	}
}