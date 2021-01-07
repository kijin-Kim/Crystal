#include "cspch.h"
#include "Renderer.h"

#include "DirectXTex/DirectXTex.h"
#include <iostream>


#include "Crystal/GamePlay/World/World.h"

#include "Crystal/AssetManager/ShaderManager.h"
#include "Crystal/AssetManager/ConstantBuffer.h"
#include "Crystal/GamePlay/Actors/Pawn.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "../Core/ApplicationUtility.h"
#include "../GamePlay/Actors/CameraPawn.h"
#include "Material.h"

namespace Crystal {
	void Renderer::Init(WindowsWindow* window)
	{
		m_Window = window;

		createDevice();
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
		CS_ASSERT(SUCCEEDED(hr), "CBV_SRV���� �����ϴµ� �����Ͽ����ϴ�.");

		

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
	

		

		{
			for (const auto& meshComponent : m_MeshComponents)
			{
				auto material = meshComponent->GetMesh()->GetMaterial();

				auto albedoTexture = material->GetTextureInput("AlbedoTexture");
				auto roughnessTexture = material->GetTextureInput("RoughnessTexture");
				auto metalicTexture = material->GetTextureInput("MetalicTexture");
				auto normalTexture = material->GetTextureInput("NormalTexture");

				/*�ӽ� : ��� �ؽ��ĵ��� Descriptor Heap�� �������� ���� �ȿ� �ִٰ� ������.*/
				D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_CommonDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				destHeapHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				m_Device->CopyDescriptorsSimple(4, destHeapHandle, albedoTexture->GetShaderResourceView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			}

			/*CBV�� �����մϴ�.*/
			m_Device->CopyDescriptorsSimple(1, m_CommonDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_PerObjectBuffer.GetCpuDescriptorHandle(),
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


			/*SRV�� �����մϴ�.*/
			D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_CommonDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			destHeapHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			//m_Device->CopyDescriptorsSimple(4, destHeapHandle, m_AlbedoTexture->GetShaderResourceView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 4;

			m_Device->CopyDescriptorsSimple(1, destHeapHandle, m_CubemapTexture->GetShaderResourceView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}


		//////////////////////////////////////////
		///// RENDER /////////////////////////////
		//////////////////////////////////////////

		auto cmdList = m_CommandQueue->GetCommandList();

		cmdList->SetPipelineState(m_PBRPipelineState.Get());
		cmdList->SetGraphicsRootSignature(m_NormalRootSignature.Get());

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->RSSetViewports(1, &cameraComponent->GetViewport());
		cmdList->RSSetScissorRects(1, &cameraComponent->GetScissorRect());
		//cmdList->OMSetRenderTargets(1, &m_RenderTargetViews[m_RtvIndex]->GetCpuHandle(), TRUE, &m_DepthStencil->GetCpuHandle());
		cmdList->OMSetRenderTargets(1, &m_ColorBufferTextures[m_RtvIndex]->GetRenderTargetView(), TRUE, &m_DepthBufferTexture->GetDepthStencilView());


		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_ColorBufferTextures[m_RtvIndex]->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cmdList->ResourceBarrier(1, &resourceBarrier);

		

		cmdList->ClearRenderTargetView(m_ColorBufferTextures[m_RtvIndex]->GetRenderTargetView(), m_ClearColor, 0, nullptr);
		//cmdList->ClearDepthStencilView(m_DepthStencil->GetCpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		cmdList->ClearDepthStencilView(m_DepthBufferTexture->GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

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
		cmdList->SetGraphicsRootSignature(m_CubemapRootSignature.Get());
		cmdList->SetGraphicsRootConstantBufferView(0, m_CubemapCbuffer.GetGpuVirtualAddress());
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_CommonDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;
		cmdList->SetGraphicsRootDescriptorTable(1, handle);
		cmdList->IASetVertexBuffers(0, 1, &m_QuadVertexBuffer->GetView());
		cmdList->IASetIndexBuffer(&m_QuadIndexBuffer->GetView());
		cmdList->DrawIndexedInstanced(m_QuadIndexBuffer->GetCount(), 1, 0, 0, 0);

		resourceBarrier.Transition.pResource = m_ColorBufferTextures[m_RtvIndex]->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cmdList->ResourceBarrier(1, &resourceBarrier);
	
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
		CS_ASSERT(SUCCEEDED(hr), "Ÿ���� Resize�ϴµ� �����Ͽ����ϴ�.");

		for (auto& rt : m_ColorBufferTextures)
		{
			rt.reset();
		}

		hr = m_SwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_ASSERT(SUCCEEDED(hr), "���۸� Resize�ϴµ� �����Ͽ����ϴ�.");

		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_ColorBufferTextures[i] = std::make_unique<Texture>(rtvBuffer.Get(), D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_ColorBufferTextures[i]->CreateRenderTargetView(m_ColorBufferTextures[i]->GetResource()->GetDesc().Format, D3D12_RTV_DIMENSION_TEXTURE2D);
		}
		m_DepthBufferTexture = std::make_unique<Texture>(m_ResWidth, m_ResHeight, 1, 0, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_DepthBufferTexture->CreateDepthStencilView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_DSV_DIMENSION_TEXTURE2D);

		auto cameraComponent = ApplicationUtility::GetPlayerController().GetMainCamera();
		cameraComponent->SetViewport({ 0,0, (FLOAT)width, (FLOAT)height, 0.0f, 1.0f });
		cameraComponent->SetScissorRect({ 0, 0, width, height });

		m_Window->SetWidth(targetParam.Width);
		m_Window->SetHeight(targetParam.Height);

	}

	void Renderer::createDevice()
	{
		HRESULT hr = E_FAIL;
		UINT createFactoryDebugFlags = 0;

#if defined CS_DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
		CS_ASSERT(SUCCEEDED(hr), "D3D����� �������̽��� �������µ� �����Ͽ����ϴ�.");
		debugController->EnableDebugLayer();
		createFactoryDebugFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		// #DirectX Create DXGI factory
		hr = CreateDXGIFactory2(createFactoryDebugFlags, IID_PPV_ARGS(m_Factory.GetAddressOf()));
		CS_ASSERT(SUCCEEDED(hr), "DXGI Factory�� �����ϴµ� �����Ͽ����ϴ�.");

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
				CS_ASSERT(SUCCEEDED(hr), "D3D Device�� �����ϴµ� �����Ͽ����ϴ�."); 
				
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
		CS_ASSERT(SUCCEEDED(hr), "Swap Chain�� �����ϴµ� �����Ͽ����ϴ�");
		Renderer::Instance().GetFactory()->MakeWindowAssociation(m_Window->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);

		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_ColorBufferTextures[i] = std::make_unique<Texture>(rtvBuffer.Get(), D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_ColorBufferTextures[i]->CreateRenderTargetView(m_ColorBufferTextures[i]->GetResource()->GetDesc().Format, D3D12_RTV_DIMENSION_TEXTURE2D);
		}
	}

	void Renderer::createDepthStencilView()
	{
		m_DepthBufferTexture = std::make_unique<Texture>(m_ResWidth, m_ResHeight, 1, 0, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, 
			D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_DepthBufferTexture->CreateDepthStencilView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_DSV_DIMENSION_TEXTURE2D);
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

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootsigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_ASSERT(SUCCEEDED(hr), "Root Signature�� �ø���ȭ�ϴµ� �����Ͽ����ϴ�");
		hr = m_Device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_NormalRootSignature));
		CS_ASSERT(SUCCEEDED(hr), "Root Signature�� �����ϴµ� �����Ͽ����ϴ�");


		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
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
		pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
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
		pipelineStateStream.DSVFormat = m_DepthBufferTexture->GetResource()->GetDesc().Format;
		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineStateStream.RTVFormats = rtvFormat;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };
		hr = m_Device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PBRPipelineState));
		CS_ASSERT(SUCCEEDED(hr), "Graphics Pipeline State Object�� �����ϴµ� �����Ͽ����ϴ�");

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
		CS_ASSERT(SUCCEEDED(hr), "Root Signature�� �ø���ȭ�ϴµ� �����Ͽ����ϴ�");
		hr = m_Device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_CubemapRootSignature));
		CS_ASSERT(SUCCEEDED(hr), "Root Signature�� �����ϴµ� �����Ͽ����ϴ�");

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
		cubemapPipelineStream.DSVFormat = m_DepthBufferTexture->GetResource()->GetDesc().Format;
		D3D12_RT_FORMAT_ARRAY cubemapRtvFormat = {};
		cubemapRtvFormat.NumRenderTargets = 1;
		cubemapRtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		cubemapPipelineStream.RTVFormats = cubemapRtvFormat;
		D3D12_PIPELINE_STATE_STREAM_DESC cubemapPipelineStreamDesc = { sizeof(cubemapPipelineStream), &cubemapPipelineStream };

		hr = m_Device->CreatePipelineState(&cubemapPipelineStreamDesc, IID_PPV_ARGS(&m_CubemapPipelineState));
		CS_ASSERT(SUCCEEDED(hr), "Graphics Pipeline State Object�� �����ϴµ� �����Ͽ����ϴ�");
	}

	void Renderer::loadResources()
	{
		///////////////////////////////////////////////////
		/////LOAD SHADER, TEXTURE, CONSTANT BUFFER///////
		///////////////////////////////////////////////////

		auto& shaderManager = ShaderManager::Instance();
		/*auto& textureManager = TextureManager::Instance();*/
		auto& constantBufferPoolManager = ConstantBufferPoolManager::Instance();


		/*�ؽ��� ���ҽ��� �ε��մϴ�.*/
		m_CubemapTexture = std::make_unique<Texture>("assets/textures/cubemaps/cubemap.dds");

		/*�� �ؽ��� ���ҽ��� ���� Shader Resource View�� ����ϴ�.*/
		m_CubemapTexture->CreateShaderResourceView(m_CubemapTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURECUBE);
		

		shaderManager.Load("assets/shaders/PBRShader.hlsl", "PBRShader");
		shaderManager.Load("assets/shaders/SkyboxShader.hlsl", "CubemapShader");
		
		m_PerFrameBuffer = constantBufferPoolManager.GetConstantBuffer(sizeof(m_PerFrameBuffer));
		m_PerObjectBuffer = constantBufferPoolManager.GetConstantBuffer(sizeof(m_PerObjectBuffer));
		m_CubemapCbuffer = constantBufferPoolManager.GetConstantBuffer(sizeof(m_CubemapCbuffer));

		Material mat(shaderManager.GetShader("PBRShader"));
		mat.Set("AlbedoTexture", std::make_shared<Texture>("assets/textures/Megaphone/Megaphone_01_16-bit_Diffuse.png"));
	}

	void Renderer::ChangeDisplayMode()
	{
		BOOL bIsFullScreen = false;
		m_SwapChain->GetFullscreenState(&bIsFullScreen, nullptr);
		if (m_bIsFullScreen == (bool)bIsFullScreen)
			return;
		HRESULT hr = m_SwapChain->SetFullscreenState(m_bIsFullScreen, nullptr);
		CS_ASSERT(SUCCEEDED(hr), "���÷��̸�带 ��ȯ�ϴµ� �����Ͽ����ϴ�.");

		for (auto& rt : m_ColorBufferTextures)
		{
			rt.reset();
		}

		hr = m_SwapChain->ResizeBuffers(2, m_ResWidth, m_ResHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_ASSERT(SUCCEEDED(hr), "���۸� Resize�ϴµ� �����Ͽ����ϴ�.");
		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_ColorBufferTextures[i] = std::make_unique<Texture>(rtvBuffer.Get(), D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_ColorBufferTextures[i]->CreateRenderTargetView(m_ColorBufferTextures[i]->GetResource()->GetDesc().Format, D3D12_RTV_DIMENSION_TEXTURE2D);
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
		CS_ASSERT(SUCCEEDED(hr), "Ÿ���� Resize�ϴµ� �����Ͽ����ϴ�.");

		m_RtvIndex = 0;
	}
}