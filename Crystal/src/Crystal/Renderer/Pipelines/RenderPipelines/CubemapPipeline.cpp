#include "cspch.h"
#include "CubemapPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Resources/Buffer.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/BasicShapeMeshes.h"

namespace Crystal {


	void CubemapPipeline::OnCreate()
	{
		RenderPipeline::OnCreate();

		m_StaticMeshComponent = std::make_shared<StaticMeshComponent>();
		m_StaticMeshComponent->OnCreate();


		auto& resourceManager = ResourceManager::Instance();
		
		m_StaticMeshComponent->SetRenderable(CreateShared<PlaneQuad2D>());

		m_Components.push_back(m_StaticMeshComponent);
	}

	void CubemapPipeline::Begin(const PipelineInputs* const pipelineInputs)
	{
		RenderPipeline::Begin(pipelineInputs);

		PrepareConstantBuffers(sizeof(PerFrameData));

		CubemapPipelineInputs* cubemapPipelineInputs = (CubemapPipelineInputs*)pipelineInputs;


		PerFrameData perFrameData = {};




		auto viewProj = cubemapPipelineInputs->Camera->GetViewProjection();
		viewProj._41 = 0.0f; viewProj._42 = 0.0f; viewProj._43 = 0.0f;
		perFrameData.InverseViewProjection = Matrix4x4::Transpose(Matrix4x4::Inverse(viewProj));
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		auto device = Device::Instance().GetD3DDevice();

		auto descHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();



		device->CopyDescriptorsSimple(1, descHandle,
			m_PerFrameConstantBuffer->GetConstantBufferView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		auto& resourceManager = ResourceManager::Instance();
		
		device->CopyDescriptorsSimple(1, descHandle,
			resourceManager.GetTexture("Cube_Skybox_Space").lock()->GetShaderResourceView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		



		
	}
}
