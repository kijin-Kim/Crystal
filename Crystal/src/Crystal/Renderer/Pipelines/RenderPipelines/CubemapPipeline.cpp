#include "cspch.h"
#include "CubemapPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Resources/Buffer.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/BasicShapeMeshes.h"
#include "Crystal/Renderer/Scene.h"

namespace Crystal {


	void CubemapPipeline::Begin()
	{
		RenderPipeline::Begin();

		PrepareConstantBuffers(sizeof(PerFrameData));



		PerFrameData perFrameData = {};


		auto& scene = GetScene();
		auto camera = scene->Cameras[0].lock();


		auto viewProj = camera->GetViewProjection();
		viewProj._41 = 0.0f; viewProj._42 = 0.0f; viewProj._43 = 0.0f;
		perFrameData.InverseViewProjection = Matrix4x4::Transpose(Matrix4x4::Inverse(viewProj));
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		auto device = Device::Instance().GetD3DDevice();

		auto descHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();



		device->CopyDescriptorsSimple(1, descHandle,
			m_PerFrameConstantBuffer->AsConstantBufferView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		device->CopyDescriptorsSimple(1, descHandle,
			scene->CubemapColorTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURECUBE), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, descHandle,
			scene->CubemapAlphaTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURECUBE), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, descHandle,
			scene->CubemapStarFarTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURECUBE), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, descHandle,
			scene->CubemapStarNearTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURECUBE), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		
	}

	void CubemapPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		auto device = Device::Instance().GetD3DDevice();
		auto rootSignature = m_Shader->GetRootSignature();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(rootSignature.GetData());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		commandList->SetGraphicsRootDescriptorTable(0, descriptorHeapHandle);


		auto renderable = GetScene()->PlaneQuad2DMesh;

		for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
		{
			renderable->Render(commandList, j);
		}

	}
}
