#include "cspch.h"
#include "GeometryStaticPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Resources/Meshes.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {
	void GeometryStaticPipeline::OnCreate()
	{
		Pipeline::OnCreate();

		auto device = Device::Instance().GetD3DDevice();


		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopology;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_HS HS;
			CD3DX12_PIPELINE_STATE_STREAM_DS DS;
			CD3DX12_PIPELINE_STATE_STREAM_GS GS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;


		CD3DX12_DESCRIPTOR_RANGE1 irradianceTextureRanage;
		irradianceTextureRanage.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_DESCRIPTOR_RANGE1 AlbedoTextureRange;
		AlbedoTextureRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

		CD3DX12_DESCRIPTOR_RANGE1 MetallicTextureRange;
		MetallicTextureRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

		CD3DX12_DESCRIPTOR_RANGE1 RoughnessTextureRange;
		RoughnessTextureRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);

		CD3DX12_DESCRIPTOR_RANGE1 NormalTextureRange;
		NormalTextureRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);

		CD3DX12_DESCRIPTOR_RANGE1 EmissiveTextureRange;
		EmissiveTextureRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);


		CD3DX12_ROOT_PARAMETER1 rootParameters[7];

		rootParameters[0].InitAsConstantBufferView(0);

		rootParameters[1].InitAsDescriptorTable(1, &irradianceTextureRanage);

		rootParameters[2].InitAsDescriptorTable(1, &AlbedoTextureRange);
		rootParameters[3].InitAsDescriptorTable(1, &MetallicTextureRange);
		rootParameters[4].InitAsDescriptorTable(1, &RoughnessTextureRange);
		rootParameters[5].InitAsDescriptorTable(1, &NormalTextureRange);
		rootParameters[6].InitAsDescriptorTable(1, &EmissiveTextureRange);

		CD3DX12_STATIC_SAMPLER_DESC staticSampler(0);

		
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &staticSampler,
		                     D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(),
		                                 rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");


		pipelineStateStream.RootSignature = m_RootSignature.Get();
		auto inputLayout = m_Shader->GetInputLayout();
		pipelineStateStream.InputLayout = {inputLayout.GetData(), inputLayout.GetCount()};
		pipelineStateStream.PrimitiveTopology = m_Shader->GetPrimitiveTopologyType();


		auto& shaderDatablobs = m_Shader->GetRaw();

		if (m_Shader->HasShader(ShaderType::Vertex))
		{
			pipelineStateStream.VS = {
				shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Vertex]->GetBufferSize()
			};
		}

		if (m_Shader->HasShader(ShaderType::Hull))
		{
			pipelineStateStream.HS = {
				shaderDatablobs[ShaderType::Hull]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Hull]->GetBufferSize()
			};
		}

		if (m_Shader->HasShader(ShaderType::Domain))
		{
			pipelineStateStream.DS = {
				shaderDatablobs[ShaderType::Domain]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Domain]->GetBufferSize()
			};
		}

		if (m_Shader->HasShader(ShaderType::Geometry))
		{
			pipelineStateStream.DS = {
				shaderDatablobs[ShaderType::Geometry]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Geometry]->GetBufferSize()
			};
		}

		if (m_Shader->HasShader(ShaderType::Pixel))
		{
			pipelineStateStream.PS = {
				shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Pixel]->GetBufferSize()
			};
		}

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

		pipelineStateStream.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(depthStencilDesc);

		pipelineStateStream.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 6;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[2] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[3] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[4] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[5] = DXGI_FORMAT_R16G16B16A16_FLOAT;

		pipelineStateStream.RTVFormats = rtvFormat;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {sizeof(pipelineStateStream), &pipelineStateStream};

		hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState));
		CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");
	}

	void GeometryStaticPipeline::Begin()
	{
		RenderPipeline::Begin();


		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerObjectData));

		auto device = Device::Instance().GetD3DDevice();

		auto& scene = GetScene();


		PerFrameData perFrameData = {};
		perFrameData.ViewProjection = Matrix4x4::Transpose(scene->Cameras[0].lock()->GetViewProjection());
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));


		D3D12_CPU_DESCRIPTOR_HANDLE irradianceTextureHandle = scene->IrradianceTexture->GetShaderResourceView(
			D3D12_SRV_DIMENSION_TEXTURE2D); // Per Frame

		D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->GetConstantBufferView(),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle,
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		int materialCount = 0;
		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/

		for (int i = 0; i < scene->StaticMeshes.size(); i++) // PerObject
		{
			auto meshComponent = scene->StaticMeshes[i].lock();
			if (!meshComponent)
				continue;

			auto staticMesh = Cast<StaticMesh>(meshComponent->GetRenderable()).get();


			PerInstanceData perInstanceData = {};

			perInstanceData.World = meshComponent->GetWorldTransform();
			auto& materials = meshComponent->GetMaterials();


			for (auto& mat : materials)
			{
				auto matRow = mat.get();

				perInstanceData.AlbedoColor = matRow->AlbedoColor;
				perInstanceData.EmissiveColor = matRow->EmissiveColor;
				perInstanceData.RoughnessConstant = matRow->RoughnessConstant;
				perInstanceData.MetallicConstant = matRow->MetallicConstant;

				perInstanceData.bToggleAlbedoTexture = matRow->AlbedoTexture ? true : false;
				perInstanceData.bToggleMetallicTexture = matRow->MetallicTexture ? true : false;
				perInstanceData.bToggleRoughnessTexture = matRow->RoughnessTexture ? true : false;
				perInstanceData.bToggleNormalTexture = matRow->NormalTexture ? true : false;
				perInstanceData.bToggleEmissivetexture = matRow->EmissiveTexture ? true : false;

				perInstanceData.bToggleIrradianceTexture = true; // TEMP

				auto it = std::find_if(m_InstanceBatches[staticMesh].MaterialLookup.begin(),
				                       m_InstanceBatches[staticMesh].MaterialLookup.end(), [&mat](Material* other)
				                       {
					                       return mat->UsingSameTextures(other);
				                       });

				bool bFindMaterial = it != m_InstanceBatches[staticMesh].MaterialLookup.end();
				if (bFindMaterial)
				{
					perInstanceData.MaterialIndex = it - m_InstanceBatches[staticMesh].MaterialLookup.begin();
					continue;
				}


				for (int j = 0; j < m_InstanceBatches[staticMesh].MaterialLookup.size(); j++)
				{
					if (m_InstanceBatches[staticMesh].MaterialLookup[j])
						continue;

					perInstanceData.MaterialIndex = j;
					m_InstanceBatches[staticMesh].MaterialLookup[j] = matRow;
					break;
				}


				if (m_InstanceBatches[staticMesh].DescriptorOffset == -1)
				{
					m_InstanceBatches[staticMesh].DescriptorOffset =
						device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
						* (2 + 50 * materialCount);
				}


				auto cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
				cpuHandle.ptr += m_InstanceBatches[staticMesh].DescriptorOffset;
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
					* (5 * perInstanceData.MaterialIndex);

				if (perInstanceData.bToggleAlbedoTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              matRow->AlbedoTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleMetallicTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              matRow->MetallicTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleRoughnessTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              matRow->RoughnessTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleNormalTexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              matRow->NormalTexture->GetShaderResourceView(
						                              D3D12_SRV_DIMENSION_TEXTURE2D),
					                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleEmissivetexture)
				{
					device->CopyDescriptorsSimple(1, cpuHandle,
					                              matRow->EmissiveTexture->GetShaderResourceView(
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

	void GeometryStaticPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto device = Device::Instance().GetD3DDevice();
		auto rootSignature = m_Shader->GetRootSignature();

		auto& scene = GetScene();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(rootSignature.GetData());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = {scene->SceneDescriptorHeap.Get()};
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);


		commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress()); // [0] : PerFrameBuffer


		CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(scene->SceneDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		commandList->SetGraphicsRootDescriptorTable(1, descriptorHeapHandle.Offset(scene->TextureHandleOffsets[scene->IrradianceTexture]));
		// [1] : IrradianceTexture


		Shared<Material> material;
		commandList->SetGraphicsRootDescriptorTable(2, descriptorHeapHandle.Offset(scene->TextureHandleOffsets[material->AlbedoTexture]));
		commandList->SetGraphicsRootDescriptorTable(3, descriptorHeapHandle.Offset(scene->TextureHandleOffsets[material->MetallicTexture]));
		commandList->SetGraphicsRootDescriptorTable(4, descriptorHeapHandle.Offset(scene->TextureHandleOffsets[material->RoughnessTexture]));
		commandList->SetGraphicsRootDescriptorTable(5, descriptorHeapHandle.Offset(scene->TextureHandleOffsets[material->NormalTexture]));
		commandList->SetGraphicsRootDescriptorTable(6, descriptorHeapHandle.Offset(scene->TextureHandleOffsets[material->EmissiveTexture]));

		commandList->IASetVertexBuffers(1, 1, &perInstanceVertexBuffer->GetVertexBufferView());
		commandList->IASetIndexBuffer();

		commandList->DrawIndexedInstanced()
	}

	void GeometryStaticPipeline::End()
	{
		m_InstanceBatches.clear();
	}


}
