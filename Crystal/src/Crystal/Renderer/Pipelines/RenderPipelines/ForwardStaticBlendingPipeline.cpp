#include "cspch.h"
#include "ForwardStaticBlendingPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"

namespace Crystal {
	void ForwardStaticBlendingPipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 20000;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


		CD3DX12_DESCRIPTOR_RANGE1 perFrameDescriptorRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0},
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE}
		};

		CD3DX12_DESCRIPTOR_RANGE1 perExecuteDescriptorRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE}
		};


		CD3DX12_ROOT_PARAMETER1 rootParameters[4];
		rootParameters[0].InitAsDescriptorTable(_countof(perFrameDescriptorRanges), perFrameDescriptorRanges, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[1].InitAsConstantBufferView(1);
		rootParameters[2].InitAsConstantBufferView(2);
		rootParameters[3].InitAsDescriptorTable(_countof(perExecuteDescriptorRanges), perExecuteDescriptorRanges, D3D12_SHADER_VISIBILITY_ALL);


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
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(),
			rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");


		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

			{"ALBEDO_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"EMISSIVE_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"ROUGHNESS_CONSTANT", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"METALLIC_CONSTANT", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"SHOULD_LIT", 0, DXGI_FORMAT_R32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"OPACITY", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TOGGLE_ALBEDO_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TOGGLE_METALLIC_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TOGGLE_ROUGHNESS_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TOGGLE_NORMAL_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TOGGLE_IRRADIANCE_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TOGGLE_EMISSIVE_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		};


		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;


		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = { inputLayout, _countof(inputLayout) };
		RenderTargetDescription renderTargetDescription(rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);


		PipelineStateDescription noCullNonPre(
			inputLayoutDesc,
			StateHelper::NonPremultiplied,
			StateHelper::DepthEnable,
			StateHelper::CullNone,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);

		PipelineStateDescription cullNonPre(
			inputLayoutDesc,
			StateHelper::NonPremultiplied,
			StateHelper::DepthEnable,
			StateHelper::CullCounterClock,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);



		noCullNonPre.CreatePipelineState(m_RootSignature, m_Shader, m_PipelineStates[{EBlendMode::BM_Translucent, true}]);
		cullNonPre.CreatePipelineState(m_RootSignature, m_Shader, m_PipelineStates[{EBlendMode::BM_Translucent, false}]);

	}

	void ForwardStaticBlendingPipeline::Begin()
	{
		RenderPipeline::Begin();

		

		auto device = Device::Instance().GetD3DDevice();

		auto& scene = GetScene();


		PerFrameData perFrameData = {};


		auto cameraComponent = scene->Cameras[0].lock();
		perFrameData.ViewProjection = Matrix4x4::Transpose(cameraComponent->GetViewProjection());

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

		m_PerFrameConstantBuffer = BufferManager::Instance().GetConstantBuffer(&perFrameData, sizeof(perFrameData));


		D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->AsConstantBufferView(),
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


		for (int i = 0; i < scene->StaticMeshes.size(); i++) // PerObject
		{
			auto meshComponent = scene->StaticMeshes[i].lock();
			if (!meshComponent)
				continue;

			if(meshComponent->GetHideInGame())
			{
				continue;
			}

			auto materials = meshComponent->GetMaterials();

			if (materials.empty())
				continue;

			if(materials[0]->BlendMode == EBlendMode::BM_Opaque)
				continue;
				

			auto lengthSquared = Vector3::LengthSquared(Vector3::Subtract(cameraComponent->GetWorldPosition(), meshComponent->GetWorldPosition()));
			
			m_AlphaSortedStaticMeshes.insert({ lengthSquared, meshComponent});

			auto cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			for (auto& mat : materials)
			{
				auto matRow = mat.get();

				PerDrawData perDrawData = {};

				perDrawData.AlbedoColor = matRow->AlbedoColor;
				perDrawData.EmissiveColor = matRow->EmissiveColor;
				perDrawData.RoughnessConstant = matRow->RoughnessConstant;
				perDrawData.MetallicConstant = matRow->MetallicConstant;

				perDrawData.bShouldLit = matRow->ShadingModel == EShadingModel::SM_Lit ? true : false;
				perDrawData.Opacity = matRow->Opacity;
				perDrawData.bToggleAlbedoTexture = !matRow->AlbedoTexture.expired() ? true : false;
				perDrawData.bToggleMetallicTexture = !matRow->MetallicTexture.expired() ? true : false;
				perDrawData.bToggleRoughnessTexture = !matRow->RoughnessTexture.expired() ? true : false;
				perDrawData.bToggleNormalTexture = !matRow->NormalTexture.expired() ? true : false;
				perDrawData.bToggleEmissiveTexture = !matRow->EmissiveTexture.expired() ? true : false;

				perDrawData.bToggleIrradianceTexture = true; // TEMP


				if (perDrawData.bToggleAlbedoTexture)
				{
					auto albedoTexture = matRow->AlbedoTexture.lock();
					device->CopyDescriptorsSimple(1, cpuHandle,
						albedoTexture->GetShaderResourceView(
							D3D12_SRV_DIMENSION_TEXTURE2D),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perDrawData.bToggleMetallicTexture)
				{
					auto metallicTexture = matRow->MetallicTexture.lock();
					device->CopyDescriptorsSimple(1, cpuHandle,
						metallicTexture->GetShaderResourceView(
							D3D12_SRV_DIMENSION_TEXTURE2D),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perDrawData.bToggleRoughnessTexture)
				{
					auto roughnessTexture = matRow->RoughnessTexture.lock();
					device->CopyDescriptorsSimple(1, cpuHandle,
						roughnessTexture->GetShaderResourceView(
							D3D12_SRV_DIMENSION_TEXTURE2D),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perDrawData.bToggleNormalTexture)
				{
					auto normalTexture = matRow->NormalTexture.lock();
					device->CopyDescriptorsSimple(1, cpuHandle,
						normalTexture->GetShaderResourceView(
							D3D12_SRV_DIMENSION_TEXTURE2D),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perDrawData.bToggleEmissiveTexture)
				{
					auto emissiveTexture = matRow->EmissiveTexture.lock();
					device->CopyDescriptorsSimple(1, cpuHandle,
						emissiveTexture->GetShaderResourceView(
							D3D12_SRV_DIMENSION_TEXTURE2D),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				
			}

		}

	}

	void ForwardStaticBlendingPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto device = Device::Instance().GetD3DDevice();


		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapHandle);
		descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		auto& scene = GetScene();


		auto it = m_AlphaSortedStaticMeshes.rbegin();		
		for (it; it != m_AlphaSortedStaticMeshes.rend(); ++it)
		{
			auto meshComponent = it->second.lock();
			if (!meshComponent)
			{
				continue;
			}

			if (meshComponent->GetHideInGame())
			{
				continue;
			}

			auto materials = meshComponent->GetMaterials();

			if (materials.empty())
				continue;

			if (materials[0]->BlendMode == EBlendMode::BM_Opaque)
				continue;

			

	
			auto renderable = meshComponent->GetRenderable().lock();

			if (!renderable)
			{
				continue;
			}

			PerObjectData perObjectData = {};
			perObjectData.World = Matrix4x4::Transpose(meshComponent->GetWorldTransform());
			commandList->SetGraphicsRootConstantBufferView(1, BufferManager::Instance().GetConstantBuffer(&perObjectData, sizeof(perObjectData))->GetGPUVirtualAddress());

			for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
			{
				if (j < materials.size() && materials[j])
				{
					PerDrawData perDrawData = {};

					perDrawData.AlbedoColor = materials[j]->AlbedoColor;
					perDrawData.EmissiveColor = materials[j]->EmissiveColor;
					perDrawData.RoughnessConstant = materials[j]->RoughnessConstant;
					perDrawData.MetallicConstant = materials[j]->MetallicConstant;

					perDrawData.bShouldLit = materials[j]->ShadingModel == EShadingModel::SM_Lit ? true : false;
					perDrawData.Opacity = materials[j]->Opacity;
					perDrawData.bToggleAlbedoTexture = !materials[j]->AlbedoTexture.expired() ? true : false;
					perDrawData.bToggleMetallicTexture = !materials[j]->MetallicTexture.expired() ? true : false;
					perDrawData.bToggleRoughnessTexture = !materials[j]->RoughnessTexture.expired() ? true : false;
					perDrawData.bToggleNormalTexture = !materials[j]->NormalTexture.expired() ? true : false;
					perDrawData.bToggleEmissiveTexture = !materials[j]->EmissiveTexture.expired() ? true : false;

					perDrawData.bToggleIrradianceTexture = true; // TEMP
					
					
					commandList->SetGraphicsRootConstantBufferView(2, BufferManager::Instance().GetConstantBuffer(&perDrawData, sizeof(perDrawData))->GetGPUVirtualAddress());
					commandList->SetGraphicsRootDescriptorTable(3, descriptorHeapHandle); // PerMaterial
					descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;


					commandList->SetPipelineState(m_PipelineStates[{materials[j]->BlendMode, materials[j]->bTwoSided}].Get());
				}
				renderable->Render(commandList, j);
			}
		}

	}

	void ForwardStaticBlendingPipeline::End()
	{
		if(!m_AlphaSortedStaticMeshes.empty())
		{
			m_AlphaSortedStaticMeshes.clear();
		}
	}


}
