#include "cspch.h"
#include "RenderSystem.h"

#include "CommandQueue.h"
#include "Crystal/Core/Device.h"
#include "Crystal/Core/WindowsWindow.h"
#include "Crystal/Gameplay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/BasicShapeMeshes.h"
#include "Crystal/Resources/DescriptorAllocator.h"
#include "Crystal/Resources/Texture.h"
#include "Pipelines/ComputePipelines/AdditveBlendingPipeline.h"
#include "Pipelines/ComputePipelines/BlurPipeline.h"
#include "Pipelines/ComputePipelines/DiffIrradSamplingPipeline.h"
#include "Pipelines/ComputePipelines/PanoToCubemapPipeline.h"
#include "Pipelines/RenderPipelines/CubemapPipeline.h"
#include "Pipelines/RenderPipelines/LightingSkeletalPipeline.h"
#include "Pipelines/RenderPipelines/GeometryStaticPipeline.h"
#include "Pipelines/RenderPipelines/LightingPipeline.h"
#include "Pipelines/RenderPipelines/LinePipeline.h"
#include "Pipelines/RenderPipelines/TonemappingPipeline.h"
#include "Pipelines/RenderPipelines/UnlitPipeline.h"

namespace Crystal {


	void RenderSystem::Begin()
	{
		CreateRenderTargets();
		CreateDepthStencilView();

		auto& resourceManager = ResourceManager::Instance();

		auto pbrStaticShader = resourceManager.GetShader("assets/shaders/GeometryPass_Static.hlsl");
		auto pbrSkeletalShader = resourceManager.GetShader("assets/shaders/PBRShader_Skeletal.hlsl");
		auto skyboxShader = resourceManager.GetShader("assets/shaders/SkyboxShader.hlsl");
		auto panoToCubemapShader = resourceManager.GetShader("assets/shaders/EquirectangularToCube.hlsl");
		auto diffIrradianceShader = resourceManager.GetShader("assets/shaders/DiffuseIrradianceSampling.hlsl");
		auto specularIrradianceShader = resourceManager.GetShader("assets/shaders/SpecularIrradianceSampling.hlsl");
		auto simpleColorShader = resourceManager.GetShader("assets/shaders/SimpleColorShader.hlsl");
		auto gaussianBlurShader = resourceManager.GetShader("assets/shaders/GaussianBlur.hlsl");
		auto additiveBlendingHdrShader = resourceManager.GetShader("assets/shaders/AdditiveBlending.hlsl");
		auto toneMappingShader = resourceManager.GetShader("assets/shaders/Tonemapping.hlsl");
		auto lightingPassShader = resourceManager.GetShader("assets/shaders/LightingPass.hlsl");
		auto unlitShader = resourceManager.GetShader("assets/shaders/UnlitShader.hlsl");


		{
			/*		pbrStaticShader->SetInputLayout({
						{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
						{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
						{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
						{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
						{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
						});*/

			pbrStaticShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

				{"MATROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"MATROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"MATROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"MATROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},

				{"MATERIAL_INDEX", 0, DXGI_FORMAT_R32_UINT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{
					"ALBEDO_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 68, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
					1
				},
				{
					"EMISSIVE_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 80,
					D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
				},
				{
					"ROUGHNESS_CONSTANT", 0, DXGI_FORMAT_R32_FLOAT, 1, 92, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
					1
				},
				{"METALLIC_CONSTANT", 0, DXGI_FORMAT_R32_FLOAT, 1, 96, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{
					"TOGGLE_ALBEDO_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 100,
					D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
				},
				{
					"TOGGLE_METALLIC_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 104,
					D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
				},
				{
					"TOGGLE_ROUGHNESS_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 108,
					D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
				},
				{
					"TOGGLE_NORMAL_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 112,
					D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
				},
				{
					"TOGGLE_IRRADIANCE_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 116,
					D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
				},
				{
					"TOGGLE_EMISSIVE_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 120,
					D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
				},
			});


			RootParameter perFrame = {1, 1, 0};
			RootParameter perObject = {0, 0, 0};
			RootParameter perExecute = {0, 5, 0};

			pbrStaticShader->SetRootSignature({perFrame, perObject, perExecute, {CD3DX12_STATIC_SAMPLER_DESC(0)}});
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


			RootParameter perFrame = {1, 1, 0};
			RootParameter perObject = {1, 0, 0};
			RootParameter perExecute = {1, 4, 0};

			pbrSkeletalShader->SetRootSignature({perFrame, perObject, perExecute, {CD3DX12_STATIC_SAMPLER_DESC(0)}});
			pbrSkeletalShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		}


		{
			/*simpleColorShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				});*/
			simpleColorShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

				{"MATROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"MATROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"MATROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"MATROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
			});

			RootParameter perFrame = {1, 0, 0};
			RootParameter perObject = {1, 0, 0};
			RootParameter perExecute = {};

			simpleColorShader->SetRootSignature({perFrame, perObject, perExecute});
			simpleColorShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
		}

