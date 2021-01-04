#include "cspch.h"
#include "Renderer.h"

#include "RenderTargetView.h"
#include "DepthStencilView.h"

#include "DirectXTex/DirectXTex.h"
#include <iostream>


#include "Crystal/GamePlay/World/World.h"

#include "Crystal/AssetManager/ShaderManager.h"
#include "Crystal/AssetManager/TextureManager.h"
#include "Crystal/AssetManager/ConstantBuffer.h"
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "../Core/ApplicationUtility.h"
#include "../GamePlay/Actors/CameraPawn.h"

namespace Crystal {
	void Renderer::Init(WindowsWindow* window)
	{
		m_Window = window;

		createDeviceContext();
		createRenderTargetViewFromSwapChain();
		createDepthStencilView();
		loadResources();
		createPipelineStates();


		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 1 + 4 + 5 + 1000;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_CommonDescriptorHeap));
		CS_ASSERT(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");

		

		////////////////////////////////////////////////////
		////////TEST OBJECTS///////////////////////////////
		///////////////////////////////////////////////////

		m_World = new World();
		Pawn* pawn = m_World->SpawnActor<Pawn>();
		CameraPawn* cameraPawn = m_World->SpawnActor<CameraPawn>();
		ApplicationUtility::GetPlayerController().Possess(cameraPawn);

		ApplicationUtility::GetPlayerController().AddAxisMapping("MoveForward", Keyboard::W, 1.0f);
		ApplicationUtility::GetPlayerController().AddAxisMapping("MoveForward", Keyboard::S, -1.0f);
		ApplicationUtility::GetPlayerController().AddAxisMapping("MoveRight", Keyboard::D, 1.0f);
		ApplicationUtility::GetPlayerController().AddAxisMapping("MoveRight", Keyboard::A, -1.0f);
		ApplicationUtility::GetPlayerController().AddAxisMapping("Turn", Mouse::X, 1.0f);
		ApplicationUtility::GetPlayerController().AddAxisMapping("LookUp", Mouse::Y, 1.0f);

		ActionMapping actionKey = {};
		actionKey.CrystalCode = Keyboard::Left;
		actionKey.bAltDown = true;
		actionKey.bCtrlDown = true;
		actionKey.bShiftDown = true;
		ApplicationUtility::GetPlayerController().AddActionMapping("Jump", actionKey);



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
	}

	void Renderer::Render()
	{
		//////////////////////////////////////////
		///// UPDATE /////////////////////////////
		//////////////////////////////////////////

		static float angle = 0.0f;
		angle += timer.DeltaTime();
		//m_Camera->SetLookAt(DirectX::XMFLOAT3(0.0f, 0.0f, 0 + cos(angle)));

		ChangeResolution(m_ResolutionItems[m_CurrentResolutionIndex]);
		ChangeDisplayMode();

		//TEMP///
		////////////

		timer.Tick();
		m_World->Update(timer.DeltaTime());
		static float modelAngle[] = { 0.0f, 90.0f, 0.0f };
		modelAngle[1] += 10.0f * timer.DeltaTime();
		if (modelAngle[1] > 359.0f)
			modelAngle[1] = 0.0f;

		XMStoreFloat4x4(&m_WorldMat, DirectX::XMMatrixIdentity());
		//XMStoreFloat4x4(&m_WorldMat, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMat), DirectX::XMMatrixTranslation(2.0f, 3.0f, 4.0f)));

		//XMStoreFloat4x4(&m_WorldMat, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMat),
		//	DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(modelAngle[0]), DirectX::XMConvertToRadians(modelAngle[1]), DirectX::XMConvertToRadians(modelAngle[2]))));
		//XMStoreFloat4x4(&m_WorldMat, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMat)));

		auto cameraComponent = ApplicationUtility::GetPlayerController().GetMainCamera();
		
		m_PerFrameData.ViewProjection = Matrix4x4::Transpose(cameraComponent->GetViewProjection());
		auto camPos = cameraComponent->GetWorldPosition();
		m_PerFrameData.CameraPositionInWorld = DirectX::XMFLOAT4(camPos.x, camPos.y, camPos.z, 0.0f);
		m_PerFrameData.LightPositionInWorld = DirectX::XMFLOAT4(1000.0f, 1000.0f, 0.0f, 0.0f);
		m_PerObjectData.World = m_WorldMat;

		m_PerFrameBuffer.SetData((void*)&m_PerFrameData);
		m_PerObjectBuffer.SetData((void*)&m_PerObjectData);

		auto viewProj = cameraComponent->GetViewProjection();
		viewProj._41 = 0.0f; viewProj._42 = 0.0f; viewProj._43 = 0.0f;
		m_CubemapCbuffer.SetData((void*)&Matrix4x4::Transpose(Matrix4x4::Inverse(viewProj)));
	

		//////////////////////////////////////////
		///// RENDER /////////////////////////////
		//////////////////////////////////////////


		{
			// Copying Per Object Datas
			/*D3D12_CPU_DESCRIPTOR_HANDLE baseHandle = m_CommonDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			D3D12_CPU_DESCRIPTOR_HANDLE cbVdestStartHandles[] = { baseHandle };
			UINT cbVdestRangeSize[] = { 1 };
			D3D12_CPU_DESCRIPTOR_HANDLE cbVSrcStartHandles[] = { m_PerObjectBuffer.GetCpuDescriptorHandle() };
			UINT cbVSrcRangeSize[] = { 1 };
			m_Device->CopyDescriptors(_countof(cbVdestStartHandles), cbVdestStartHandles, cbVdestRangeSize,
				_countof(cbVSrcStartHandles), cbVSrcStartHandles, cbVSrcRangeSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);*/

			m_Device->CopyDescriptorsSimple(1, m_CommonDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_PerObjectBuffer.GetCpuDescriptorHandle(),
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			{
				struct SRVMaterial
				{
					D3D12_CPU_DESCRIPTOR_HANDLE srcHandle;
					UINT numDescriptor;
				};

				struct Material
				{
					std::string ShaderName;// Or Shader Itself
					ConstantBuffer CBuffer = {};
				};
			}

			//Copy Descriptors in Texture pool
			auto& textureManager = TextureManager::Instance();

			D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_CommonDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			destHeapHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			auto& texture = textureManager.GetTexture("MegaphoneMaterial");
			m_Device->CopyDescriptorsSimple(texture.Count, destHeapHandle, texture.CpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 4;

			auto& cubemapTexture = textureManager.GetTexture("CubemapMaterial");
			m_Device->CopyDescriptorsSimple(cubemapTexture.Count, destHeapHandle, cubemapTexture.CpuHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		auto cmdList = m_CommandQueue->GetCommandList();

		cmdList->SetPipelineState(m_PBRPipelineState.Get());
		cmdList->SetGraphicsRootSignature(m_RootSignature->GetRaw());

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->RSSetViewports(1, &cameraComponent->GetViewport());
		cmdList->RSSetScissorRects(1, &cameraComponent->GetScissorRect());
		cmdList->OMSetRenderTargets(1, &m_RenderTargetViews[m_RtvIndex]->GetCpuHandle(), TRUE, &m_DepthStencil->GetCpuHandle());


		Resource* rtvResource = m_RenderTargetViews[m_RtvIndex]->GetResource();
		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = rtvResource->GetRaw();
		resourceBarrier.Transition.StateBefore = rtvResource->GetResourceState();
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cmdList->ResourceBarrier(1, &resourceBarrier);
		rtvResource->SetResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET);

		

		cmdList->ClearRenderTargetView(m_RenderTargetViews[m_RtvIndex]->GetCpuHandle(), m_ClearColor, 0, nullptr);
		cmdList->ClearDepthStencilView(m_DepthStencil->GetCpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		cmdList->SetGraphicsRootConstantBufferView(0, m_PerFrameBuffer.GetGpuVirtualAddress());
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_CommonDescriptorHeap.Get() };
		cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		cmdList->SetGraphicsRootDescriptorTable(1, m_CommonDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		/*D3D12_GPU_DESCRIPTOR_HANDLE hCommon = m_CommonDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		hCommon.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		cmdList->SetGraphicsRootDescriptorTable(0, hCommon);*/

		for (const auto& meshComponent : m_MeshComponents)
		{
			DirectX::XMFLOAT4X4 world = meshComponent->GetTransform();
			m_PerObjectData.World = world;
			meshComponent->GetMesh()->Render(cmdList);
		}

		//---------------Cubemap
		cmdList->SetPipelineState(m_CubemapPipelineState.Get());
		cmdList->SetGraphicsRootSignature(m_CubemapRootSignature->GetRaw());
		cmdList->SetGraphicsRootConstantBufferView(0, m_CubemapCbuffer.GetGpuVirtualAddress());
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_CommonDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;
		cmdList->SetGraphicsRootDescriptorTable(1, handle);
		cmdList->IASetVertexBuffers(0, 1, &m_QuadVertexBuffer->GetView());
		cmdList->IASetIndexBuffer(&m_QuadIndexBuffer->GetView());
		cmdList->DrawIndexedInstanced(m_QuadIndexBuffer->GetCount(), 1, 0, 0, 0);

		rtvResource = m_RenderTargetViews[m_RtvIndex]->GetResource();
		resourceBarrier.Transition.pResource = rtvResource->GetRaw();
		resourceBarrier.Transition.StateBefore = rtvResource->GetResourceState();
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cmdList->ResourceBarrier(1, &resourceBarrier);
		rtvResource->SetResourceState(D3D12_RESOURCE_STATE_PRESENT);
	
		m_CommandQueue->Execute(cmdList);

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

		int width = atoi(widthInChar);
		int height = atoi(heightInChar);
		if (width == m_ResWidth && height == m_ResHeight && m_Window->GetWidth() == m_ResWidth && m_Window->GetHeight() == m_ResHeight)
			return;
		
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
		CS_ASSERT(SUCCEEDED(hr), "타겟을 Resize하는데 실패하였습니다.");

		for (auto& rt : m_RenderTargetViews)
		{
			rt.reset();
		}
		m_RenderTargetViews.clear();

		hr = m_SwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_ASSERT(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");

		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_RenderTargetViews.push_back(std::make_unique<RenderTargetView>(rtvBuffer.Get()));
		}
		m_DepthStencilBuffer.Reset();
		createDepthStencilView();

		auto cameraComponent = ApplicationUtility::GetPlayerController().GetMainCamera();
		cameraComponent->SetViewport({ 0,0, (FLOAT)width, (FLOAT)height, 0.0f, 1.0f });
		cameraComponent->SetScissorRect({ 0, 0, width, height });

		m_Window->SetWidth(targetParam.Width);
		m_Window->SetHeight(targetParam.Height);

	}

	Renderer::~Renderer()
	{
#if defined CS_DEBUG
		Microsoft::WRL::ComPtr<IDXGIDebug1> pdxgiDebug = nullptr;
		HRESULT hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pdxgiDebug));
		CS_ASSERT(SUCCEEDED(hr), "DXGI디버그 인터페이스를 가져오는데 실패하였습니다.");
		hr = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
#endif
	}

	void Renderer::createDeviceContext()
	{
		HRESULT hr = E_FAIL;
		UINT createFactoryDebugFlags = 0;

#if defined CS_DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		CS_ASSERT(SUCCEEDED(hr), "D3D디버그 인터페이스를 가져오는데 실패하였습니다.");
		debugController->EnableDebugLayer();

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

	}

	void Renderer::createRenderTargetViewFromSwapChain()
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
		swapChainFullscreenDesc.RefreshRate.Numerator = 0;
		swapChainFullscreenDesc.RefreshRate.Denominator = 1;
		swapChainFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainFullscreenDesc.Windowed = true;

		// #DirectX Create Swap Chain https://gamedev.stackexchange.com/questions/149822/direct3d-12-cant-create-a-swap-chain
		HRESULT hr = m_Factory->CreateSwapChainForHwnd(m_CommandQueue->GetRaw(), m_Window->GetWindowHandle(), &swapChainDesc,
			&swapChainFullscreenDesc, nullptr, m_SwapChain.GetAddressOf());
		CS_ASSERT(SUCCEEDED(hr), "Swap Chain을 생성하는데 실패하였습니다");
		Renderer::Instance().GetFactory()->MakeWindowAssociation(m_Window->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);

		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_RenderTargetViews.push_back(std::make_unique<RenderTargetView>(rtvBuffer.Get()));
		}

	}

	void Renderer::createDepthStencilView()
	{
		// #DirectX Depth / Stencil View heap properties
		D3D12_HEAP_PROPERTIES dsvHeapProperties = {};
		dsvHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		dsvHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		dsvHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		dsvHeapProperties.CreationNodeMask = 1;
		dsvHeapProperties.VisibleNodeMask = 1;

		// #DirectX Depth / Stencil View description
		D3D12_RESOURCE_DESC dsvResourcDesc = {};
		dsvResourcDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		dsvResourcDesc.Alignment = 0;
		dsvResourcDesc.Width = m_ResWidth;
		dsvResourcDesc.Height = m_ResHeight;
		dsvResourcDesc.DepthOrArraySize = 1;
		dsvResourcDesc.MipLevels = 0;
		dsvResourcDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvResourcDesc.SampleDesc.Count = 1;
		dsvResourcDesc.SampleDesc.Quality = 0;
		dsvResourcDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		dsvResourcDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		// #DirectX Depth / Stencil View Clear Value
		D3D12_CLEAR_VALUE dsvClearValue = {};
		dsvClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvClearValue.DepthStencil.Depth = 1.0f;
		dsvClearValue.DepthStencil.Stencil = 0;


		// #DirectX Create Depth / Stencil Buffer
		HRESULT hr = m_Device->CreateCommittedResource(&dsvHeapProperties, D3D12_HEAP_FLAG_NONE, &dsvResourcDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, &dsvClearValue, IID_PPV_ARGS(&m_DepthStencilBuffer));
		CS_ASSERT(SUCCEEDED(hr), "DepthStencil 리소스를 만드는데 실패하였습니다");

		m_DepthStencil = std::make_unique<DepthStencilView>(m_DepthStencilBuffer.Get());
	}

	

	void Renderer::createPipelineStates()
	{
		///////////////////////////////////////////////////
		//////////////ROOT SIGNATURE /////////////////////
		///////////////////////////////////////////////////

		CD3DX12_ROOT_PARAMETER1 rootParameter[2];
		rootParameter[0].InitAsConstantBufferView(0);

		CD3DX12_DESCRIPTOR_RANGE1 commonDescriptorHeapRanges[2] = {};
		commonDescriptorHeapRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
		commonDescriptorHeapRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 0);
		rootParameter[1].InitAsDescriptorTable(_countof(commonDescriptorHeapRanges), commonDescriptorHeapRanges);
		// [0] Root Constant Buffer View     Per Frame Data ( CameraPosition, LightPosition etc.)
		// [1] Root Descriptor Table          [1] Object1 ( Transform, Material )
		//								          [2] Object2 ( Transform, Material )
		//								          [3] Object3 ( Transform, Material )

		CD3DX12_STATIC_SAMPLER_DESC StaticSamplerDescs[1] = {};
		StaticSamplerDescs[0].Init(0);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootsigDesc;
		rootsigDesc.Init_1_1(_countof(rootParameter), rootParameter, _countof(StaticSamplerDescs), StaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DSV;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream, cubemapPipelineStream;

		////////////////////////////////////////////////////
		////////PIPELINE STATE//////////////////////////////
		////////////////////////////////////////////////////

		pipelineStateStream.RootSignature = m_RootSignature->GetRaw();
		pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
		pipelineStateStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		auto& shaderManager = ShaderManager::Instance();
		auto& shaderDatablobs = shaderManager.GetShader("PBRShader")->GetRaw();
		pipelineStateStream.VS = { shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(), shaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
		pipelineStateStream.PS = { shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(), shaderDatablobs[ShaderType::Pixel]->GetBufferSize() };

		pipelineStateStream.DSV = CD3DX12_DEPTH_STENCIL_DESC(m_DepthStencil->GetDesc());
		pipelineStateStream.DSVFormat = m_DepthStencil->GetFormat();
		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = m_RenderTargetViews[0]->GetFormat();
		pipelineStateStream.RTVFormats = rtvFormat;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };
		HRESULT hr = m_Device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PBRPipelineState));
		CS_ASSERT(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");

		///////////////////////////////////////////////////
		//////////////ROOT SIGNATURE /////////////////////
		///////////////////////////////////////////////////

		CD3DX12_DESCRIPTOR_RANGE1 cubemapCommonDescriptorHeapRanges[1] = {};
		cubemapCommonDescriptorHeapRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER1 cubemapRootParameter[2];
		cubemapRootParameter[0].InitAsConstantBufferView(0);
		cubemapRootParameter[1].InitAsDescriptorTable(_countof(cubemapCommonDescriptorHeapRanges), cubemapCommonDescriptorHeapRanges);

		CD3DX12_STATIC_SAMPLER_DESC cubemapStaticSamplerDescs[1] = {};
		cubemapStaticSamplerDescs[0].Init(0);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC cubemapRootsigDesc(_countof(cubemapRootParameter), cubemapRootParameter, _countof(cubemapStaticSamplerDescs), cubemapStaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		m_CubemapRootSignature = std::make_unique<RootSignature>(cubemapRootsigDesc);

		D3D12_INPUT_ELEMENT_DESC cubemapInputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		////////////////////////////////////////////////////
		////////PIPELINE STATE//////////////////////////////
		////////////////////////////////////////////////////

		cubemapPipelineStream.RootSignature = m_CubemapRootSignature->GetRaw();
		cubemapPipelineStream.InputLayout = { cubemapInputLayout, _countof(cubemapInputLayout) };
		cubemapPipelineStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		auto& cubemapShaderDatablobs = shaderManager.GetShader("CubemapShader")->GetRaw();
		cubemapPipelineStream.VS = { cubemapShaderDatablobs[ShaderType::Vertex]->GetBufferPointer(), cubemapShaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
		cubemapPipelineStream.PS = { cubemapShaderDatablobs[ShaderType::Pixel]->GetBufferPointer(), cubemapShaderDatablobs[ShaderType::Pixel]->GetBufferSize() };

		cubemapPipelineStream.DSV = CD3DX12_DEPTH_STENCIL_DESC(m_DepthStencil->GetDesc());
		cubemapPipelineStream.DSVFormat = m_DepthStencil->GetFormat();
		D3D12_RT_FORMAT_ARRAY cubemapRtvFormat = {};
		cubemapRtvFormat.NumRenderTargets = 1;
		cubemapRtvFormat.RTFormats[0] = m_RenderTargetViews[0]->GetFormat();
		cubemapPipelineStream.RTVFormats = cubemapRtvFormat;
		D3D12_PIPELINE_STATE_STREAM_DESC cubemapPipelineStreamDesc = { sizeof(cubemapPipelineStream), &cubemapPipelineStream };

		hr = m_Device->CreatePipelineState(&cubemapPipelineStreamDesc, IID_PPV_ARGS(&m_CubemapPipelineState));
		CS_ASSERT(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");
	}

	void Renderer::loadResources()
	{
		///////////////////////////////////////////////////
		/////LOAD SHADER, TEXTURE, CONSTANT BUFFER///////
		///////////////////////////////////////////////////

		auto& shaderManager = ShaderManager::Instance();
		auto& textureManager = TextureManager::Instance();
		auto& constantBufferPoolManager = ConstantBufferPoolManager::Instance();

		textureManager.Load(
			{ "assets/textures/Megaphone/Megaphone_01_16-bit_Diffuse.png",
			"assets/textures/Megaphone/Megaphone_01_16-bit_Roughness.png",
			"assets/textures/Megaphone/Megaphone_01_16-bit_Roughness.png",
			"assets/textures/Megaphone/Megaphone_01_16-bit_Metallic.png", },
			"MegaphoneMaterial");

		textureManager.Load({ "assets/textures/cubemaps/cubemap.dds" }, "CubemapMaterial");

		shaderManager.Load("assets/shaders/PBRShader.hlsl", "PBRShader");
		shaderManager.Load("assets/shaders/SkyboxShader.hlsl", "CubemapShader");

		m_PerFrameBuffer = constantBufferPoolManager.GetConstantBuffer(sizeof(m_PerFrameBuffer));
		m_PerObjectBuffer = constantBufferPoolManager.GetConstantBuffer(sizeof(m_PerObjectBuffer));
		m_CubemapCbuffer = constantBufferPoolManager.GetConstantBuffer(sizeof(m_CubemapCbuffer));
	}

	void Renderer::ChangeDisplayMode()
	{
		BOOL bIsFullScreen = false;
		m_SwapChain->GetFullscreenState(&bIsFullScreen, nullptr);
		if (m_bIsFullScreen == (bool)bIsFullScreen)
			return;
		HRESULT hr = m_SwapChain->SetFullscreenState(m_bIsFullScreen, nullptr);
		CS_ASSERT(SUCCEEDED(hr), "디스플레이모드를 변환하는데 실패하였습니다.");

		for (auto& rt : m_RenderTargetViews)
		{
			rt.reset();
		}
		m_RenderTargetViews.clear();

		hr = m_SwapChain->ResizeBuffers(2, m_ResWidth, m_ResHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_ASSERT(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");
		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_RenderTargetViews.push_back(std::make_unique<RenderTargetView>(rtvBuffer.Get()));
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
		CS_ASSERT(SUCCEEDED(hr), "타겟을 Resize하는데 실패하였습니다.");

		m_RtvIndex = 0;
	}
}