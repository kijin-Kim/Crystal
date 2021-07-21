#include "cspch.h"
#include "ForwardStaticPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"
#include "Crystal/Resources/Meshes.h"

namespace Crystal {

	void ForwardStaticPipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 20000;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV���� �����ϴµ� �����Ͽ����ϴ�.");


		CD3DX12_DESCRIPTOR_RANGE1 perFrameDescriptorRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0},
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE}
		};

		CD3DX12_DESCRIPTOR_RANGE1 perExecuteDescriptorRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE}
		};


		CD3DX12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].InitAsDescriptorTable(_countof(perFrameDescriptorRanges), perFrameDescriptorRanges, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[1].InitAsDescriptorTable(_countof(perExecuteDescriptorRanges), perExecuteDescriptorRanges, D3D12_SHADER_VISIBILITY_ALL);


		CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] = {
			CD3DX12_STATIC_SAMPLER_DESC(0),
			CD3DX12_STATIC_SAMPLER_DESC(1, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			                            D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			                            D3D12_TEXTURE_ADDRESS_MODE_BORDER)
		};


		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(staticSamplers), staticSamplers,
		                     D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		hr = D3D12SerializeVersionedRootSignature(&rootSigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_FATAL(SUCCEEDED(hr), "Root Signature�� �ø���ȭ�ϴµ� �����Ͽ����ϴ�");
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(),
		                                 rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature�� �����ϴµ� �����Ͽ����ϴ�");


		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

			{"MATROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},

			
			{"ALBEDO_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"EMISSIVE_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"ROUGHNESS_CONSTANT", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"METALLIC_CONSTANT", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"SHOULD_LIT", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_ALBEDO_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_METALLIC_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_ROUGHNESS_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_NORMAL_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_IRRADIANCE_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_EMISSIVE_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		};


		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 6;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[2] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[3] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[4] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[5] = DXGI_FORMAT_R16G16B16A16_FLOAT;


		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {inputLayout, _countof(inputLayout)};
		RenderTargetDescription renderTargetDescription(rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);

		PipelineStateDescription pipelineStateDescription(
			inputLayoutDesc,
			StateHelper::Opaque,
			StateHelper::DepthEnable,
			StateHelper::CullCounterClock,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);

		pipelineStateDescription.CreatePipelineState(m_RootSignature, m_Shader, m_PipelineStates[{EBlendMode::BM_Opaque, true}]);


	}

	void ForwardStaticPipeline::Begin()
	{
		RenderPipeline::Begin();


		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerObjectData));

		auto device = Device::Instance().GetD3DDevice();

		auto& scene = GetScene();


		PerFrameData perFrameData = {};


		perFrameData.ViewProjection = Matrix4x4::Transpose(scene->Cameras[0].lock()->GetViewProjection());

		const int maxLightCount = 100;
		int lightCount = 0;

		for (const auto& weak : scene->Lights)
		{
			if (lightCount >= maxLightCount)
				break;

			auto lightComponent = weak.lock();
			if (!lightComponent)
				continue;


			perFrameData.Lights[lightCount].Direction = lightComponent->GetLocalForwardVector();

			perFrameData.Lights[lightCount].Color = lightComponent->GetLightColor();
			perFrameData.Lights[lightCount].Intensity = lightComponent->GetLightIntensity();


			lightCount++;
		}


		perFrameData.LightCount = lightCount;

		
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));


		D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->GetConstantBufferView(),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle,
		                              scene->IrradianceTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURECUBE),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle,
		                              scene->ShadowMapTexture->
		                                     GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		int materialCount = 0;
		/*���͸����� Shader Visible Descriptor Heap�� �����մϴ�.*/

		for (int i = 0; i < scene->StaticMeshes.size(); i++) // PerObject
		{
			auto meshComponent = scene->StaticMeshes[i].lock();
			if (!meshComponent)
				continue;

			if (meshComponent->GetHideInGame())
			{
				continue;
			}


			auto staticMesh = Cast<StaticMesh>(meshComponent->GetRenderable()).get();


			PerInstanceData perInstanceData = {};

			perInstanceData.World = meshComponent->GetWorldTransform();
			auto& materials = meshComponent->GetMaterials();


			if (materials.empty())
				continue;

			if (!((materials[0]->ShadingModel == EShadingModel::SM_Lit && materials[0]->BlendMode == EBlendMode::BM_Translucent)
				|| (materials[0]->ShadingModel == EShadingModel::SM_Unlit)))
				continue;


			for (auto& mat : materials)
			{
				auto matRow = mat.get();

				perInstanceData.bShouldLit = mat->ShadingModel == EShadingModel::SM_Lit ? true : false;
				perInstanceData.AlbedoColor = matRow->AlbedoColor;
				perInstanceData.EmissiveColor = matRow->EmissiveColor;
				perInstanceData.RoughnessConstant = matRow->RoughnessConstant;
				perInstanceData.MetallicConstant = matRow->MetallicConstant;

				perInstanceData.bToggleAlbedoTexture = !matRow->AlbedoTexture.expired() ? true : false;
				perInstanceData.bToggleMetallicTexture = !matRow->MetallicTexture.expired() ? true : false;
				perInstanceData.bToggleRoughnessTexture = !matRow->RoughnessTexture.expired() ? true : false;
				perInstanceData.bToggleNormalTexture = !matRow->NormalTexture.expired() ? true : false;
				perInstanceData.bToggleEmissivetexture = !matRow->EmissiveTexture.expired() ? true : false;

				perInstanceData.bToggleIrradianceTexture = true; // TEMP

				bool bFindMaterial = false;
				if (m_InstanceBatches[staticMesh].Material)
				{
					bFindMaterial = m_InstanceBatches[staticMesh].Material->UsingSameTextures(matRow);
				}

				if (bFindMaterial)
				{
					continue;
				}


				m_InstanceBatches[staticMesh].Material = matRow;


				if (m_InstanceBatches[staticMesh].DescriptorOffset == -1)
				{
					m_InstanceBatches[staticMesh].DescriptorOffset =
						device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
						* (3 + 5 * materialCount);
				}


				auto cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				cpuHandle.ptr += m_InstanceBatches[staticMesh].DescriptorOffset;
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
					* (5 * 0);


				if (perInstanceData.bToggleAlbedoTexture)
				{
					auto albedoTexture = matRow->AlbedoTexture.lock();
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              albedoTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleMetallicTexture)
				{
					auto metallicTexture = matRow->MetallicTexture.lock();
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              metallicTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleRoughnessTexture)
				{
					auto roughnessTexture = matRow->RoughnessTexture.lock();
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              roughnessTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleNormalTexture)
				{
					auto normalTexture = matRow->NormalTexture.lock();
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              normalTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleEmissivetexture)
				{
					auto emissiveTexture = matRow->EmissiveTexture.lock();
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              emissiveTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


				materialCount++;
			}
			m_InstanceBatches[staticMesh].PerInstanceDatas.push_back(perInstanceData);
		}


		for (auto& pair : m_InstanceBatches)
		{
			auto& instanceBatches = pair.second;

			if (!instanceBatches.PerInstanceVertexBuffer)
			{
				instanceBatches.PerInstanceVertexBuffer = std::make_unique<Buffer>(nullptr, sizeof(PerInstanceData) *
				                                                                   instanceBatches.PerInstanceDatas.size(),
				                                                                   instanceBatches.PerInstanceDatas.size(), false, true);
			}

			instanceBatches.PerInstanceVertexBuffer->SetData(instanceBatches.PerInstanceDatas.data(),
			                                                 0, sizeof(PerInstanceData) * instanceBatches.
			                                                                              PerInstanceDatas.size());
		}
	}

	void ForwardStaticPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto& device = Device::Instance();


		commandList->SetPipelineState(m_PipelineStates[{EBlendMode::BM_Opaque, true}].Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = {m_DescriptorHeap.Get()};
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapHandle);


		for (const auto& pair : m_InstanceBatches)
		{
			auto& renderable = pair.first;
			auto& instanceBatch = pair.second;
			auto& perInstanceVertexBuffer = instanceBatch.PerInstanceVertexBuffer;

			auto handle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			handle.ptr += instanceBatch.DescriptorOffset;
			commandList->SetGraphicsRootDescriptorTable(1, handle);


			commandList->IASetVertexBuffers(1, 1, &perInstanceVertexBuffer->GetVertexBufferView());

			if (!renderable)
			{
				continue;
			}

			//���⼭���� Texture2D Array Per Instance
			for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
			{
				renderable->Render(commandList, j, perInstanceVertexBuffer->GetSize() / sizeof(PerInstanceData));
			}
		}
	}

	void ForwardStaticPipeline::End()
	{
		m_InstanceBatches.clear();
	}
}