		{
			skyboxShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			});

			RootParameter perFrame = {1, 1, 0};
			RootParameter perObject = {};
			RootParameter perExecute = {};

			skyboxShader->SetRootSignature({perFrame, perObject, perExecute, {CD3DX12_STATIC_SAMPLER_DESC(0)}});
			skyboxShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		}

		{
			RootParameter perFrame = {0, 1, 1};
			RootParameter perObject = {};
			RootParameter perExecute = {};

			panoToCubemapShader->SetRootSignature({perFrame, perObject, perExecute, {CD3DX12_STATIC_SAMPLER_DESC(0)}});
			panoToCubemapShader->SetDispatchThreadGroupCounts({2048 / 32, 2048 / 32, 6});
		}

		{
			RootParameter perFrame = {0, 1, 1};
			RootParameter perObject = {};
			RootParameter perExecute = {};

			diffIrradianceShader->SetRootSignature({perFrame, perObject, perExecute, {CD3DX12_STATIC_SAMPLER_DESC(0)}});
			diffIrradianceShader->SetDispatchThreadGroupCounts({32 / 32, 32 / 32, 6});
		}

		{
			RootParameter perFrame = {0, 0, 1};
			RootParameter perObject = {};
			RootParameter perExecute = {};


			gaussianBlurShader->SetRootSignature({perFrame, perObject, perExecute});
			gaussianBlurShader->SetDispatchThreadGroupCounts({1920 / 8, 1080 / 8, 6});
		}

		{
			RootParameter perFrame = {0, 1, 1};
			RootParameter perObject = {};
			RootParameter perExecute = {};


			additiveBlendingHdrShader->SetRootSignature({perFrame, perObject, perExecute});
			additiveBlendingHdrShader->SetDispatchThreadGroupCounts({1920 / 8, 1080 / 8, 1});
		}

		{
			toneMappingShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			});

			RootParameter perFrame = {0, 1, 0};
			RootParameter perObject = {};
			RootParameter perExecute = {};


			toneMappingShader->SetRootSignature({perFrame, perObject, perExecute, {CD3DX12_STATIC_SAMPLER_DESC(0)}});
			toneMappingShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		}

		{
			lightingPassShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			});


			RootParameter perFrame = {1, 6, 0};
			RootParameter perObject = {};
			RootParameter perExecute = {};


			lightingPassShader->SetRootSignature({perFrame, perObject, perExecute, {CD3DX12_STATIC_SAMPLER_DESC(0)}});
			lightingPassShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		}

		{
			unlitShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

				{"MATROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"MATROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"MATROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"MATROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
			});

			RootParameter perFrame = {1, 0, 0};
			RootParameter perObject = {1, 0, 0};
			RootParameter perExecute = {0, 10, 0};

			unlitShader->SetRootSignature({perFrame, perObject, perExecute, {CD3DX12_STATIC_SAMPLER_DESC(0)}});
			unlitShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		}


		m_LightPipelines.push_back(CreatePipeline<GeometryStaticPipeline>(pbrStaticShader, "PBRStaticPipeline"));
		m_LightPipelines.push_back(
			CreatePipeline<LightingSkeletalPipeline>(pbrSkeletalShader, "PBRSkeletalPipeline"));
		m_LightPipelines.push_back(CreatePipeline<LightingPipeline>(lightingPassShader, "LightingPipeline"));


		m_Pipelines.push_back(CreatePipeline<LinePipeline>(simpleColorShader, "SimpleColorLinePipeline"));
		m_Pipelines.push_back(CreatePipeline<CubemapPipeline>(skyboxShader, "CubemapPipeline"));
		m_Pipelines.push_back(CreatePipeline<PanoToCubemapPipeline>(panoToCubemapShader, "PanoToCubemapPipeline"));
		m_Pipelines.push_back(
			CreatePipeline<DiffIrradSamplingPipeline>(diffIrradianceShader, "DiffIradiancePipeline"));
		m_Pipelines.push_back(CreatePipeline<BlurPipeline>(gaussianBlurShader, "GaussianBlurPipeline"));
		m_Pipelines.push_back(
			CreatePipeline<AdditiveBlendingPipeline>(additiveBlendingHdrShader, "AdditiveBlendingPipeline"));
		m_Pipelines.push_back(CreatePipeline<TonemappingPipeline>(toneMappingShader, "TonemappingPipeline"));
		m_Pipelines.push_back(CreatePipeline<UnlitPipeline>(unlitShader, "UnlitPipeline"));


		auto level = Cast<Level>(GetOuter());
		auto& scene = level->GetScene();

		scene->PanoramaTexture = resourceManager.GetTexture("assets/textures/cubemaps/T_Skybox_11_HybridNoise.hdr");

		scene->CubemapTexture = CreateShared<Texture>(2048, 2048, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                             D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                             D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
		                                             D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);


		scene->IrradianceTexture = CreateShared<Texture>(32, 32, 6, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE |
		                                                D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);


		/// COMPUTE

		auto commandQueue = Device::Instance().GetCommandQueue();
		auto commandList = commandQueue->GetCommandList();

		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

		resourceBarrier.Transition.pResource = scene->CubemapTexture->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		m_Pipelines[2]->Begin();
		m_Pipelines[2]->Record(commandList);

		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		commandList->ResourceBarrier(1, &resourceBarrier);

		resourceBarrier.Transition.pResource = scene->IrradianceTexture->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);


		m_Pipelines[3]->Begin();
		m_Pipelines[3]->Record(commandList);

		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		commandQueue->Execute(commandList);
		CS_INFO("HDRI로부터 Cubemap 생성중...");
		commandQueue->Flush();
		CS_INFO("HDRI로부터 Cubemap 생성 완료");
	}

	void RenderSystem::Update(const float deltaTime)
	{
		ChangeResolution(1920, 1080);
		ChangeDisplayMode();


		auto commandQueue = Device::Instance().GetCommandQueue();
		auto commandList = commandQueue->GetCommandList();


		const auto level = Cast<Level>(GetOuter());
		auto& scene = level->GetScene();


		if (!level)
		{
			// Level이 존재 하지 않으면, 그릴 것이 없습니다.
			return;
		}


		if(scene->Cameras.empty())
		{
			return;
		}
	
		const auto mainCamera = scene->Cameras[0].lock();
		if (!mainCamera)
		{
			//TODO : Use Default Camera
			return;
		}


		
		// const auto mainCamera

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->RSSetViewports(1, &mainCamera->GetViewport());
		commandList->RSSetScissorRects(1, &mainCamera->GetScissorRect());

		D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[] = {
			scene->AlbedoBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			scene->RoughnessMetallicAoBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			scene->EmissiveBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			scene->WorldNormalBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			scene->IrradianceBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			scene->WorldPositionBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D)
		};

		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = scene->ColorBufferTextures[m_RtvIndex]->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);


		commandList->OMSetRenderTargets(_countof(renderTargets), renderTargets, false,
		                                &scene->DepthStencilBufferTexture->GetDepthStencilView(
			                                D3D12_DSV_DIMENSION_TEXTURE2D));

		float clearColorValue[4] = {1.0f, 0.0f, 1.0f, 0.0f};
		float clearColorBlack[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		const auto clearDepthValue = 1.0f;
		const auto clearStencilValue = 0.0f;

		commandList->ClearRenderTargetView(
			scene->ColorBufferTextures[m_RtvIndex]->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			clearColorValue, 0, nullptr);
		commandList->ClearRenderTargetView(
			scene->FloatingPointBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			clearColorValue, 0, nullptr);
		commandList->ClearRenderTargetView(
			scene->BrightColorBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			clearColorBlack, 0, nullptr);

		//== Clear G-Buffers ========
		commandList->ClearRenderTargetView(scene->AlbedoBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
		                                   clearColorBlack, 0, nullptr);
		commandList->ClearRenderTargetView(
			scene->RoughnessMetallicAoBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			clearColorBlack, 0, nullptr);
		commandList->ClearRenderTargetView(scene->EmissiveBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
		                                   clearColorBlack, 0, nullptr);
		commandList->ClearRenderTargetView(
			scene->WorldNormalBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			clearColorBlack, 0, nullptr);
		commandList->ClearRenderTargetView(
			scene->IrradianceBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			clearColorBlack, 0, nullptr);
		commandList->ClearRenderTargetView(
			scene->WorldPositionBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			clearColorBlack, 0, nullptr);


		commandList->ClearDepthStencilView(
			scene->DepthStencilBufferTexture->GetDepthStencilView(D3D12_DSV_DIMENSION_TEXTURE2D),
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearDepthValue,
			clearStencilValue, 0, nullptr);


		m_LightPipelines[0]->Begin();
		m_LightPipelines[0]->Record(commandList);

		// TODO : CPU 복사로 인한 퍼포먼스 이슈


		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[] = {
			scene->FloatingPointBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D),
			scene->BrightColorBuffer->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D)
		};
		commandList->OMSetRenderTargets(_countof(rtvHandles), rtvHandles, false,
		                                &scene->DepthStencilBufferTexture->GetDepthStencilView(
			                                D3D12_DSV_DIMENSION_TEXTURE2D));

		m_LightPipelines[2]->Begin();
		m_LightPipelines[2]->Record(commandList);

		m_LightPipelines[1]->Begin();
		m_LightPipelines[1]->Record(commandList);


		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);


		m_Pipelines[0]->Begin();
		m_Pipelines[0]->Record(commandList);


		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_Pipelines[7]->Begin();
		m_Pipelines[7]->Record(commandList);


		m_Pipelines[1]->Begin();
		m_Pipelines[1]->Record(commandList);


		resourceBarrier.Transition.pResource = scene->BrightColorBuffer->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		m_Pipelines[4]->Begin();
		m_Pipelines[4]->Record(commandList);

		resourceBarrier.Transition.pResource = scene->BrightColorBuffer->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		resourceBarrier.Transition.pResource = scene->FloatingPointBuffer->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);


		m_Pipelines[5]->Begin();
		m_Pipelines[5]->Record(commandList);

		resourceBarrier.Transition.pResource = scene->FloatingPointBuffer->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);


		resourceBarrier.Transition.pResource = scene->BrightColorBuffer->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);


		commandList->OMSetRenderTargets(
			1, &scene->ColorBufferTextures[m_RtvIndex]->GetRenderTargetView(D3D12_RTV_DIMENSION_TEXTURE2D), false,
			&scene->DepthStencilBufferTexture->GetDepthStencilView(D3D12_DSV_DIMENSION_TEXTURE2D));

		m_Pipelines[6]->Begin();
		m_Pipelines[6]->Record(commandList);


		resourceBarrier.Transition.pResource = scene->FloatingPointBuffer->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			| D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);


		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = scene->ColorBufferTextures[m_RtvIndex]->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		commandQueue->Execute(commandList);

		DXGI_PRESENT_PARAMETERS presentParameters;
		presentParameters.DirtyRectsCount = 0;
		presentParameters.pDirtyRects = nullptr;
		presentParameters.pScrollRect = nullptr;
		presentParameters.pScrollOffset = nullptr;
		m_SwapChain->Present1(1, 0, &presentParameters);

		commandQueue->Flush();


		m_LightPipelines[0]->End();
		m_Pipelines[0]->End();
		m_Pipelines[7]->End();

		m_RtvIndex++;
		m_RtvIndex = m_RtvIndex % 2;


		m_CleanUpTimer.Tick();

		if (m_CleanUpTimer.GetElapsedTime() >= m_CleanUpTime)
		{
			m_CleanUpTimer.Reset();

			ResourceManager::Instance().RemoveGarbage();
			scene->RemoveGarbage();
			CS_DEBUG_INFO("Garbage Removed");
		}
	}

	void RenderSystem::ChangeResolution(int width, int height)
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


		hr = m_SwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM,
		                                DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_FATAL(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");


		auto level = Cast<Level>(GetOuter());
		auto& scene = level->GetScene();

		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));

			scene->ColorBufferTextures[i] = CreateShared<Texture>(rtvBuffer.Get(),
			                                                     D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		}


		scene->FloatingPointBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1,
		                                                  DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                  D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
		                                                  D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                  D3D12_RESOURCE_STATE_RENDER_TARGET);


		scene->BrightColorBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1,
		                                                DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
		                                                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                D3D12_RESOURCE_STATE_RENDER_TARGET);


		scene->DepthStencilBufferTexture = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 0,
		                                                        DXGI_FORMAT_D24_UNORM_S8_UINT,
		                                                        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		                                                        D3D12_RESOURCE_STATE_DEPTH_WRITE);

		if (!level)
		{
			// Level이 존재 하지 않으면, 그릴 것이 없습니다.
			return;
		}

		const auto playerController = level->GetPlayerController(0).lock();
		if (!playerController)
		{
			//TODO : Use Default Camera
			return;
		}

		const auto cameraComponent = scene->Cameras[0].lock();
		if (!cameraComponent)
		{
			//TODO : Use Default Camera
			return;
		}


		cameraComponent->SetViewport({0, 0, (FLOAT)width, (FLOAT)height, 0.0f, 1.0f});
		cameraComponent->SetScissorRect({0, 0, width, height});


		CS_INFO("해상도를 변경 완료.");
		CS_INFO("현재 해상도 : %d X %d", targetParam.Width, targetParam.Height);
	}

	void RenderSystem::ChangeDisplayMode()
	{
		BOOL bIsFullScreen = false;
		m_SwapChain->GetFullscreenState(&bIsFullScreen, nullptr);
		if (m_bIsFullScreen == static_cast<bool>(bIsFullScreen))
			return;


		CS_INFO("디스플레이 모드를 변환중...");


		HRESULT hr = m_SwapChain->SetFullscreenState(m_bIsFullScreen, nullptr);
		CS_FATAL(SUCCEEDED(hr), "디스플레이모드를 변환하는데 실패하였습니다.");


		hr = m_SwapChain->ResizeBuffers(2, m_ResWidth, m_ResHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
		                                DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_FATAL(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");


		auto level = Cast<Level>(GetOuter());
		auto& scene = level->GetScene();

		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));


			scene->ColorBufferTextures[i] = CreateShared<Texture>(rtvBuffer.Get(),
			                                                     D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		}

		scene->FloatingPointBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1,
		                                                  DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                  D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
		                                                  D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                  D3D12_RESOURCE_STATE_RENDER_TARGET);


		scene->BrightColorBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1,
		                                                DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
		                                                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                D3D12_RESOURCE_STATE_RENDER_TARGET);


		scene->DepthStencilBufferTexture = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 0,
		                                                        DXGI_FORMAT_D24_UNORM_S8_UINT,
		                                                        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		                                                        D3D12_RESOURCE_STATE_DEPTH_WRITE);


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

	void RenderSystem::CreateRenderTargets()
	{
		//============ SwapChain RenderTargets ==================

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
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDesc;
		swapChainFullscreenDesc.RefreshRate.Numerator = 60;
		swapChainFullscreenDesc.RefreshRate.Denominator = 1;
		swapChainFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainFullscreenDesc.Windowed = true;

		// #DirectX Create Swap Chain https://gamedev.stackexchange.com/questions/149822/direct3d-12-cant-create-a-swap-chain

		auto& device = Device::Instance();
		auto factory = device.GetFactory();

		auto level = Cast<Level>(GetOuter());
		auto& scene = level->GetScene();


		HWND currentHandle = GetActiveWindow();


		HRESULT hr = factory->CreateSwapChainForHwnd(device.GetCommandQueue()->GetRaw(), currentHandle, &swapChainDesc,
		                                             &swapChainFullscreenDesc, nullptr, m_SwapChain.GetAddressOf());
		CS_FATAL(SUCCEEDED(hr), "Swap Chain을 생성하는데 실패하였습니다");
		factory->MakeWindowAssociation(currentHandle, DXGI_MWA_NO_ALT_ENTER);


		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));


			scene->ColorBufferTextures[i] = CreateShared<Texture>(rtvBuffer.Get(),
			                                                     D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		}


		scene->FloatingPointBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1,
		                                                  DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                  D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
		                                                  D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                  D3D12_RESOURCE_STATE_RENDER_TARGET);


		scene->BrightColorBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1,
		                                                DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
		                                                D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                D3D12_RESOURCE_STATE_RENDER_TARGET);


		scene->DepthStencilBufferTexture = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 0,
		                                                        DXGI_FORMAT_D24_UNORM_S8_UINT,
		                                                        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		                                                        D3D12_RESOURCE_STATE_DEPTH_WRITE);


		//=== G-Buffers======


		scene->AlbedoBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                           D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		                                           D3D12_RESOURCE_STATE_RENDER_TARGET);

		scene->RoughnessMetallicAoBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1,
		                                                        DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                        D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		                                                        D3D12_RESOURCE_STATE_RENDER_TARGET);

		scene->EmissiveBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                             D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		                                             D3D12_RESOURCE_STATE_RENDER_TARGET);

		scene->WorldNormalBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1,
		                                                DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		                                                D3D12_RESOURCE_STATE_RENDER_TARGET);
		scene->IrradianceBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1,
		                                               DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                               D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		                                               D3D12_RESOURCE_STATE_RENDER_TARGET);
		scene->WorldPositionBuffer = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 1,
		                                                  DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                  D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		                                                  D3D12_RESOURCE_STATE_RENDER_TARGET);
	}


	void RenderSystem::CreateDepthStencilView()
	{
		auto level = Cast<Level>(GetOuter());
		auto& scene = level->GetScene();

		scene->DepthStencilBufferTexture = CreateShared<Texture>(m_ResWidth, m_ResHeight, 1, 0,
		                                                        DXGI_FORMAT_D24_UNORM_S8_UINT,
		                                                        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		                                                        D3D12_RESOURCE_STATE_DEPTH_WRITE);
	}

	void RenderSystem::RegisterPrimitiveComponentNew(std::weak_ptr<PrimitiveComponent> componentWeak)
	{
		auto component = componentWeak.lock();

		const auto& materials = component->GetMaterials();


		if (materials.empty())
			return;

		switch (materials[0]->ShadingModel)
		{
		case EShadingModel::ShadingModel_Undefined:
			m_Pipelines[7]->RegisterPipelineComponents(componentWeak);
			break;
		case EShadingModel::ShadingModel_Unlit:
			m_Pipelines[0]->RegisterPipelineComponents(componentWeak);
			break;
		case EShadingModel::ShadingModel_DefaultLit:
			{
				auto type = component->StaticType();
				if (type == "StaticMeshComponent")
				{
					m_LightPipelines[0]->RegisterPipelineComponents(componentWeak);
				}

				if (type == "SkeletalMeshComponent")
				{
					m_LightPipelines[1]->RegisterPipelineComponents(componentWeak);
				}

				break;
			}

		default: ;
		}
	}


}
