#include "cspch.h"
#include "PostProcessPipeline.h"
#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/Scene.h"

void Crystal::PostProcessPipeline::Begin(const Shared<Scene>& scene)
{
	ComputePipeline::Begin(scene);
	auto& device = Device::Instance();
	auto d3dDevice = device.GetD3DDevice();


	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dDevice->CopyDescriptorsSimple(1, handle, scene->FloatingPointBuffer->GetUnorderedAccessView(D3D12_UAV_DIMENSION_TEXTURE2D),
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	


	for (auto& weak : scene->PostProcesses)
	{
		auto postProcess = weak.lock();
		if (!postProcess)
		{
			continue;
		}

		if(postProcess->GetHiddenInGame())
		{
			continue;
		}

		auto& mats = postProcess->GetMaterials();
		if (mats.empty())
		{
			continue;
		}

		auto it = m_OrderSortedPostProcesses.insert({ postProcess->GetPostProcessOrder(), RenderData() });
		it->second.Component = postProcess;
		it->second.DescriptorHeapOffsets = handle.ptr - m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;
		
		PerExecuteData perExecuteData = {};


		perExecuteData.AlbedoColor = mats[0]->AlbedoColor;
		perExecuteData.AlbedoColorMultiplier = mats[0]->TintColor;
		perExecuteData.Opacity = mats[0]->Opacity;
		perExecuteData.OpacityMultiplier = mats[0]->OpacityMultiplier;
		perExecuteData.TexCoordMultiplier = mats[0]->TexCoordMultiplier;
		perExecuteData.bToggleAlbedoTexture = !mats[0]->AlbedoTexture.expired() ? true : false;
		perExecuteData.bToggleOpacityTexture = !mats[0]->OpacityTexture.expired() ? true : false;
		perExecuteData.bToggleSceneColorMaskTexture = !mats[0]->SceneColorMaskTexture.expired() ? true : false;
		


		auto perFrameBuffer = BufferManager::Instance().GetConstantBuffer(&perExecuteData, sizeof(perExecuteData));
		d3dDevice->CopyDescriptorsSimple(1, handle, perFrameBuffer->AsConstantBufferView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		if (perExecuteData.bToggleAlbedoTexture)
		{
			auto albedoTexture = mats[0]->AlbedoTexture.lock();
			d3dDevice->CopyDescriptorsSimple(1, handle, albedoTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
			                                 D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
		handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		if (perExecuteData.bToggleOpacityTexture)
		{
			auto opacityTexture = mats[0]->OpacityTexture.lock();
			d3dDevice->CopyDescriptorsSimple(1, handle, opacityTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
		handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		if (perExecuteData.bToggleSceneColorMaskTexture)
		{
			auto sceneColorMaskTexture = mats[0]->SceneColorMaskTexture.lock();
			d3dDevice->CopyDescriptorsSimple(1, handle, sceneColorMaskTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
		handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

}

void Crystal::PostProcessPipeline::End()
{
	ComputePipeline::End();

	if (!m_OrderSortedPostProcesses.empty())
	{
		m_OrderSortedPostProcesses.clear();
	}
}

void Crystal::PostProcessPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
{
	auto& device = Device::Instance();

	commandList->SetPipelineState(m_PipelineState.Get());
	commandList->SetComputeRootSignature(m_Shader->GetRootSignature().GetData());
	ID3D12DescriptorHeap* irradianceSamplingHeaps[] = { m_DescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(irradianceSamplingHeaps), irradianceSamplingHeaps);

	auto handle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	commandList->SetComputeRootDescriptorTable(0, handle);
	handle.ptr += device.GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);



	auto it = m_OrderSortedPostProcesses.begin();
	for(it; it != m_OrderSortedPostProcesses.end(); ++it)
	{
		auto postProcess = it->second.Component.lock();
		if (!postProcess)
		{
			continue;
		}


		auto descriptorHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();;
		descriptorHandle.ptr += it->second.DescriptorHeapOffsets;
		commandList->SetComputeRootDescriptorTable(1, descriptorHandle);

		auto dispatchThreadGroupCounts = m_Shader->GetDispatchThreadGroupCounts();
		commandList->Dispatch(dispatchThreadGroupCounts.x, dispatchThreadGroupCounts.y, dispatchThreadGroupCounts.z);
	}

	
}


