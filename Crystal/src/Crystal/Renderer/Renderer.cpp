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
#include "Pipelines/RenderPipelines/LightingSkeletalPipeline.h"
#include "Pipelines/ComputePipelines/BlurPipeline.h"
#include "Pipelines/ComputePipelines/AdditveBlendingPipeline.h"
#include "Pipelines/RenderPipelines/TonemappingPipeline.h"
#include "Crystal/Resources/DescriptorAllocation.h"

namespace Crystal {
	void Renderer::Init(WindowsWindow* window)
	{
		m_Window = window;

		CreateDevice();

		for (int i = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
		{
			m_DescriptorAllocators[i]
				= std::make_unique<DescriptorAllocator>(D3D12_DESCRIPTOR_HEAP_TYPE(i));
		}


		CreateRenderTargetViewFromSwapChain();
		CreateDepthStencilView();

		
		auto& resourceManager = Crystal::ResourceManager::Instance();

		m_FloatingPointBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RENDER_TARGET, "FloatingPointBuffer");
		m_FloatingPointBuffer.lock()->CreateShaderResourceView(m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);
		m_FloatingPointBuffer.lock()->CreateRenderTargetView(m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_FloatingPointBuffer.lock()->CreateUnorderedAccessView(m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_UAV_DIMENSION_TEXTURE2D);



		m_BrightColorBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, 
			D3D12_RESOURCE_STATE_RENDER_TARGET, "BrightColorBuffer");
		m_BrightColorBuffer.lock()->CreateRenderTargetView(m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_BrightColorBuffer.lock()->CreateUnorderedAccessView(m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_UAV_DIMENSION_TEXTURE2D);
		m_BrightColorBuffer.lock()->CreateShaderResourceView(m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);

		//=======================================================================

		

		//============================================================================================
		auto panoTexture = resourceManager.CreateTextureFromFile(
			"assets/textures/cubemaps/T_Cube_Skybox_1.hdr", "Pano_Skybox_Space").lock();
		panoTexture->CreateShaderResourceView(panoTexture->GetResource()->GetDesc().Format, D3D12_SRV_DIMENSION_TEXTURE2D);

		auto cubemap = resourceManager.CreateTexture(2048, 2048, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, "Cube_Skybox_Space").lock();
		cubemap->CreateUnorderedAccessView(cubemap->GetResource()->GetDesc().Format,
			D3D12_UAV_DIMENSION_TEXTURE2DARRAY);
		cubemap->CreateShaderResourceView(cubemap->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURECUBE);

		auto irradianceMap = resourceManager.CreateTexture(32, 32, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, "Cube_Skybox_Space_Irradiance").lock();
		irradianceMap->CreateUnorderedAccessView(irradianceMap->GetResource()->GetDesc().Format,
			D3D12_UAV_DIMENSION_TEXTURE2DARRAY);
		irradianceMap->CreateShaderResourceView(irradianceMap->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURECUBE);

		// TODO : TEMP
		m_PanoTexture = resourceManager.GetTexture("Pano_Skybox_Space").lock();
		m_CubemapTexture = resourceManager.GetTexture("Cube_Skybox_Space").lock();
		m_IrradiancemapTexture = resourceManager.GetTexture("Cube_Skybox_Space_Irradiance").lock();
		//============================================================================================

		auto pbrStaticShader = resourceManager.CreateShaderFromFile("assets/shaders/PBRShader_Static.hlsl", "PBRShader_Static").lock();
		auto pbrSkeletalShader = resourceManager.CreateShaderFromFile("assets/shaders/PBRShader_Skeletal.hlsl", "PBRShader_Skeletal").lock();
		auto skyboxShader = resourceManager.CreateShaderFromFile("assets/shaders/SkyboxShader.hlsl", "Skybox").lock();
		auto panoToCubemapShader = resourceManager.CreateShaderFromFile("assets/shaders/EquirectangularToCube.hlsl", "PanoToCubemap").lock();
		auto diffIrradianceShader = resourceManager.CreateShaderFromFile("assets/shaders/DiffuseIrradianceSampling.hlsl", "DiffuseIrradianceSampling").lock();
		auto specularIrradianceShader = resourceManager.CreateShaderFromFile("assets/shaders/SpecularIrradianceSampling.hlsl", "SpecularIrradianceSampling").lock();
		auto simpleColorShader = resourceManager.CreateShaderFromFile("assets/shaders/SimpleColorShader.hlsl", "SimpleColorShader").lock();
		auto gaussianBlurShader = resourceManager.CreateShaderFromFile("assets/shaders/GaussianBlur.hlsl", "GaussianBlur").lock();
		auto additiveBlendingHdrShader = resourceManager.CreateShaderFromFile("assets/shaders/AdditiveBlending.hlsl", "AdditiveBelndingHDR").lock();
		auto tonemappingShader = resourceManager.CreateShaderFromFile("assets/shaders/Tonemapping.hlsl", "Tonemapping").lock();

		{
			pbrStaticShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				});


			RootParameter perFrame = { 1, 1, 0 };
			RootParameter perObject = { 1, 0, 0 };
			RootParameter perExecute = { 1, 5, 0 };

			pbrStaticShader->SetRootSignature({ perFrame, perObject, perExecute, { CD3DX12_STATIC_SAMPLER_DESC(0) } });
			pbrStaticShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		}

