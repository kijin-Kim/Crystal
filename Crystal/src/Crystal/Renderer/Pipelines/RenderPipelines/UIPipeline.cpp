#include "cspch.h"
#include "UIPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/ButtonComponent.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"

namespace Crystal {

	void UIPipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 1000;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV���� �����ϴµ� �����Ͽ����ϴ�.");


		CD3DX12_DESCRIPTOR_RANGE1 perExecuteDescriptorRanges[] = {
			{D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE},
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
		CS_FATAL(SUCCEEDED(hr), "Root Signature�� �ø���ȭ�ϴµ� �����Ͽ����ϴ�");
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(),
		                                 rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature�� �����ϴµ� �����Ͽ����ϴ�");


		D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};


		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;


		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {inputLayout, _countof(inputLayout)};
		RenderTargetDescription renderTargetDescription(rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);

		PipelineStateDescription pipelineStateDescription(
			inputLayoutDesc,
			StateHelper::NonPremultiplied,
			StateHelper::DepthDisable,
			StateHelper::CullCounterClock,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);

		pipelineStateDescription.CreatePipelineState(m_RootSignature, m_Shader, m_PipelineState);
	}

	void UIPipeline::Begin(const Shared<Scene>& scene)
	{
		auto descriptorHeapStartPtr = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;
		auto handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();


		auto& device = Device::Instance();
		auto d3dDevice = device.GetD3DDevice();


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


			m_SortedMeshes.insert({textureComponent->GetWorldPosition().z, {handle.ptr - descriptorHeapStartPtr}});

			PerObjectData perObjectData = {};
			perObjectData.World = Matrix4x4::Transpose(textureComponent->GetWorldTransform());

			auto& materials = textureComponent->GetMaterials();
			for (auto& mat : materials)
			{
				if(!mat)
				{
					continue;
				}
				perObjectData.AlbedoColor = mat->AlbedoColor;
				perObjectData.TintColor = mat->TintColor;
				perObjectData.Opacity = mat->Opacity;
				perObjectData.OpacityMultiplier = mat->OpacityMultiplier;
				perObjectData.bToggleAlbedoTexture = !mat->AlbedoTexture.expired() ? true : false;
				perObjectData.bToggleOpacityTexture = !mat->OpacityTexture.expired() ? true : false;
				perObjectData.bUseAlbedoTextureAlpha = mat->bUseAlbedoTextureAlpha;


				auto perObjectBuffer = BufferManager::Instance().GetConstantBuffer(&perObjectData, sizeof(PerObjectData));
				d3dDevice->CopyDescriptorsSimple(1, handle, perObjectBuffer->AsConstantBufferView(),
				                                 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				// �ؽ��ĸ� Heap�� Copy


				if (perObjectData.bToggleAlbedoTexture)
				{
					d3dDevice->CopyDescriptorsSimple(1, handle, mat->AlbedoTexture.lock()->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
					                                 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if (perObjectData.bToggleOpacityTexture)
				{
					d3dDevice->CopyDescriptorsSimple(1, handle, mat->OpacityTexture.lock()->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
					                                 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
		}


		for (auto& button : scene->Buttons)
		{
			auto buttonComponent = button.lock();
			if (!buttonComponent)
			{
				continue;
			}
			if (buttonComponent->GetHiddenInGame())
			{
				continue;
			}


			m_SortedMeshes.insert({buttonComponent->GetWorldPosition().z, {handle.ptr - descriptorHeapStartPtr}});

			PerObjectData perObjectData = {};
			perObjectData.World = Matrix4x4::Transpose(buttonComponent->GetWorldTransform());


			auto material = buttonComponent->GetNormalMaterial();

			EButtonState buttonState = buttonComponent->GetButtonRenderState();
			switch (buttonState)
			{
			case EButtonState::BS_Pressed:
				{
					auto mat = buttonComponent->GetPressedMaterial();
					if (mat)
					{
						material = mat;
					}
				}
				break;
			case EButtonState::BS_Hovered:
				{
					auto mat = buttonComponent->GetHoveredMaterial();
					if (mat)
					{
						material = mat;
					}
				}
				break;
			}

			if (!material)
			{
				continue;
			}

			perObjectData.AlbedoColor = material->AlbedoColor;
			perObjectData.TintColor = material->TintColor;
			perObjectData.Opacity = material->Opacity;
			perObjectData.OpacityMultiplier = material->OpacityMultiplier;
			perObjectData.bToggleAlbedoTexture = !material->AlbedoTexture.expired() ? true : false;
			perObjectData.bToggleOpacityTexture = !material->OpacityTexture.expired() ? true : false;
			perObjectData.bUseAlbedoTextureAlpha = material->bUseAlbedoTextureAlpha;


			auto perObjectBuffer = BufferManager::Instance().GetConstantBuffer(&perObjectData, sizeof(PerObjectData));
			d3dDevice->CopyDescriptorsSimple(1, handle, perObjectBuffer->AsConstantBufferView(),
			                                 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			// �ؽ��ĸ� Heap�� Copy


			if (perObjectData.bToggleAlbedoTexture)
			{
				d3dDevice->CopyDescriptorsSimple(1, handle, material->AlbedoTexture.lock()->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
				                                 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
			handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			if (perObjectData.bToggleOpacityTexture)
			{
				d3dDevice->CopyDescriptorsSimple(1, handle, material->OpacityTexture.lock()->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
				                                 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}
			handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
	}

	void UIPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = {m_DescriptorHeap.Get()};
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);


		auto& scene = GetScene();


		PerFrameData perFrameData = {};
		perFrameData.View = Matrix4x4::Transpose(Matrix4x4::LookTo({0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}));
		perFrameData.Projection = Matrix4x4::Transpose(Matrix4x4::OrthoGraphicOffCenter(0.0f, 1920.0f, 0.0f, 1080.0f, 0.1f, 1000.0f));

		auto perFrameBuffer = BufferManager::Instance().GetConstantBuffer(&perFrameData, sizeof(PerFrameData));

		commandList->SetGraphicsRootConstantBufferView(0, perFrameBuffer->GetGPUVirtualAddress());

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapStart = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		for (auto& sortedData : m_SortedMeshes)
		{
			auto handle = descriptorHeapStart;
			handle.ptr += sortedData.second.DescriptorHeapOffsets;
			commandList->SetGraphicsRootDescriptorTable(1, handle);


			std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;

			auto& vertexBuffers = scene->PlaneQuad2DMesh->GetVertexBuffers();
			for (const auto& vertexbuffer : vertexBuffers)
			{
				vertexBufferViews.push_back(vertexbuffer->AsVertexBufferView(vertexbuffer->GetSize() / vertexbuffer->GetElementCount()));
			}

			commandList->IASetVertexBuffers(0, vertexBufferViews.size(), vertexBufferViews.data());
			commandList->IASetIndexBuffer(&scene->PlaneQuad2DMesh->GetIndexBuffers()[0]->AsIndexBufferView());
			commandList->DrawIndexedInstanced(scene->PlaneQuad2DMesh->GetIndexBuffers()[0]->GetElementCount(), 1, 0, 0, 0);
		}
	}

	void UIPipeline::End()
	{
		m_SortedMeshes.clear();
	}
}


