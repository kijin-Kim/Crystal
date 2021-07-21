#include "cspch.h"
#include "UnlitPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"


namespace Crystal {

	void UnlitPipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 20000;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


		

		CD3DX12_DESCRIPTOR_RANGE1 perExecuteDescriptorRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1},
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0}
		};

		CD3DX12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].InitAsConstantBufferView(0);
		rootParameters[1].InitAsDescriptorTable(_countof(perExecuteDescriptorRanges), perExecuteDescriptorRanges, D3D12_SHADER_VISIBILITY_ALL);


		CD3DX12_STATIC_SAMPLER_DESC staticSamplers[] = {
			CD3DX12_STATIC_SAMPLER_DESC(0)
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

			{"MATROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"SUBUV_INDEX", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		};


	

		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 2;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;


		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = { inputLayout, _countof(inputLayout) };
		RenderTargetDescription renderTargetDescription(rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);

		PipelineStateDescription pipelineStateDescription(
			inputLayoutDesc,
			StateHelper::Additive,
			StateHelper::DepthEnable,
			StateHelper::CullCounterClock,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);

		pipelineStateDescription.CreatePipelineState(m_RootSignature, m_Shader, m_PipelineState);



		m_PerFrameConstantBuffer = CreateUnique<Buffer>(nullptr, sizeof(PerFrameData), 0, true, true);
		m_PerDrawCallConstantBuffer = CreateUnique<Buffer>(nullptr, sizeof(PerFrameData), 0, true, true);
		m_PerDrawCallConstantBuffer->CreateConstantBufferView();
	}

	void UnlitPipeline::Begin()
	{
		auto& scene = GetScene();

		PerFrameData perFrameData = {};

		auto& mainCamera = scene->Cameras[0].lock();

		perFrameData.View = Matrix4x4::Transpose(mainCamera->GetView());
		perFrameData.Projection = Matrix4x4::Transpose(mainCamera->GetProjection());

		m_PerFrameConstantBuffer->SetData(&perFrameData, 0, sizeof(perFrameData));

		auto handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();


		auto& device = Device::Instance();
		auto d3dDevice = device.GetD3DDevice();


		m_InstanceBatches.clear();


		for (auto& p : scene->Particles)
		{
			auto particleComponent = p.lock();
			if (!particleComponent)
			{
				continue;
			}
			if(particleComponent->GetHideInGame())
			{
				continue;
			}

			auto& particles = particleComponent->GetParticles();


			for (auto& particle : particles)
			{
				PerInstanceData perInstanceData = {};


				perInstanceData.World = particle.World;
				perInstanceData.World._11 = perFrameData.View._11;
				perInstanceData.World._12 = perFrameData.View._12;
				perInstanceData.World._13 = perFrameData.View._13;
				perInstanceData.World._21 = perFrameData.View._21;
				perInstanceData.World._22 = perFrameData.View._22;
				perInstanceData.World._23 = perFrameData.View._23;
				perInstanceData.World._31 = perFrameData.View._31;
				perInstanceData.World._32 = perFrameData.View._32;
				perInstanceData.World._33 = perFrameData.View._33;

				

				perInstanceData.World = Matrix4x4::Multiply(perInstanceData.World, Matrix4x4::Scale(particle.Scale));
				perInstanceData.World = Matrix4x4::Multiply(perInstanceData.World, Matrix4x4::RotationAxis(mainCamera->GetWorldForwardVector(), 45.0f));
				perInstanceData.World = Matrix4x4::Multiply(perInstanceData.World, Matrix4x4::Translation(particle.Position));
				

				perInstanceData.SubUVIndex = particle.SubImageIndex;


				// 같은 텍스쳐를 쓰면 ㅂ묶을 수 있음

				auto& materials = particleComponent->GetMaterials();
				for (auto& mat : materials)
				{
					PerDrawCallData perDrawCallData = {};
					perDrawCallData.HorizontalSubImageCount = 6;
					perDrawCallData.VerticalSubImageCount = 6;

					m_PerDrawCallConstantBuffer->SetData(&perDrawCallData, 0, sizeof(perDrawCallData));


					auto texture = mat->AlbedoTexture.lock();
					if (!texture)
					{
						continue;
					}


					auto it = m_InstanceBatches.find(texture.get()); // 같은 텍스쳐를 쓰고있는 배치가 있으면,
					if (it != m_InstanceBatches.end())
					{
						++it->second.InstanceCount;
						it->second.PerInstanceDatas.push_back(perInstanceData);
						continue;
					}

					auto result = m_InstanceBatches.insert({texture.get(), InstanceBatch()});
					auto& newInstanceBatch = result.first->second;

					newInstanceBatch.DescriptorHeapOffset = handle.ptr - m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;


					d3dDevice->CopyDescriptorsSimple(1, handle, m_PerDrawCallConstantBuffer->GetConstantBufferView(),
					                                 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					// 텍스쳐를 Heap에 Copy
					d3dDevice->CopyDescriptorsSimple(1, handle, texture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
					                                 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
					handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


					newInstanceBatch.PerInstanceDatas.push_back(perInstanceData);
				}
			}
		}

		m_DrawDatas.clear();

		for (auto& batch : m_InstanceBatches)
		{
			DrawData drawData = {};

			drawData.InstanceCount = batch.second.InstanceCount;
			drawData.DescriptorHeapOffset = batch.second.DescriptorHeapOffset;

			drawData.InstanceVertexBuffer = CreateUnique<Buffer>(nullptr, sizeof(PerInstanceData) * batch.second.PerInstanceDatas.size(),
			                                                     batch.second.PerInstanceDatas.size(), false, true);
			drawData.InstanceVertexBuffer->SetData(batch.second.PerInstanceDatas.data(), 0, sizeof(PerInstanceData) * batch.second.PerInstanceDatas.size());

			m_DrawDatas.push_back(std::move(drawData));
		}
	}

	void UnlitPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto& device = Device::Instance();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = {m_DescriptorHeap.Get()};
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);


		auto& scene = GetScene();

		commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress());

		for (const auto& drawData : m_DrawDatas)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			descriptorHeapHandle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * drawData.DescriptorHeapOffset;
			commandList->SetGraphicsRootDescriptorTable(1, descriptorHeapHandle);


			std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;

			auto& vertexBuffers = scene->PlaneQuad3DMesh->GetVertexBuffers();
			for (const auto& vertexbuffer : vertexBuffers)
			{
				vertexBufferViews.push_back(vertexbuffer->GetVertexBufferView());
			}


			commandList->IASetVertexBuffers(0, vertexBufferViews.size(), vertexBufferViews.data());
			commandList->IASetVertexBuffers(1, 1, &drawData.InstanceVertexBuffer->GetVertexBufferView());
			commandList->IASetIndexBuffer(&scene->PlaneQuad3DMesh->GetIndexBuffers()[0]->GetIndexBufferView());
			commandList->DrawIndexedInstanced(scene->PlaneQuad3DMesh->GetIndexBuffers()[0]->GetElementCount(), drawData.InstanceCount, 0, 0, 0);
		}
	}


}