		{
			pbrSkeletalShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				});


			RootParameter perFrame = { 1, 1, 0 };
			RootParameter perObject = { 1, 0, 0 };
			RootParameter perExecute = { 1, 4, 0 };

			pbrSkeletalShader->SetRootSignature({ perFrame, perObject, perExecute, { CD3DX12_STATIC_SAMPLER_DESC(0) } });
			pbrSkeletalShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		}




		{
			simpleColorShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				});

			RootParameter perFrame = { 1, 0, 0 };
			RootParameter perObject = { 1, 0, 0 };
			RootParameter perExecute = {};

			simpleColorShader->SetRootSignature({ perFrame, perObject, perExecute });
			simpleColorShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
		}

		{
			skyboxShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				});

			RootParameter perFrame = { 1, 1, 0 };
			RootParameter perObject = {};
			RootParameter perExecute = {};

			skyboxShader->SetRootSignature({ perFrame, perObject, perExecute, { CD3DX12_STATIC_SAMPLER_DESC(0) } });
			skyboxShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		}

		{

			RootParameter perFrame = { 0, 1, 1 };
			RootParameter perObject = {};
			RootParameter perExecute = {};

			panoToCubemapShader->SetRootSignature({ perFrame, perObject, perExecute, { CD3DX12_STATIC_SAMPLER_DESC(0) } });
			panoToCubemapShader->SetDispatchThreadGroupCounts({ 2048 / 32, 2048 / 32, 6 });
		}

		{

			RootParameter perFrame = { 0, 1, 1 };
			RootParameter perObject = {};
			RootParameter perExecute = {};

			diffIrradianceShader->SetRootSignature({ perFrame, perObject, perExecute, { CD3DX12_STATIC_SAMPLER_DESC(0) } });
			diffIrradianceShader->SetDispatchThreadGroupCounts({ 32 / 32, 32 / 32, 6 });
		}

		{
			RootParameter perFrame = { 0, 0, 1 };
			RootParameter perObject = {};
			RootParameter perExecute = {};
			

			gaussianBlurShader->SetRootSignature({ perFrame, perObject, perExecute});
			gaussianBlurShader->SetDispatchThreadGroupCounts({ 1920 / 8, 1080 / 8, 15 });
		}

		{
			RootParameter perFrame = { 0, 1, 1 };
			RootParameter perObject = {};
			RootParameter perExecute = {};


			additiveBlendingHdrShader->SetRootSignature({ perFrame, perObject, perExecute });
			additiveBlendingHdrShader->SetDispatchThreadGroupCounts({ 1920 / 8, 1080 / 8, 1 });
		}

		{
			tonemappingShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				});

			RootParameter perFrame = { 0, 1, 0 };
			RootParameter perObject = {};
			RootParameter perExecute = {};


			tonemappingShader->SetRootSignature({ perFrame, perObject, perExecute, { CD3DX12_STATIC_SAMPLER_DESC(0)} });
			tonemappingShader->SetDispatchThreadGroupCounts({ 1920 / 8, 1080 / 8, 1 });
			tonemappingShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		}



		m_LightPipelines.push_back(CreatePipline<LightingStaticPipeline>(pbrStaticShader, "PBRStaticPipeline"));
		m_LightPipelines.push_back(CreatePipline<LightingSkeletalPipeline>(pbrSkeletalShader, "PBRSkeletalPipeline"));


		m_Pipelines.push_back(CreatePipline<LinePipeline>(simpleColorShader, "SimpleColorLinePipeline"));
		m_Pipelines.push_back(CreatePipline<CubemapPipeline>(skyboxShader, "CubemapPipeline"));
		m_Pipelines.push_back(CreatePipline<PanoToCubemapPipeline>(panoToCubemapShader, "PanoToCubemapPipeline"));
		m_Pipelines.push_back(CreatePipline<DiffIrradSamplingPipeline>(diffIrradianceShader, "DiffIradiancePipeline"));
		m_Pipelines.push_back(CreatePipline<BlurPipeline>(gaussianBlurShader, "GaussianBlurPipeline"));
		m_Pipelines.push_back(CreatePipline<AdditiveBlendingPipeline>(additiveBlendingHdrShader, "AdditiveBlendingPipeline"));
		m_Pipelines.push_back(CreatePipline<TonemappingPipeline>(tonemappingShader, "TonemappingPipeline"));
	}

	void Renderer::PrepareRender()
	{
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
		m_Pipelines[2]->PrepareRecord(&panoToCubemapPipelineInputs);
		m_Pipelines[2]->Record(commandList);

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

		m_Pipelines[3]->PrepareRecord(&diffIrradSamplingPipelineInputs);
		m_Pipelines[3]->Record(commandList);

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
		resourceBarrier.Transition.pResource = m_ColorBufferTextures[m_RtvIndex].lock()->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		const auto mainCamera = ApplicationUtility::GetPlayerController().GetMainCamera().lock();

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->RSSetViewports(1, &mainCamera->GetViewport());
		commandList->RSSetScissorRects(1, &mainCamera->GetScissorRect());

		D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[2] = { m_FloatingPointBuffer.lock()->GetRenderTargetView() , 
		m_BrightColorBuffer.lock()->GetRenderTargetView() };


		commandList->OMSetRenderTargets(_countof(renderTargets), renderTargets, false,
			&m_DepthStencilBufferTexture.lock()->GetDepthStencilView());

		float clearColorValue[4] = { 1.0f, 0.0f, 1.0f, 0.0f };
		float clearColorBlack[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		const auto clearDepthValue = 1.0f;
		const auto clearStencilValue = 0.0f;

		commandList->ClearRenderTargetView(m_ColorBufferTextures[m_RtvIndex].lock()->GetRenderTargetView(),
			clearColorValue, 0, nullptr);
		commandList->ClearRenderTargetView(m_FloatingPointBuffer.lock()->GetRenderTargetView(),
			clearColorValue, 0, nullptr);
		commandList->ClearRenderTargetView(m_BrightColorBuffer.lock()->GetRenderTargetView(),
			clearColorBlack, 0, nullptr);


		commandList->ClearDepthStencilView(m_DepthStencilBufferTexture.lock()->GetDepthStencilView(),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearDepthValue, clearStencilValue, 0, nullptr);

		LightingStaticPipeline::LightingPipelineInputs lightingPipelineInputs = {};
		lightingPipelineInputs.Camera = mainCamera.get();
		lightingPipelineInputs.IrradiancemapTexture = m_IrradiancemapTexture.get();

		m_LightPipelines[0]->PrepareRecord(&lightingPipelineInputs);
		m_LightPipelines[0]->Record(commandList);

		m_LightPipelines[1]->PrepareRecord(&lightingPipelineInputs);
		m_LightPipelines[1]->Record(commandList);
		

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		LinePipeline::LinePipelineInputs wireframePipelineInputs = {};
		wireframePipelineInputs.Camera = mainCamera.get();

		m_Pipelines[0]->PrepareRecord(&wireframePipelineInputs);
		m_Pipelines[0]->Record(commandList);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		

		CubemapPipeline::CubemapPipelineInputs cubemapPipelineInputs = {};
		cubemapPipelineInputs.Camera = mainCamera.get();

		m_Pipelines[1]->PrepareRecord(&cubemapPipelineInputs);
		m_Pipelines[1]->Record(commandList);



		resourceBarrier.Transition.pResource = m_BrightColorBuffer.lock()->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		m_Pipelines[4]->PrepareRecord(nullptr);
		m_Pipelines[4]->Record(commandList);

		resourceBarrier.Transition.pResource = m_BrightColorBuffer.lock()->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		resourceBarrier.Transition.pResource = m_FloatingPointBuffer.lock()->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		
		AdditiveBlendingPipeline::AdditiveBlendingPipelineInputs input = {};
		m_Pipelines[5]->PrepareRecord(&input);
		m_Pipelines[5]->Record(commandList);

		resourceBarrier.Transition.pResource = m_FloatingPointBuffer.lock()->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);


		resourceBarrier.Transition.pResource = m_BrightColorBuffer.lock()->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);



		commandList->OMSetRenderTargets(1, &m_ColorBufferTextures[m_RtvIndex].lock()->GetRenderTargetView(), false,
			&m_DepthStencilBufferTexture.lock()->GetDepthStencilView());

		m_Pipelines[6]->PrepareRecord(&input);
		m_Pipelines[6]->Record(commandList);


		resourceBarrier.Transition.pResource = m_FloatingPointBuffer.lock()->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);



		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_ColorBufferTextures[m_RtvIndex].lock()->GetResource();
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

		ReleaseStaleDescriptors();

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


		auto& resourceManager = Crystal::ResourceManager::Instance();


		resourceManager.DestroyTexture("ColorBuffer_0");
		resourceManager.DestroyTexture("ColorBuffer_1");
		resourceManager.DestroyTexture("DepthStencilBuffer");
		resourceManager.DestroyTexture("FloatingPointBuffer");
		resourceManager.DestroyTexture("BrightColorBuffer");

		hr = m_SwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_FATAL(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");

		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));

			m_ColorBufferTextures[i] = resourceManager.CreateTextureByResource(rtvBuffer.Get(), "ColorBuffer_" + std::to_string(i),
				D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_ColorBufferTextures[i].lock()->CreateRenderTargetView(m_ColorBufferTextures[i].lock()->GetResource()->GetDesc().Format,
				D3D12_RTV_DIMENSION_TEXTURE2D);
			m_ColorBufferTextures[i].lock()->CreateShaderResourceView(m_ColorBufferTextures[i].lock()->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		m_FloatingPointBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RENDER_TARGET, "FloatingPointBuffer");
		m_FloatingPointBuffer.lock()->CreateShaderResourceView(m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);
		m_FloatingPointBuffer.lock()->CreateRenderTargetView(m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_FloatingPointBuffer.lock()->CreateUnorderedAccessView(m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_UAV_DIMENSION_TEXTURE2D);
		

		m_BrightColorBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_RENDER_TARGET, "BrightColorBuffer");
		m_BrightColorBuffer.lock()->CreateRenderTargetView(m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_BrightColorBuffer.lock()->CreateUnorderedAccessView(m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_UAV_DIMENSION_TEXTURE2D);
		m_BrightColorBuffer.lock()->CreateShaderResourceView(m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);


		


		m_DepthStencilBufferTexture = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 0,
			DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, "DepthStencilBuffer").lock();
		m_DepthStencilBufferTexture.lock()->CreateDepthStencilView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_DSV_DIMENSION_TEXTURE2D);


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

	void Renderer::RegisterLightComponent(std::weak_ptr<LightComponent> componentWeak)
	{
		for (const auto& pipeline : m_LightPipelines)
		{
			pipeline->RegisterLightComponents(componentWeak);
		}
	}

	void Renderer::RegisterRendererComponent(std::weak_ptr<PrimitiveComponent> componentWeak)
	{
		auto component = componentWeak.lock();

		const auto& materials = component->GetMaterials();
		if (materials.empty())
			return;

		std::vector<int> registeredPipelineIndices; // Component가 Register되어 있는 pipeline의 컨테이너 상의 Index
		std::vector<int> registeredLightPipelineIndices;

		for (const auto& mat : materials)
		{
			for (int i = 0; i < m_Pipelines.size(); i++)
			{
				bool bAlreadyRegisteredInThisPipeline = std::find(registeredPipelineIndices.begin(), 
					registeredPipelineIndices.end(), i) != registeredPipelineIndices.end();

				// 이미 Register되어 있거나 material이 이 Pipeline에서 사용되지 않으면
				if(bAlreadyRegisteredInThisPipeline || !m_Pipelines[i]->IsValidForThisPipeline(mat)) 
					continue;


				m_Pipelines[i]->RegisterPipelineComponents(componentWeak); // 이 파이프라인에 컴포넌트를 등록
				registeredPipelineIndices.push_back(i);
			}

			for (int i = 0; i < m_LightPipelines.size(); i++)
			{
				bool bAlreadyRegisteredInThisPipeline = std::find(registeredLightPipelineIndices.begin(),
					registeredLightPipelineIndices.end(), i) != registeredLightPipelineIndices.end();

				// 이미 Register되어 있거나 material이 이 Pipeline에서 사용되지 않으면
				if (bAlreadyRegisteredInThisPipeline || !m_LightPipelines[i]->IsValidForThisPipeline(mat))
					continue;


				m_LightPipelines[i]->RegisterPipelineComponents(componentWeak); // 이 파이프라인에 컴포넌트를 등록
				registeredLightPipelineIndices.push_back(i);
			}
		}
	}

	DescriptorAllocation Renderer::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorCount)
	{
		return m_DescriptorAllocators[type]->Allocate(descriptorCount);
	}

	void Renderer::ReleaseStaleDescriptors()
	{
		for (auto & DescriptorAllocator : m_DescriptorAllocators)
		{
			DescriptorAllocator->ReleaseStaleDescriptors();
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

		auto& resourceManager = Crystal::ResourceManager::Instance();


		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));
		
			m_ColorBufferTextures[i] = resourceManager.CreateTextureByResource(rtvBuffer.Get(), "ColorBuffer_" + std::to_string(i), 
				D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_ColorBufferTextures[i].lock()->CreateRenderTargetView(m_ColorBufferTextures[i].lock()->GetResource()->GetDesc().Format,
				D3D12_RTV_DIMENSION_TEXTURE2D);
			m_ColorBufferTextures[i].lock()->CreateShaderResourceView(m_ColorBufferTextures[i].lock()->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}

	}

	void Renderer::CreateDepthStencilView()
	{
		auto& resourceManager = ResourceManager::Instance();

		m_DepthStencilBufferTexture = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 0,
			DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, "DepthStencilBuffer");
		m_DepthStencilBufferTexture.lock()->CreateDepthStencilView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_DSV_DIMENSION_TEXTURE2D);
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


		auto& resourceManager = ResourceManager::Instance();
		resourceManager.DestroyTexture("ColorBuffer_0");
		resourceManager.DestroyTexture("ColorBuffer_1");
		resourceManager.DestroyTexture("FloatingPointBuffer");
		resourceManager.DestroyTexture("BrightColorBuffer");

		hr = m_SwapChain->ResizeBuffers(2, m_ResWidth, m_ResHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_FATAL(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");


		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));

			m_ColorBufferTextures[i] = resourceManager.CreateTextureByResource(rtvBuffer.Get(), "ColorBuffer_" + std::to_string(i),
				D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_ColorBufferTextures[i].lock()->CreateRenderTargetView(m_ColorBufferTextures[i].lock()->GetResource()->GetDesc().Format,
				D3D12_RTV_DIMENSION_TEXTURE2D);
			m_ColorBufferTextures[i].lock()->CreateShaderResourceView(m_ColorBufferTextures[i].lock()->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}

		m_FloatingPointBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_RENDER_TARGET, "FloatingPointBuffer");
		m_FloatingPointBuffer.lock()->CreateShaderResourceView(m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);
		m_FloatingPointBuffer.lock()->CreateRenderTargetView(m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_FloatingPointBuffer.lock()->CreateUnorderedAccessView(m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_UAV_DIMENSION_TEXTURE2D);


		m_BrightColorBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_RENDER_TARGET, "BrightColorBuffer");
		m_BrightColorBuffer.lock()->CreateRenderTargetView(m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_BrightColorBuffer.lock()->CreateUnorderedAccessView(m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_UAV_DIMENSION_TEXTURE2D);
		m_BrightColorBuffer.lock()->CreateShaderResourceView(m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);




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