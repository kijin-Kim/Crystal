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
#include "Pipelines/RenderPipelines/LightingStaticPipeline.h"
#include "Pipelines/RenderPipelines/LightPassPipeline.h"
#include "Pipelines/RenderPipelines/LinePipeline.h"
#include "Pipelines/RenderPipelines/TonemappingPipeline.h"
#include "Pipelines/RenderPipelines/UnlitPipeline.h"

namespace Crystal {

	RenderSystem::RenderSystem()
	{		
		auto d3dDevice = Device::Instance().GetD3DDevice();


		CreateRenderTargets();
		CreateDepthStencilView();

		auto& resourceManager = ResourceManager::Instance();

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

		auto pbrStaticShader = resourceManager.CreateShaderFromFile("assets/shaders/GeometryPass_Static.hlsl", "PBRShader_Static").lock();
		auto pbrSkeletalShader = resourceManager.CreateShaderFromFile("assets/shaders/PBRShader_Skeletal.hlsl", "PBRShader_Skeletal").lock();
		auto skyboxShader = resourceManager.CreateShaderFromFile("assets/shaders/SkyboxShader.hlsl", "Skybox").lock();
		auto panoToCubemapShader = resourceManager.CreateShaderFromFile("assets/shaders/EquirectangularToCube.hlsl", "PanoToCubemap").lock();
		auto diffIrradianceShader = resourceManager.CreateShaderFromFile("assets/shaders/DiffuseIrradianceSampling.hlsl", "DiffuseIrradianceSampling").lock();
		auto specularIrradianceShader = resourceManager.CreateShaderFromFile("assets/shaders/SpecularIrradianceSampling.hlsl", "SpecularIrradianceSampling").lock();
		auto simpleColorShader = resourceManager.CreateShaderFromFile("assets/shaders/SimpleColorShader.hlsl", "SimpleColorShader").lock();
		auto gaussianBlurShader = resourceManager.CreateShaderFromFile("assets/shaders/GaussianBlur.hlsl", "GaussianBlur").lock();
		auto additiveBlendingHdrShader = resourceManager.CreateShaderFromFile("assets/shaders/AdditiveBlending.hlsl", "AdditiveBelndingHDR").lock();
		auto toneMappingShader = resourceManager.CreateShaderFromFile("assets/shaders/Tonemapping.hlsl", "Tonemapping").lock();
		auto lightingPassShader = resourceManager.CreateShaderFromFile("assets/shaders/LightingPass.hlsl", "LightingPass").lock();
		auto unlitShader = resourceManager.CreateShaderFromFile("assets/shaders/UnlitShader.hlsl", "UnlitPass").lock();

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
				{"ALBEDO_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 68, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"EMISSIVE_COLOR", 0,DXGI_FORMAT_R32G32B32_FLOAT, 1, 80, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"ROUGHNESS_CONSTANT", 0, DXGI_FORMAT_R32_FLOAT, 1, 92, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"METALLIC_CONSTANT", 0, DXGI_FORMAT_R32_FLOAT, 1, 96, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"TOGGLE_ALBEDO_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 100, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"TOGGLE_METALLIC_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 104, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"TOGGLE_ROUGHNESS_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 108, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"TOGGLE_NORMAL_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 112, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"TOGGLE_IRRADIANCE_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 116, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				{"TOGGLE_EMISSIVE_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, 120, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
				});


			RootParameter perFrame = { 1, 1, 0 };
			RootParameter perObject = { 0, 0, 0 };
			RootParameter perExecute = { 0, 50, 0 };

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


			gaussianBlurShader->SetRootSignature({ perFrame, perObject, perExecute });
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
			toneMappingShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				});

			RootParameter perFrame = { 0, 1, 0 };
			RootParameter perObject = {};
			RootParameter perExecute = {};


			toneMappingShader->SetRootSignature({ perFrame, perObject, perExecute, { CD3DX12_STATIC_SAMPLER_DESC(0)} });
			toneMappingShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

		}

		{
			lightingPassShader->SetInputLayout({
				{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
				});


			RootParameter perFrame = { 1, 6, 0 };
			RootParameter perObject = {};
			RootParameter perExecute = {};


			lightingPassShader->SetRootSignature({ perFrame, perObject, perExecute, { CD3DX12_STATIC_SAMPLER_DESC(0)} });
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

			RootParameter perFrame = { 1, 0, 0 };
			RootParameter perObject = { 1, 0, 0 };
			RootParameter perExecute = { 0, 10, 0};

			unlitShader->SetRootSignature({ perFrame, perObject, perExecute, {CD3DX12_STATIC_SAMPLER_DESC(0)} });
			unlitShader->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);	
		}

		LoadEngineContents();
		



		m_LightPipelines.push_back(CreatePipeline<LightingStaticPipeline>(pbrStaticShader, "PBRStaticPipeline"));
		m_LightPipelines.push_back(CreatePipeline<LightingSkeletalPipeline>(pbrSkeletalShader, "PBRSkeletalPipeline"));
		m_LightPipelines.push_back(CreatePipeline<LightPassPipeline>(lightingPassShader, "LightPassPipeline"));
		

		m_Pipelines.push_back(CreatePipeline<LinePipeline>(simpleColorShader, "SimpleColorLinePipeline"));
		m_Pipelines.push_back(CreatePipeline<CubemapPipeline>(skyboxShader, "CubemapPipeline"));
		m_Pipelines.push_back(CreatePipeline<PanoToCubemapPipeline>(panoToCubemapShader, "PanoToCubemapPipeline"));
		m_Pipelines.push_back(CreatePipeline<DiffIrradSamplingPipeline>(diffIrradianceShader, "DiffIradiancePipeline"));
		m_Pipelines.push_back(CreatePipeline<BlurPipeline>(gaussianBlurShader, "GaussianBlurPipeline"));
		m_Pipelines.push_back(CreatePipeline<AdditiveBlendingPipeline>(additiveBlendingHdrShader, "AdditiveBlendingPipeline"));
		m_Pipelines.push_back(CreatePipeline<TonemappingPipeline>(toneMappingShader, "TonemappingPipeline"));
		m_Pipelines.push_back(CreatePipeline<UnlitPipeline>(unlitShader, "UnlitPipeline"));


		
	}



	void RenderSystem::Begin()
	{
		Actor::Begin();

		/// COMPUTE
		auto commandQueue = Device::Instance().GetCommandQueue();
		auto commandList = commandQueue->GetCommandList();

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
		m_Pipelines[2]->Begin(&panoToCubemapPipelineInputs);
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

		m_Pipelines[3]->Begin(&diffIrradSamplingPipelineInputs);
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

	void RenderSystem::LoadEngineContents()
	{
		auto& resourceManager = ResourceManager::Instance();
		resourceManager.CreateRenderable<Line>("LineMesh");
		resourceManager.CreateRenderable<LineBox>("LineBoxMesh");
		resourceManager.CreateRenderable<LineSphere>("LineSphereMesh");
		resourceManager.CreateRenderable<PlaneQuad2D>("2DPlaneQuadMesh");
		resourceManager.CreateRenderable<PlaneQuad3D>("3DPlaneQuadMesh");
		resourceManager.CreateRenderable<PlaneQuad3DTextured>("3DPlaneQuadMeshTextured");
		resourceManager.CreateRenderableFromFile<StaticMesh>("assets/models/Sphere.fbx", "Sphere");
	}



	void RenderSystem::Update(const float deltaTime)
	{
		ChangeResolution(1920, 1080);
		ChangeDisplayMode();


		auto commandQueue = Device::Instance().GetCommandQueue();
		auto commandList = commandQueue->GetCommandList();


		D3D12_RESOURCE_BARRIER resourceBarrier = {};
		resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resourceBarrier.Transition.pResource = m_ColorBufferTextures[m_RtvIndex].lock()->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		const auto level = Cast<Level>(GetOuter());
		if (!level)
		{
			// Level이 존재 하지 않으면, 그릴 것이 없습니다.
			return;
		}

		const auto playerController = Cast<PlayerController>(level->GetActorByClass("PlayerController"));
		if (!playerController)
		{
			//TODO : Use Default Camera
			return;
		}

		const auto mainCamera = playerController->GetMainCamera().lock();
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
			m_AlbedoBuffer.lock()->GetRenderTargetView(),
			m_RoughnessMetallicAoBuffer.lock()->GetRenderTargetView(),
			m_EmissiveBuffer.lock()->GetRenderTargetView(),
			m_WorldNormalBuffer.lock()->GetRenderTargetView(),
			m_IrradianceBuffer.lock()->GetRenderTargetView(),
			m_WorldPositionBuffer.lock()->GetRenderTargetView()
		};


		commandList->OMSetRenderTargets(_countof(renderTargets), renderTargets, false,
		                                &m_DepthStencilBufferTexture.lock()->GetDepthStencilView());

		float clearColorValue[4] = {1.0f, 0.0f, 1.0f, 0.0f};
		float clearColorBlack[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		const auto clearDepthValue = 1.0f;
		const auto clearStencilValue = 0.0f;

		commandList->ClearRenderTargetView(m_ColorBufferTextures[m_RtvIndex].lock()->GetRenderTargetView(),
		                                   clearColorValue, 0, nullptr);
		commandList->ClearRenderTargetView(m_FloatingPointBuffer.lock()->GetRenderTargetView(),
		                                   clearColorValue, 0, nullptr);
		commandList->ClearRenderTargetView(m_BrightColorBuffer.lock()->GetRenderTargetView(),
		                                   clearColorBlack, 0, nullptr);

		//== Clear G-Buffers ========
		commandList->ClearRenderTargetView(m_AlbedoBuffer.lock()->GetRenderTargetView(),
		                                   clearColorBlack, 0, nullptr);
		commandList->ClearRenderTargetView(m_RoughnessMetallicAoBuffer.lock()->GetRenderTargetView(),
		                                   clearColorBlack, 0, nullptr);
		commandList->ClearRenderTargetView(m_EmissiveBuffer.lock()->GetRenderTargetView(),
		                                   clearColorBlack, 0, nullptr);
		commandList->ClearRenderTargetView(m_WorldNormalBuffer.lock()->GetRenderTargetView(),
		                                   clearColorBlack, 0, nullptr);
		commandList->ClearRenderTargetView(m_IrradianceBuffer.lock()->GetRenderTargetView(),
		                                   clearColorBlack, 0, nullptr);
		commandList->ClearRenderTargetView(m_WorldPositionBuffer.lock()->GetRenderTargetView(),
		                                   clearColorBlack, 0, nullptr);


		commandList->ClearDepthStencilView(m_DepthStencilBufferTexture.lock()->GetDepthStencilView(),
		                                   D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, clearDepthValue,
		                                   clearStencilValue, 0, nullptr);

		LightingStaticPipeline::LightingPipelineInputs lightingPipelineInputs = {};
		lightingPipelineInputs.Camera = mainCamera.get();
		lightingPipelineInputs.IrradiancemapTexture = m_IrradiancemapTexture.get();

		m_LightPipelines[0]->Begin(&lightingPipelineInputs);
		m_LightPipelines[0]->Record(commandList);

		/*m_LightPipelines[1]->Begin(&lightingPipelineInputs);
		m_LightPipelines[1]->Record(commandList);*/


		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[] = {
			m_FloatingPointBuffer.lock()->GetRenderTargetView(),
			m_BrightColorBuffer.lock()->GetRenderTargetView()
		};
		commandList->OMSetRenderTargets(_countof(rtvHandles), rtvHandles, false,
		                                &m_DepthStencilBufferTexture.lock()->GetDepthStencilView());

		m_LightPipelines[2]->Begin(&lightingPipelineInputs);
		m_LightPipelines[2]->Record(commandList);


		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		LinePipeline::LinePipelineInputs wireframePipelineInputs = {};
		wireframePipelineInputs.Camera = mainCamera.get();

		m_Pipelines[0]->Begin(&wireframePipelineInputs);
		m_Pipelines[0]->Record(commandList);

		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_Pipelines[7]->Begin(&wireframePipelineInputs);
		m_Pipelines[7]->Record(commandList);


		CubemapPipeline::CubemapPipelineInputs cubemapPipelineInputs = {};
		cubemapPipelineInputs.Camera = mainCamera.get();

		m_Pipelines[1]->Begin(&cubemapPipelineInputs);
		m_Pipelines[1]->Record(commandList);


		resourceBarrier.Transition.pResource = m_BrightColorBuffer.lock()->GetResource();
		resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		commandList->ResourceBarrier(1, &resourceBarrier);

		m_Pipelines[4]->Begin(nullptr);
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
		m_Pipelines[5]->Begin(&input);
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

		m_Pipelines[6]->Begin(&input);
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


		auto& resourceManager = Crystal::ResourceManager::Instance();


		DestroyRenderTargets();

		hr = m_SwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM,
		                                DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_FATAL(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");

		for (int i = 0; i < 2; i++)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> rtvBuffer = nullptr;
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&rtvBuffer));

			m_ColorBufferTextures[i] = resourceManager.CreateTextureByResource(
				rtvBuffer.Get(), "ColorBuffer_" + std::to_string(i),
				D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
			m_ColorBufferTextures[i].lock()->CreateRenderTargetView(
				m_ColorBufferTextures[i].lock()->GetResource()->GetDesc().Format,
				D3D12_RTV_DIMENSION_TEXTURE2D);
			m_ColorBufferTextures[i].lock()->CreateShaderResourceView(
				m_ColorBufferTextures[i].lock()->GetResource()->GetDesc().Format,
				D3D12_SRV_DIMENSION_TEXTURE2D);
		}


		m_FloatingPointBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1,
		                                                      DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                      D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
		                                                      D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                      D3D12_RESOURCE_STATE_RENDER_TARGET,
		                                                      "FloatingPointBuffer");
		m_FloatingPointBuffer.lock()->CreateShaderResourceView(
			m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);
		m_FloatingPointBuffer.lock()->CreateRenderTargetView(
			m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_FloatingPointBuffer.lock()->CreateUnorderedAccessView(
			m_FloatingPointBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_UAV_DIMENSION_TEXTURE2D);


		m_BrightColorBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1,
		                                                    DXGI_FORMAT_R16G16B16A16_FLOAT,
		                                                    D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
		                                                    D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
		                                                    D3D12_RESOURCE_STATE_RENDER_TARGET, "BrightColorBuffer");
		m_BrightColorBuffer.lock()->CreateRenderTargetView(m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
		                                                   D3D12_RTV_DIMENSION_TEXTURE2D);
		m_BrightColorBuffer.lock()->CreateUnorderedAccessView(
			m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_UAV_DIMENSION_TEXTURE2D);
		m_BrightColorBuffer.lock()->CreateShaderResourceView(
			m_BrightColorBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);


		m_DepthStencilBufferTexture = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 0,
		                                                            DXGI_FORMAT_D24_UNORM_S8_UINT,
		                                                            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
		                                                            D3D12_RESOURCE_STATE_DEPTH_WRITE,
		                                                            "DepthStencilBuffer").lock();
		m_DepthStencilBufferTexture.lock()->CreateDepthStencilView(DXGI_FORMAT_D24_UNORM_S8_UINT,
		                                                           D3D12_DSV_DIMENSION_TEXTURE2D);


		const auto level = Cast<Level>(GetOuter());
		if (!level)
		{
			// Level이 존재 하지 않으면, 그릴 것이 없습니다.
			return;
		}

		const auto playerController = Cast<PlayerController>(level->GetActorByClass("PlayerController"));
		if (!playerController)
		{
			//TODO : Use Default Camera
			return;
		}

		const auto cameraComponent = playerController->GetMainCamera().lock();
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

		DestroyRenderTargets();


		hr = m_SwapChain->ResizeBuffers(2, m_ResWidth, m_ResHeight, DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		CS_FATAL(SUCCEEDED(hr), "버퍼를 Resize하는데 실패하였습니다.");


		auto& resourceManager = ResourceManager::Instance();

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
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC  swapChainFullscreenDesc;
		swapChainFullscreenDesc.RefreshRate.Numerator = 60;
		swapChainFullscreenDesc.RefreshRate.Denominator = 1;
		swapChainFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainFullscreenDesc.Windowed = true;

		// #DirectX Create Swap Chain https://gamedev.stackexchange.com/questions/149822/direct3d-12-cant-create-a-swap-chain

		auto& device = Device::Instance();
		auto factory = device.GetFactory();


		HWND currentHandle = GetActiveWindow();
		
		
		HRESULT hr = factory->CreateSwapChainForHwnd(device.GetCommandQueue()->GetRaw(), currentHandle, &swapChainDesc,
			&swapChainFullscreenDesc, nullptr, m_SwapChain.GetAddressOf());
		CS_FATAL(SUCCEEDED(hr), "Swap Chain을 생성하는데 실패하였습니다");
		factory->MakeWindowAssociation(currentHandle, DXGI_MWA_NO_ALT_ENTER);

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


		//=== G-Buffers======

		m_AlbedoBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			D3D12_RESOURCE_STATE_RENDER_TARGET, "AlbedoBuffer");
		m_AlbedoBuffer.lock()->CreateRenderTargetView(m_AlbedoBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_AlbedoBuffer.lock()->CreateShaderResourceView(m_AlbedoBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);


		m_RoughnessMetallicAoBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			D3D12_RESOURCE_STATE_RENDER_TARGET, "RoughnessMetallicAOBuffer");
		m_RoughnessMetallicAoBuffer.lock()->CreateRenderTargetView(m_RoughnessMetallicAoBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_RoughnessMetallicAoBuffer.lock()->CreateShaderResourceView(m_RoughnessMetallicAoBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);


		m_EmissiveBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			D3D12_RESOURCE_STATE_RENDER_TARGET, "EmissiveBuffer");
		m_EmissiveBuffer.lock()->CreateRenderTargetView(m_EmissiveBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_EmissiveBuffer.lock()->CreateShaderResourceView(m_EmissiveBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);


		m_WorldNormalBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			D3D12_RESOURCE_STATE_RENDER_TARGET, "WorldNormalBuffer");
		m_WorldNormalBuffer.lock()->CreateRenderTargetView(m_WorldNormalBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_WorldNormalBuffer.lock()->CreateShaderResourceView(m_WorldNormalBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);


		m_IrradianceBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			D3D12_RESOURCE_STATE_RENDER_TARGET, "IrradianceBuffer");
		m_IrradianceBuffer.lock()->CreateRenderTargetView(m_IrradianceBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_IrradianceBuffer.lock()->CreateShaderResourceView(m_IrradianceBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);


		m_WorldPositionBuffer = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
			D3D12_RESOURCE_STATE_RENDER_TARGET, "WorldPositionBuffer");
		m_WorldPositionBuffer.lock()->CreateRenderTargetView(m_WorldPositionBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_RTV_DIMENSION_TEXTURE2D);
		m_WorldPositionBuffer.lock()->CreateShaderResourceView(m_WorldPositionBuffer.lock()->GetResource()->GetDesc().Format,
			D3D12_SRV_DIMENSION_TEXTURE2D);


	}

	void RenderSystem::DestroyRenderTargets()
	{
		auto& resourceManager = ResourceManager::Instance();
		resourceManager.DestroyTexture("ColorBuffer_0");
		resourceManager.DestroyTexture("ColorBuffer_1");
		resourceManager.DestroyTexture("FloatingPointBuffer");
		resourceManager.DestroyTexture("BrightColorBuffer");
	}

	void RenderSystem::CreateDepthStencilView()
	{
		auto& resourceManager = ResourceManager::Instance();

		m_DepthStencilBufferTexture = resourceManager.CreateTexture(m_ResWidth, m_ResHeight, 1, 0,
			DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, "DepthStencilBuffer");
		m_DepthStencilBufferTexture.lock()->CreateDepthStencilView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_DSV_DIMENSION_TEXTURE2D);
	}

	void RenderSystem::RegisterLightComponent(std::weak_ptr<LightComponent> componentWeak)
	{
		for (const auto& pipeline : m_LightPipelines)
		{
			pipeline->RegisterLightComponents(componentWeak);
		}
	}

	void RenderSystem::RegisterPrimitiveComponentNew(std::weak_ptr<PrimitiveComponent> componentWeak)
	{
		auto component = componentWeak.lock();

		const auto& materials = component->GetMaterials();


		if (materials.empty())
			return;

		for (const auto& mat : materials)
		{
			switch (mat->ShadingModel)
			{
			case EShadingModel::ShadingModel_Undefined:
				m_Pipelines[7]->RegisterPipelineComponents(componentWeak);
				break;
			case EShadingModel::ShadingModel_Unlit:
				m_Pipelines[0]->RegisterPipelineComponents(componentWeak);
				break;
			case EShadingModel::ShadingModel_DefaultLit:
				m_LightPipelines[0]->RegisterPipelineComponents(componentWeak);
				break;
			default:;

				
			}

			
		}
		
	}


}
