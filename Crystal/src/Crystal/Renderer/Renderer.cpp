#include "cspch.h"
#include "Renderer.h"

#include "RenderTarget.h"
#include "DepthStencil.h"

#include "DirectXTex/DirectXTex.h"
#include <iostream>

#include "examples/imgui_impl_dx12.h"
#include "examples/imgui_impl_win32.h"
ID3D12DescriptorHeap* g_pd3dSrvDescHeap = nullptr;

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

		auto [left, top, width, height] = m_Window->GetClientSize();
		// #DirectX Swap Chain Description
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
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
		hr = m_Factory->CreateSwapChainForHwnd(m_CommandQueue->GetRaw(), m_Window->GetWindowHandle(), &swapChainDesc,
			&swapChainFullscreenDesc, nullptr, m_SwapChain.GetAddressOf());
		CS_ASSERT(SUCCEEDED(hr), "Swap Chain을 생성하는데 실패하였습니다");


		/*RECT rt = {};
		GetClientRect(m_Window->GetWindowHandle(), &rt);

		DXGI_MODE_DESC targetParam = {};
		targetParam.Width = 1920;
		targetParam.Height = 1080;
		targetParam.RefreshRate.Numerator = 60;
		targetParam.RefreshRate.Denominator = 1;
		targetParam.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		targetParam.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		targetParam.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		m_SwapChain->ResizeTarget(&targetParam);

		m_SwapChain->ResizeBuffers(2, 1920, 1080, DXGI_FORMAT_R8G8B8A8_UNORM, swapChainDesc.Flags);*/

		

		Renderer::Get().GetFactory()->MakeWindowAssociation(m_Window->GetWindowHandle(), DXGI_MWA_NO_ALT_ENTER);


		for (int i = 0; i < 2; i++)
		{
			ID3D12Resource* rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
			m_RenderTargets.push_back(std::make_unique<RenderTarget>(rtvBuffer));
			SAFE_RELEASE(rtvBuffer);
		}


		m_DepthStencil = std::make_unique<DepthStencil>(width, height);
		//m_DepthStencil = std::make_unique<DepthStencil>(1920, 1080);


		hr = m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(256),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_ConstantBufferResource));
		CS_ASSERT(SUCCEEDED(hr), "");

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 1 + 4;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		hr = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_CommonDescriptorHeap));
		CS_ASSERT(SUCCEEDED(hr), "");

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_ConstantBufferResource->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = 256;
		m_Device->CreateConstantBufferView(&cbvDesc, m_CommonDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////// TEMP TEMP ///////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////


		std::string filePaths[] = {

			//"assets/textures/metal_plate_1k_png/metal_plate_diff_1k.png",
			//"assets/textures/metal_plate_1k_png/metal_plate_rough_1k.png",
			//"assets/textures/metal_plate_1k_png/metal_plate_spec_1k.png",

			"assets/textures/Megaphone/Megaphone_01_16-bit_Diffuse.png",
			"assets/textures/Megaphone/Megaphone_01_16-bit_Roughness.png",
			"assets/textures/Megaphone/Megaphone_01_16-bit_Metallic.png",
			"assets/textures/Megaphone/Megaphone_01_16-bit_Normal.png",

			/*"assets/textures/rustediron1-alt2-bl/rustediron2_basecolor.png",
			"assets/textures/rustediron1-alt2-bl/rustediron2_roughness.png",
			"assets/textures/rustediron1-alt2-bl/rustediron2_metallic.png",*/

			//"assets/textures/Ottoman/Ottoman_01_16-bit_Diffuse.png",
			//"assets/textures/Ottoman/Ottoman_01_16-bit_Roughness.png",
			//"assets/textures/Ottoman/Ottoman_01_16-bit_Metallic.png",
		};

		m_TextureBuffers.resize(_countof(filePaths));

		D3D12_CPU_DESCRIPTOR_HANDLE nextHandle = m_CommonDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	
		for (int i = 0; i < _countof(filePaths); i++)
		{
			nextHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			std::filesystem::path filePath(filePaths[i].c_str());
			CS_ASSERT(std::filesystem::exists(filePath), "%s 파일이 존재하지 않습니다.", filePath.string().c_str());


			DirectX::TexMetadata metaData;
			DirectX::ScratchImage scratchImage;
			hr = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metaData, scratchImage);
			CS_ASSERT(SUCCEEDED(hr), "%s 텍스쳐를 로드하는데 실패하였습니다.", filePath.string().c_str());

			metaData.format = DirectX::MakeSRGB(metaData.format);

			hr = m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Tex2D(metaData.format, (UINT64)metaData.width, (UINT)metaData.height, (UINT16)metaData.arraySize, (UINT16)metaData.mipLevels), D3D12_RESOURCE_STATE_COMMON,
				nullptr, IID_PPV_ARGS(&m_TextureBuffers[i]));
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
			m_Device->GetCopyableFootprints(&m_TextureBuffers[i]->GetDesc(), 0, (UINT)subResources.size(), 0, nullptr, nullptr, nullptr, &requiredSize);

			Microsoft::WRL::ComPtr<ID3D12Resource> textureUploadBuffer = nullptr;
			hr = m_Device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(requiredSize), D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr, IID_PPV_ARGS(&textureUploadBuffer));
			CS_ASSERT(SUCCEEDED(hr), "텍스쳐 업로드 버퍼를 생성하는데 실패하였습니다.");

			auto cmdList = m_CommandQueue->GetCommandList();
			UpdateSubresources(cmdList.Get(), m_TextureBuffers[i].Get(), textureUploadBuffer.Get(), 0, 0, (UINT)subResources.size(), subResources.data());

			D3D12_RESOURCE_BARRIER resourceBarrier = {};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = m_TextureBuffers[i].Get();
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

			cmdList->ResourceBarrier(1, &resourceBarrier);

			m_CommandQueue->Execute(cmdList);
			m_CommandQueue->Flush();


			D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
			shaderResourceViewDesc.Format = metaData.format;
			shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			m_Device->CreateShaderResourceView(m_TextureBuffers[i].Get(), &shaderResourceViewDesc, nextHandle);
		}		

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////


		

		m_ShaderLibrary->Load("assets/shaders/BlinnPhongShader", "BlinnPhongShader");
		m_ShaderLibrary->Load("assets/shaders/PBRShader", "PBRShader"); 

		///////////////////////////////////////////////////
		///////////////////// SHADER //////////////////////
		///////////////////////////////////////////////////

		CD3DX12_DESCRIPTOR_RANGE1 commonDescriptorHeapRanges[2] = {};
		commonDescriptorHeapRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		commonDescriptorHeapRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);


		CD3DX12_STATIC_SAMPLER_DESC StaticSamplerDescs[1] = {};
		StaticSamplerDescs[0].Init(0);

		CD3DX12_ROOT_PARAMETER1 rootParameter[1];
		rootParameter[0].InitAsDescriptorTable(_countof(commonDescriptorHeapRanges), commonDescriptorHeapRanges);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootsigDesc(_countof(rootParameter), rootParameter, _countof(StaticSamplerDescs), StaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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

		auto& shaderDatablobs = m_ShaderLibrary->GetShader("PBRShader")->GetRaw();
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
		//m_Camera->SetPosition(DirectX::XMFLOAT3(0, 0.0f, -100.0f));
		m_Camera->SetPosition(DirectX::XMFLOAT3(0, 100.0f, -500.0f));
		//m_Camera->SetPosition(DirectX::XMFLOAT3(0, 100.0f, -100.0f));

		//model = new Model("assets/models/Sphere.fbx");
		model = new Model("assets/models/Megaphone_01.fbx");
		//model = new Model("assets/models/Ottoman_01.fbx");


		//////////////////////////////////////////
		///// IMGUI IMPLE ////////////////////////
		//////////////////////////////////////////
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap));

		bool c = ImGui_ImplWin32_Init(m_Window->GetWindowHandle());
		bool d = ImGui_ImplDX12_Init(m_Device.Get(), 2,
			DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
			g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
	}

	void Renderer::Render()
	{
		//////////////////////////////////////////
		///// UPDATE /////////////////////////////
		//////////////////////////////////////////


		//TEMP///
		m_Camera->OnEvent(Event());
		////////////

		timer.Tick();
		static float angleA = 0.0f;
		angleA += 1.0f * timer.DeltaTime();

		XMStoreFloat4x4(&m_WorldMat, DirectX::XMMatrixIdentity());
		XMStoreFloat4x4(&m_WorldMat, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMat), DirectX::XMMatrixTranslation(2.0f, 3.0f, 4.0f)));

		XMStoreFloat4x4(&m_WorldMat, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMat),
			DirectX::XMMatrixRotationRollPitchYaw(0.0f, angleA, 0.0f)));
		XMStoreFloat4x4(&m_WorldMat, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMat)));

		m_ConstantBufferData.World = m_WorldMat;
		m_ConstantBufferData.ViewProj = m_Camera->GetViewProjection();
		m_ConstantBufferData.LightPositionInWorld = DirectX::XMFLOAT3(1000.0f, 1000.0F, 0.0F);
		m_ConstantBufferData.CameraPositionInWorld = m_Camera->GetWorldPosition();

		D3D12_RANGE readRange = { 0,0 };
		UINT8* bufferBegin = nullptr;
		m_ConstantBufferResource->Map(0, &readRange, (void**)&bufferBegin);
		memcpy(bufferBegin, (void*)&m_ConstantBufferData, sizeof(m_ConstantBufferData));
		m_ConstantBufferResource->Unmap(0, nullptr);


		//////////////////////////////////////////
		///// RENDER /////////////////////////////
		//////////////////////////////////////////



		//////////////////////////////////////////
		///// IMGUI IMPLE ////////////////////////
		//////////////////////////////////////////
		bool show_demo_window = true;
		bool show_another_window = true;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		// Start the Dear ImGui frame
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

	
		auto cmdList = m_CommandQueue->GetCommandList();

		float clearColor[] = { 135.0f / 255.0f, 206.0f / 255.0f  , 250.0f / 255.0f, 1.0f };
		//float clearColor[] = { 0.0f, 0.0f, 0.0f};

		cmdList->SetPipelineState(m_GraphicsPipeline.get()->GetRaw());
		cmdList->SetGraphicsRootSignature(m_RootSignature->GetRaw());

		

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->RSSetViewports(1, &m_Camera->GetViewport());
		cmdList->RSSetScissorRects(1, &m_Camera->GetScissorRect());
		cmdList->OMSetRenderTargets(1, &m_RenderTargets[m_RtvIndex]->GetCpuHandle(), TRUE, &m_DepthStencil->GetCpuHandle());

		m_RenderTargets[m_RtvIndex]->TransResourceState(cmdList.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET);

		cmdList->ClearRenderTargetView(m_RenderTargets[m_RtvIndex]->GetCpuHandle(), clearColor, 0, nullptr);
		cmdList->ClearDepthStencilView(m_DepthStencil->GetCpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_CommonDescriptorHeap.Get() };
		cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		cmdList->SetGraphicsRootDescriptorTable(0, m_CommonDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		model->Render(cmdList);


		cmdList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());

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