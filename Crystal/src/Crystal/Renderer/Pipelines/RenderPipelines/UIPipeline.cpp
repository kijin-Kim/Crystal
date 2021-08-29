#include "cspch.h"
#include "UIPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"

namespace Crystal {

	void UIPipeline::OnCreate()
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
			{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE}
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


		D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

			{"MATROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},

			{"TOGGLE_ALBEDO_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"TOGGLE_OPACITY_TEXTURE", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"USE_ALBEDO_ALPHA", 0, DXGI_FORMAT_R32_UINT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"ALBEDO_COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"OPACITY", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"OPACITY_MULTIPLIER", 0, DXGI_FORMAT_R32_FLOAT, 1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
		};


		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;


		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {inputLayout, _countof(inputLayout)};
		RenderTargetDescription renderTargetDescription(rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);

		PipelineStateDescription pipelineStateDescription(
			inputLayoutDesc,
			StateHelper::NonPremultiplied,
			StateHelper::DepthEnable,
			StateHelper::CullCounterClock,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);
		
		pipelineStateDescription.CreatePipelineState(m_RootSignature, m_Shader, m_PipelineState);


		m_PerFrameConstantBuffer = CreateUnique<Buffer>(nullptr, sizeof(PerFrameData), 0, true, true);
	}

	void UIPipeline::Begin(const Shared<Scene>& scene)
	{
		PerFrameData perFrameData = {};

		auto& mainCamera = scene->Cameras[0].lock();

		perFrameData.View = Matrix4x4::Transpose(Matrix4x4::LookTo({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}));
		perFrameData.Projection = Matrix4x4::Transpose(Matrix4x4::OrthoGraphic(1920.0f, 1080.0f, 0.1f, 1000.0f));

		m_PerFrameConstantBuffer->SetData(&perFrameData, 0, sizeof(perFrameData));

		auto handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();


		auto& device = Device::Instance();
		auto d3dDevice = device.GetD3DDevice();


		m_InstanceBatches.clear();


		for (auto& texture : scene->Textures)
		{
			auto textureComponent = texture.lock();
			if (!textureComponent)
			{
				continue;
			}
			if (textureComponent->GetHiddenInGame())
			{
				continue;
			}


			PerInstanceData perInstanceData = {};


			perInstanceData.World = textureComponent->GetWorldTransform();




			// 같은 텍스쳐를 쓰면 ㅂ묶을 수 있음

			auto& materials = textureComponent->GetMaterials();
			for (auto& mat : materials)
			{
				
				perInstanceData.AlbedoColor = mat->AlbedoColor;
				perInstanceData.Opacity = mat->Opacity;
				perInstanceData.OpacityMultiplier = mat->OpacityMultiplier;

				perInstanceData.bToggleAlbedoTexture = !mat->AlbedoTexture.expired() ? true : false;
				perInstanceData.bToggleOpacityTexture = !mat->OpacityTexture.expired() ? true : false;
				perInstanceData.bUseAlbedoTextureAlpha = mat->bUseAlbedoTextureAlpha;


				auto it = m_InstanceBatches.find(mat.get()); // 같은 텍스쳐를 쓰고있는 배치가 있으면,
				if (it != m_InstanceBatches.end())
				{
					++it->second.InstanceCount;
					it->second.PerInstanceDatas.push_back(perInstanceData);
					continue;
				}

				auto result = m_InstanceBatches.insert({ mat.get(), InstanceBatch()});
				auto& newInstanceBatch = result.first->second;

				newInstanceBatch.DescriptorHeapOffset = handle.ptr - m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;

				// 텍스쳐를 Heap에 Copy

				if(perInstanceData.bToggleAlbedoTexture)
				{
					d3dDevice->CopyDescriptorsSimple(1, handle, mat->AlbedoTexture.lock()->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perInstanceData.bToggleOpacityTexture)
				{
					d3dDevice->CopyDescriptorsSimple(1, handle, mat->OpacityTexture.lock()->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


				newInstanceBatch.PerInstanceDatas.push_back(perInstanceData);
			}
		}

		m_DrawDatas.clear();

		for (auto& batch : m_InstanceBatches)
		{
			DrawData drawData = {};

			drawData.InstanceCount = batch.second.InstanceCount;
			drawData.DescriptorHeapOffset = batch.second.DescriptorHeapOffset;

			drawData.InstanceVertexBuffer = BufferManager::Instance().GetBuffer(batch.second.PerInstanceDatas.data(), sizeof(PerInstanceData) * batch.second.PerInstanceDatas.size(),
				batch.second.PerInstanceDatas.size(), true);

			m_DrawDatas.push_back(std::move(drawData));
		}
	}

	void UIPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
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
			descriptorHeapHandle.ptr += drawData.DescriptorHeapOffset;
			commandList->SetGraphicsRootDescriptorTable(1, descriptorHeapHandle);


			std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;

			auto& vertexBuffers = scene->PlaneQuad2DMesh->GetVertexBuffers();
			for (const auto& vertexbuffer : vertexBuffers)
			{
				vertexBufferViews.push_back(vertexbuffer->AsVertexBufferView(vertexbuffer->GetSize() / vertexbuffer->GetElementCount()));
			}


			commandList->IASetVertexBuffers(0, vertexBufferViews.size(), vertexBufferViews.data());
			commandList->IASetVertexBuffers(1, 1, &drawData.InstanceVertexBuffer->AsVertexBufferView(sizeof(PerInstanceData)));
			commandList->IASetIndexBuffer(&scene->PlaneQuad2DMesh->GetIndexBuffers()[0]->AsIndexBufferView());
			commandList->DrawIndexedInstanced(scene->PlaneQuad2DMesh->GetIndexBuffers()[0]->GetElementCount(), drawData.InstanceCount, 0, 0, 0);
		}
	}
}
