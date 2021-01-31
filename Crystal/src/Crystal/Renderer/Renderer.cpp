#include "cspch.h"
#include "Renderer.h"

#include "DirectXTex/DirectXTex.h"
#include <iostream>

#include "Crystal/Resources/ShaderManager.h"
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "../Core/ApplicationUtility.h"
#include "../GamePlay/Actors/CameraPawn.h"
#include "Material.h"

#include "imgui/backends/imgui_impl_dx12.h"
#include "imgui/backends/imgui_impl_win32.h"

namespace Crystal {
	void Renderer::Init(WindowsWindow* window)
	{
		m_Window = window;

		CreateDevice();
		CreateRenderTargetViewFromSwapChain();
		CreateDepthStencilView();
		LoadResources();
		CreatePipelineStates();
		CreateComputePipelineStates();
		
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 1 + 4 + 5 + 1000;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_CommonDescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


		descriptorHeapDesc.NumDescriptors = 1;
		hr = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_CubemapDescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");

		descriptorHeapDesc.NumDescriptors = 2;
		hr = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_EquiToCubeDescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");

		descriptorHeapDesc.NumDescriptors = 2;
		hr = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DiffuseIrradianceSamplingDescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");



		/*ImGui 구현*/
		{
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
		


		float quadVertices[] = {
			-1.0f, -1.0f,
			-1.0f, +1.0f,
			+1.0f, +1.0f,
			+1.0f, -1.0f
		};

		uint32_t quadIndices[] = {
			0, 1, 2,
			0, 2, 3
		};

		m_QuadVertexBuffer = std::make_unique<VertexBuffer>((void*)quadVertices, (UINT)(sizeof(float) * 2), (UINT)_countof(quadVertices));
		m_QuadIndexBuffer = std::make_unique<IndexBuffer>((void*)quadIndices, (UINT)(sizeof(uint32_t) * _countof(quadIndices)), (UINT)(_countof(quadIndices)));


		
		/*Compute Pipeline Resource를 복사합니다.*/
		D3D12_CPU_DESCRIPTOR_HANDLE equiToCubeDestHandle = m_EquiToCubeDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_Device->CopyDescriptorsSimple(1, equiToCubeDestHandle, m_EquirectangularTexture->GetShaderResourceView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		equiToCubeDestHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_Device->CopyDescriptorsSimple(1, equiToCubeDestHandle, m_CubemapTexture->GetUnorderedAccessView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		
		D3D12_CPU_DESCRIPTOR_HANDLE irradianceSampling = m_DiffuseIrradianceSamplingDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_Device->CopyDescriptorsSimple(1, irradianceSampling, m_CubemapTexture->GetShaderResourceView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		irradianceSampling.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_Device->CopyDescriptorsSimple(1, irradianceSampling, m_IrradiancemapTexture->GetUnorderedAccessView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		
				/// COMPUTE
		{
			auto cmdList = m_CommandQueue->GetCommandList();

			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_CubemapTexture->GetResource();
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			cmdList->ResourceBarrier(1, &resourceBarrier);

			/*Equi To Cube*/
			cmdList->SetPipelineState(m_EquiToCubePipeline.Get());
			cmdList->SetComputeRootSignature(m_EquiToCubeRootSignature.Get());
			ID3D12DescriptorHeap* equiToCubeHeaps[] = { m_EquiToCubeDescriptorHeap.Get() };
			cmdList->SetDescriptorHeaps(_countof(equiToCubeHeaps), equiToCubeHeaps);
			cmdList->SetComputeRootDescriptorTable(0, m_EquiToCubeDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			auto desc = m_CubemapTexture->GetResource()->GetDesc();
			cmdList->Dispatch((UINT)(desc.Width / 32), (UINT)(desc.Height / 32), 6);

			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			cmdList->ResourceBarrier(1, &resourceBarrier);

			resourceBarrier.Transition.pResource = m_IrradiancemapTexture->GetResource();
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			cmdList->ResourceBarrier(1, &resourceBarrier);

			/*DiffuseIrradiance Sample*/
			cmdList->SetPipelineState(m_DiffuseIrradianceSamplingPipeline.Get());
			cmdList->SetComputeRootSignature(m_DiffuseIrradianceSamplingRootSignature.Get());
			ID3D12DescriptorHeap* irradianceSamplingHeaps[] = { m_DiffuseIrradianceSamplingDescriptorHeap.Get() };
			cmdList->SetDescriptorHeaps(_countof(irradianceSamplingHeaps), irradianceSamplingHeaps);
			cmdList->SetComputeRootDescriptorTable(0, m_DiffuseIrradianceSamplingDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			cmdList->Dispatch((UINT)(m_IrradiancemapTexture->GetResource()->GetDesc().Width / 32), (UINT)(m_IrradiancemapTexture->GetResource()->GetDesc().Height / 32), 6);

			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			cmdList->ResourceBarrier(1, &resourceBarrier);

			m_CommandQueue->Execute(cmdList);
			CS_INFO("HDRI로부터 Cubemap 생성중...");
			m_CommandQueue->Flush();
			CS_INFO("HDRI로부터 Cubemap 생성 완료");
		}


		m_LightingPipeline = std::make_unique<LightingPipeline>("PBRLightingPipeline");
		m_CubemapPipeline = std::make_unique<CubemapPipeline>("CubemapPipeline");
		m_ClearPipeline = std::make_unique<ClearPipeline>("ClearPipeline");

	}

	void Renderer::Render()
	{
		ChangeResolution(m_ResolutionItems[m_CurrentResolutionIndex]);
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

		ClearPipeline::ClearPipelineInputs clearPipelineInputs = {};
		clearPipelineInputs.RenderTargets[0] = m_RenderTargetTextures[m_RtvIndex].get();
		clearPipelineInputs.ClearColorValue = { 1.0f, 0.0f, 1.0f, 0.0f };
		clearPipelineInputs.DepthStencilBuffer = m_DepthStencilBufferTexture.get();
		clearPipelineInputs.bClearDepth = true;
		clearPipelineInputs.ClearDepthValue = 1.0f;
		clearPipelineInputs.bClearStencil = true;
		clearPipelineInputs.ClearStencilValue = 0.0f;
		m_ClearPipeline->Begin(&clearPipelineInputs);
		m_ClearPipeline->Record(commandList);
		m_ClearPipeline->End();

		LightingPipeline::LightingPipelineInputs lightingPipelineInputs = {};
		lightingPipelineInputs.RenderComponents = &m_RenderComponents;
		lightingPipelineInputs.Camera = ApplicationUtility::GetPlayerController().GetMainCamera();
		lightingPipelineInputs.RenderTargets[0] = clearPipelineInputs.RenderTargets[0];
		lightingPipelineInputs.DepthStencilBuffer = clearPipelineInputs.DepthStencilBuffer;
		lightingPipelineInputs.IrradiancemapTexture = m_IrradiancemapTexture.get();
		m_LightingPipeline->Begin(&lightingPipelineInputs);
		m_LightingPipeline->Record(commandList);
		m_LightingPipeline->End();


		CubemapPipeline::CubemapPipelineInputs cubemapPipelineInputs = {};
		cubemapPipelineInputs.Camera = ApplicationUtility::GetPlayerController().GetMainCamera();
		cubemapPipelineInputs.RenderTargets[0] = lightingPipelineInputs.RenderTargets[0];
		cubemapPipelineInputs.DepthStencilBuffer = lightingPipelineInputs.DepthStencilBuffer;
		cubemapPipelineInputs.CubemapTexture = m_CubemapTexture.get();
		m_CubemapPipeline->Begin(&cubemapPipelineInputs);
		m_CubemapPipeline->Record(commandList);
		m_CubemapPipeline->End();

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

	void Renderer::ChangeResolution(const char* formattedResolution)
	{
		// 0. Parse String
		// 1. Clear Reference of RenderTarget
		// 2. Destruct DepthStencil
		// 3. Resize
		// 4. Set Camera's Viewport And ScissorRect
		// 5. Make Sure Current Index of RenderTarget is BackBuffer
		bool bCompleteGetWidth = false;
		char widthInChar[100] = {};
		int widthIndex = 0;
		char heightInChar[100] = {};
		int heightIndex = 0;

		for (int i = 0; formattedResolution[i]; i++)
		{
			if (formattedResolution[i] == 'x')
			{
				bCompleteGetWidth = true;
				continue;
			}

			if (!bCompleteGetWidth)
				widthInChar[widthIndex++] = formattedResolution[i];
			else
				heightInChar[heightIndex++] = formattedResolution[i];
		}

		const int width = atoi(widthInChar);
		const int height = atoi(heightInChar);
		if (width == m_ResWidth && height == m_ResHeight && m_Window->GetWidth() == m_ResWidth && m_Window->GetHeight() == m_ResHeight)
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
			m_RenderTargetTextures[i]->CreateRenderTargetView(m_RenderTargetTextures[i]->GetResource()->GetDesc().Format, D3D12_RTV_DIMENSION_TEXTURE2D);
		}
		m_DepthStencilBufferTexture = std::make_unique<Texture>(m_ResWidth, m_ResHeight, 1, 0, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_DepthStencilBufferTexture->CreateDepthStencilView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_DSV_DIMENSION_TEXTURE2D);

		auto cameraComponent = ApplicationUtility::GetPlayerController().GetMainCamera();
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

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			NewFilter.DenyList.NumSeverities = _countof(severities);
			NewFilter.DenyList.pSeverityList = severities;
			NewFilter.DenyList.NumIDs = _countof(denyIDs);
			NewFilter.DenyList.pIDList = denyIDs;
			infoQueue->PushStorageFilter(&NewFilter);
		}
#endif
		m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		m_FenceEvent = CreateEvent(nullptr, false, false, nullptr);

		m_CommandQueue = std::make_shared<CommandQueue>(m_Device, D3D12_COMMAND_LIST_TYPE_DIRECT);

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
			m_RenderTargetTextures[i]->CreateRenderTargetView(m_RenderTargetTextures[i]->GetResource()->GetDesc().Format, D3D12_RTV_DIMENSION_TEXTURE2D);
		}
	}

	void Renderer::CreateDepthStencilView()
	{
		m_DepthStencilBufferTexture = std::make_unique<Texture>(m_ResWidth, m_ResHeight, 1, 0, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, 
			D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_DepthStencilBufferTexture->CreateDepthStencilView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_DSV_DIMENSION_TEXTURE2D);
	}

	

	void Renderer::CreatePipelineStates()
	{
		///////////////////////////////////////////////////
		//////////////ROOT SIGNATURE /////////////////////
		///////////////////////////////////////////////////
		
		CD3DX12_ROOT_PARAMETER1 rootParameter[3] = {};
		rootParameter[0].InitAsConstantBufferView(0);

		CD3DX12_DESCRIPTOR_RANGE1 globalRootParamRanges[1] = {};
		globalRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		rootParameter[1].InitAsDescriptorTable(_countof(globalRootParamRanges), globalRootParamRanges);

		CD3DX12_DESCRIPTOR_RANGE1 perObjectRootParamRanges[2] = {};
		perObjectRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
		perObjectRootParamRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // VOLATILE로 설정해야 모든 Descriptor가 없어도 렌더링을 진행할 수 있음.
		rootParameter[2].InitAsDescriptorTable(_countof(perObjectRootParamRanges), perObjectRootParamRanges);

		CD3DX12_STATIC_SAMPLER_DESC StaticSamplerDescs[1] = {};
		StaticSamplerDescs[0].Init(0);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootsigDesc;
		rootsigDesc.Init_1_1(_countof(rootParameter), rootParameter, _countof(StaticSamplerDescs), StaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootsigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다 %s", rootSignatureErrorBlob->GetBufferPointer());
		hr = m_Device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_NormalRootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");


		D3D12_INPUT_ELEMENT_DESC staticInputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};

		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopology;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DSV;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream, cubemapPipelineStream;

		////////////////////////////////////////////////////
		////////PIPELINE STATE//////////////////////////////
		////////////////////////////////////////////////////

		pipelineStateStream.RootSignature = m_NormalRootSignature.Get();
		pipelineStateStream.InputLayout = { staticInputLayout, _countof(staticInputLayout) };
		pipelineStateStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		auto& shaderManager = ShaderManager::Instance();
		auto& shaderDatablobs = shaderManager.GetShader("PBRShader")->GetRaw();
		pipelineStateStream.VS = { shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(), shaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
		pipelineStateStream.PS = { shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(), shaderDatablobs[ShaderType::Pixel]->GetBufferSize() };

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		depthStencilDesc.StencilEnable = false;
		depthStencilDesc.StencilReadMask = 0x00;
		depthStencilDesc.StencilWriteMask = 0x00;
		depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
		depthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

		pipelineStateStream.DSV = CD3DX12_DEPTH_STENCIL_DESC(depthStencilDesc);
		pipelineStateStream.DSVFormat = m_DepthStencilBufferTexture->GetResource()->GetDesc().Format;
		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineStateStream.RTVFormats = rtvFormat;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };
//		hr = m_Device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PBRPipelineState));
		//CS_ASSERT(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");

		///////////////////////////////////////////////////
		//////////////ROOT SIGNATURE /////////////////////
		///////////////////////////////////////////////////

		CD3DX12_DESCRIPTOR_RANGE1 cubemapCommonDescriptorHeapRanges[1] = {};
		cubemapCommonDescriptorHeapRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER1 cubemapRootParameter[2] = {};
		cubemapRootParameter[0].InitAsConstantBufferView(0);
		cubemapRootParameter[1].InitAsDescriptorTable(_countof(cubemapCommonDescriptorHeapRanges), cubemapCommonDescriptorHeapRanges);

		CD3DX12_STATIC_SAMPLER_DESC cubemapStaticSamplerDescs[1] = {};
		cubemapStaticSamplerDescs[0].Init(0);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC cubemapRootsigDesc(_countof(cubemapRootParameter), cubemapRootParameter, _countof(cubemapStaticSamplerDescs), cubemapStaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		rootSignatureDataBlob = nullptr;
		rootSignatureErrorBlob = nullptr;
		hr = D3D12SerializeVersionedRootSignature(&cubemapRootsigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
		hr = m_Device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_CubemapRootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");

		D3D12_INPUT_ELEMENT_DESC cubemapInputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		////////////////////////////////////////////////////
		////////PIPELINE STATE//////////////////////////////
		////////////////////////////////////////////////////

		cubemapPipelineStream.RootSignature = m_CubemapRootSignature.Get();
		cubemapPipelineStream.InputLayout = { cubemapInputLayout, _countof(cubemapInputLayout) };
		cubemapPipelineStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		auto& cubemapShaderDatablobs = shaderManager.GetShader("CubemapShader")->GetRaw();
		cubemapPipelineStream.VS = { cubemapShaderDatablobs[ShaderType::Vertex]->GetBufferPointer(), cubemapShaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
		cubemapPipelineStream.PS = { cubemapShaderDatablobs[ShaderType::Pixel]->GetBufferPointer(), cubemapShaderDatablobs[ShaderType::Pixel]->GetBufferSize() };
		cubemapPipelineStream.DSV = CD3DX12_DEPTH_STENCIL_DESC(depthStencilDesc);
		cubemapPipelineStream.DSVFormat = m_DepthStencilBufferTexture->GetResource()->GetDesc().Format;
		D3D12_RT_FORMAT_ARRAY cubemapRtvFormat = {};
		cubemapRtvFormat.NumRenderTargets = 1;
		cubemapRtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		cubemapPipelineStream.RTVFormats = cubemapRtvFormat;
		D3D12_PIPELINE_STATE_STREAM_DESC cubemapPipelineStreamDesc = { sizeof(cubemapPipelineStream), &cubemapPipelineStream };

		hr = m_Device->CreatePipelineState(&cubemapPipelineStreamDesc, IID_PPV_ARGS(&m_CubemapPipelineState));
		CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");
		

		
		////////////////////////////////////////////////////
		////////PIPELINE STATE//////////////////////////////
		////////////////////////////////////////////////////

		D3D12_INPUT_ELEMENT_DESC animatedInputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		pipelineStateStream.InputLayout = { animatedInputLayout, _countof(animatedInputLayout) };
		hr = m_Device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PbrAnimatedPipelineState));
		CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");


	}

	void Renderer::CreateComputePipelineStates()
	{
		{
			CD3DX12_DESCRIPTOR_RANGE1 descHeapRange[2] = {};
			descHeapRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			descHeapRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

			CD3DX12_ROOT_PARAMETER1 rootParameters[1] = {};
			rootParameters[0].InitAsDescriptorTable(_countof(descHeapRange), descHeapRange);


			CD3DX12_STATIC_SAMPLER_DESC computeStaticSampler[1] = {};
			computeStaticSampler[0].Init(0);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC computeRootSig(_countof(rootParameters), rootParameters, _countof(computeStaticSampler), computeStaticSampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
			HRESULT hr = D3D12SerializeVersionedRootSignature(&computeRootSig, &rootSignatureDataBlob, &rootSignatureErrorBlob);
			CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
			hr = m_Device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_EquiToCubeRootSignature));
			CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");

			auto& shaderManager = ShaderManager::Instance();
			auto& shaderDatablobs = shaderManager.GetShader("EquiToCube")->GetRaw();
			D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc = {};
			computePipelineStateDesc.pRootSignature = m_EquiToCubeRootSignature.Get();
			computePipelineStateDesc.CS = { shaderDatablobs[ShaderType::Compute]->GetBufferPointer(), shaderDatablobs[ShaderType::Compute]->GetBufferSize() };

			hr = m_Device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&m_EquiToCubePipeline));
			CS_FATAL(SUCCEEDED(hr), "PipelineState를 생성하는데 실패하였습니다.");
		}

		{

			CD3DX12_DESCRIPTOR_RANGE1 descHeapRange[2] = {};
			descHeapRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			descHeapRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

			CD3DX12_ROOT_PARAMETER1 rootParameters[1] = {};
			rootParameters[0].InitAsDescriptorTable(_countof(descHeapRange), descHeapRange);


			CD3DX12_STATIC_SAMPLER_DESC computeStaticSampler[1] = {};
			computeStaticSampler[0].Init(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC computeRootSig(_countof(rootParameters), rootParameters, _countof(computeStaticSampler), computeStaticSampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
			HRESULT hr = D3D12SerializeVersionedRootSignature(&computeRootSig, &rootSignatureDataBlob, &rootSignatureErrorBlob);
			CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
			hr = m_Device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_DiffuseIrradianceSamplingRootSignature));
			CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");

			auto& shaderManager = ShaderManager::Instance();
			auto& shaderDatablobs = shaderManager.GetShader("DiffuseIrradianceSampling")->GetRaw();
			D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc = {};
			computePipelineStateDesc.pRootSignature = m_DiffuseIrradianceSamplingRootSignature.Get();
			computePipelineStateDesc.CS = { shaderDatablobs[ShaderType::Compute]->GetBufferPointer(), shaderDatablobs[ShaderType::Compute]->GetBufferSize() };

			hr = m_Device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&m_DiffuseIrradianceSamplingPipeline));
			CS_FATAL(SUCCEEDED(hr), "PipelineState를 생성하는데 실패하였습니다.");
		}

		
	}

	void Renderer::LoadResources()
	{
		///////////////////////////////////////////////////
		/////LOAD SHADER, TEXTURE, CONSTANT BUFFER///////
		///////////////////////////////////////////////////

		auto& shaderManager = ShaderManager::Instance();

		/*텍스쳐 리소스를 로드합니다.*/
		m_EquirectangularTexture = std::make_unique<Texture>("assets/textures/cubemaps/pink_sunrise_4k.hdr");
		//m_EquirectangularTexture = std::make_unique<Texture>("assets/textures/cubemaps/debughdri.png");
		m_CubemapTexture = std::make_unique<Texture>(2048, 2048, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		m_IrradiancemapTexture = std::make_unique<Texture>(32, 32, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		/*각 텍스쳐 리소스에 대한 Shader Resource View를 만듭니다.*/
		m_EquirectangularTexture->CreateShaderResourceView(m_EquirectangularTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);
		m_CubemapTexture->CreateUnorderedAccessView(m_CubemapTexture->GetResource()->GetDesc().Format, D3D12_UAV_DIMENSION_TEXTURE2DARRAY);
		m_CubemapTexture->CreateShaderResourceView(m_CubemapTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURECUBE);
		m_IrradiancemapTexture->CreateUnorderedAccessView(m_IrradiancemapTexture->GetResource()->GetDesc().Format, D3D12_UAV_DIMENSION_TEXTURE2DARRAY);
		m_IrradiancemapTexture->CreateShaderResourceView(m_IrradiancemapTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURECUBE);
		
		shaderManager.Load("assets/shaders/PBRShader.hlsl", "PBRShader");
		shaderManager.Load("assets/shaders/SkyboxShader.hlsl", "CubemapShader");
		shaderManager.Load("assets/shaders/EquirectangularToCube.hlsl", "EquiToCube");
		shaderManager.Load("assets/shaders/DiffuseIrradianceSampling.hlsl", "DiffuseIrradianceSampling");
		shaderManager.Load("assets/shaders/SpecularIrradianceSampling.hlsl", "SpecularIrradianceSampling");
		
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

		hr = m_SwapChain->ResizeBuffers(2, m_ResWidth, m_ResHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_FATAL(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");
		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_RenderTargetTextures[i] = std::make_unique<Texture>(rtvBuffer.Get(), D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_RenderTargetTextures[i]->CreateRenderTargetView(m_RenderTargetTextures[i]->GetResource()->GetDesc().Format, D3D12_RTV_DIMENSION_TEXTURE2D);
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
		if(m_bIsFullScreen)
			CS_INFO("현재 디스플레이 모드 : 전체화면 모드");
		else
			CS_INFO("현재 디스플레이 모드 : 창 모드");
	}
}