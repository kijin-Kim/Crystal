#include "cspch.h"
#include "InstancedStaticMesh.h"
#include "Crystal/Core/Device.h"

namespace Crystal {


	void InstancedStaticMesh::AddInstance(const Shared<StaticMeshComponent>& meshComponent, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,
		uint32_t descriptorTableOffset)
	{
		if (meshComponent->GetHideInGame())
		{
			return;
		}

		auto staticMesh = Cast<StaticMesh>(meshComponent->GetRenderable()).get();


		auto& materials = meshComponent->GetMaterials();

		
		StaticMeshPerInstanceData perInstanceData = {};

		perInstanceData.World = meshComponent->GetWorldTransform();

		auto device = Device::Instance().GetD3DDevice();

		for (auto& mat : materials)
		{
			auto matRow = mat.get();

			
			perInstanceData.AlbedoColor = matRow->AlbedoColor;
			perInstanceData.EmissiveColor = matRow->EmissiveColor;
			perInstanceData.RoughnessConstant = matRow->RoughnessConstant;
			perInstanceData.MetallicConstant = matRow->MetallicConstant;
			
			perInstanceData.bShouldLit = matRow->ShadingModel == EShadingModel::SM_Lit ? true : false;
			perInstanceData.Opacity = matRow->Opacity;
			perInstanceData.bToggleAlbedoTexture = !matRow->AlbedoTexture.expired() ? true : false;
			perInstanceData.bToggleMetallicTexture = !matRow->MetallicTexture.expired() ? true : false;
			perInstanceData.bToggleRoughnessTexture = !matRow->RoughnessTexture.expired() ? true : false;
			perInstanceData.bToggleNormalTexture = !matRow->NormalTexture.expired() ? true : false;
			perInstanceData.bToggleEmissivetexture = !matRow->EmissiveTexture.expired() ? true : false;

			perInstanceData.bToggleIrradianceTexture = true; // TEMP

			bool bFindMaterial = false;
			if (InstancedStaticMeshes[staticMesh].HasMaterial())
			{
				bFindMaterial = InstancedStaticMeshes[staticMesh].Material->UsingSameTextures(matRow);
			}

			if (bFindMaterial)
			{
				continue;
			}

			InstancedStaticMeshes[staticMesh].Material = matRow;


			if (InstancedStaticMeshes[staticMesh].DescriptorOffset == -1)
			{
				InstancedStaticMeshes[staticMesh].DescriptorOffset =
					device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
					* (descriptorTableOffset + 5 * MaterialCount);
			}


			auto cpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
			cpuHandle.ptr += InstancedStaticMeshes[staticMesh].DescriptorOffset;


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


			MaterialCount++;
		}
		InstancedStaticMeshes[staticMesh].AddPerInstanceData(perInstanceData);
	}

	void InstancedStaticMesh::PrepareDynamicVertexBuffer()
	{
		auto& bufferManager = BufferManager::Instance();

		
		for (auto& pair : InstancedStaticMeshes)
		{
			auto& instancedStaticMeshes = pair.second;

			instancedStaticMeshes.PerInstanceVertexBuffer = bufferManager.GetBuffer(instancedStaticMeshes.PerInstanceDatas.data(), sizeof(StaticMeshPerInstanceData) *
				instancedStaticMeshes.PerInstanceDatas.size(),
				instancedStaticMeshes.PerInstanceDatas.size(), true);		
		}
	}

	void InstancedStaticMesh::Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t rootParameterIndex)
	{
		for (const auto& pair : InstancedStaticMeshes)
		{
			auto& renderable = pair.first;
			auto& instanceBatch = pair.second;
			auto& perInstanceVertexBuffer = instanceBatch.PerInstanceVertexBuffer;

			auto handle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
			handle.ptr += instanceBatch.DescriptorOffset;
			commandList->SetGraphicsRootDescriptorTable(rootParameterIndex, handle);


			commandList->IASetVertexBuffers(1, 1, &perInstanceVertexBuffer->AsVertexBufferView());

			if (!renderable)
			{
				continue;
			}

			//여기서부터 Texture2D Array Per Instance
			for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
			{
				renderable->Render(commandList, j, perInstanceVertexBuffer->GetSize() / sizeof(StaticMeshPerInstanceData));
			}
		}
	}


	void InstancedStaticMesh::Clear()
	{
		InstancedStaticMeshes.clear();
		MaterialCount = 0;
	}
}
