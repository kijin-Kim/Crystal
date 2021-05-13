#include "cspch.h"
#include "CubemapPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Resources/Buffer.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/Resources/ResourceManager.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/BasicShapeMeshes.h"

namespace Crystal {


	void CubemapPipeline::OnCreate()
	{
		RenderPipeline::OnCreate();

		m_StaticMeshComponent = std::make_shared<StaticMeshComponent>();
		m_StaticMeshComponent->OnCreate();


		
		m_StaticMeshComponent->SetRenderable(CreateShared<PlaneQuad2D>());

		m_Components.push_back(m_StaticMeshComponent);
	}

	void CubemapPipeline::Begin()
	{
		RenderPipeline::Begin();

		PrepareConstantBuffers(sizeof(PerFrameData));



		PerFrameData perFrameData = {};


		auto renderSystem = Cast<RenderSystem>(GetOuter());
		auto level = Cast<Level>(renderSystem->GetOuter());
		auto& scene = level->GetScene();
		auto camera = scene.Cameras[0].lock();


		auto viewProj = camera->GetViewProjection();
		viewProj._41 = 0.0f; viewProj._42 = 0.0f; viewProj._43 = 0.0f;
		perFrameData.InverseViewProjection = Matrix4x4::Transpose(Matrix4x4::Inverse(viewProj));
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		auto device = Device::Instance().GetD3DDevice();

		auto descHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();



		device->CopyDescriptorsSimple(1, descHandle,
			m_PerFrameConstantBuffer->GetConstantBufferView(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		
;		
		device->CopyDescriptorsSimple(1, descHandle,
			scene.CubemapTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURECUBE), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		descHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		



		
	}
}
