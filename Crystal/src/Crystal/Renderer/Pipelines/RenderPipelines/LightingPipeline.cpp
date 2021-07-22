#include "cspch.h"
#include "LightingPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {


	void LightingPipeline::OnCreate()
	{
		RenderPipeline::OnCreate();

		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 20000;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");





		auto inputLayout = m_Shader->GetInputLayout();
	
		
		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 2;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;

		

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = { inputLayout.GetData(), inputLayout.GetCount() };
		RenderTargetDescription renderTargetDescription(rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);

		PipelineStateDescription pipelineStateDescription(
			inputLayoutDesc,
			StateHelper::Opaque,
			StateHelper::DepthDisable,
			StateHelper::CullNone,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
		);

		pipelineStateDescription.CreatePipelineState(m_Shader->GetRootSignature().GetData(), m_Shader, m_PipelineState);


		m_StaticMeshComponent = std::make_shared<StaticMeshComponent>();


		auto& scene = GetScene();

		m_StaticMeshComponent->SetRenderable(scene->PlaneQuad2DMesh);

		m_Components.push_back(m_StaticMeshComponent);
	}

	void LightingPipeline::Begin()
	{
		RenderPipeline::Begin();


		PrepareConstantBuffers(sizeof(PerFrameData));


		auto device = Device::Instance().GetD3DDevice();

		auto renderSystem = Cast<RenderSystem>(GetOuter());
		auto level = Cast<Level>(renderSystem->GetOuter());
		auto& scene = level->GetScene();

		
	
		

		PerFrameData perFrameData = {};

		auto camera = scene->Cameras[0].lock();
		perFrameData.ViewProjection = Matrix4x4::Transpose(camera->GetViewProjection());
		
		auto shadowLightSource = scene->Lights[0].lock();
		perFrameData.LightViewProjection = Matrix4x4::Transpose(shadowLightSource->GetLightViewProjection());
		
		perFrameData.CameraPositionInWorld = camera->GetWorldPosition();


		


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

		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		auto destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->AsConstantBufferView(),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		device->CopyDescriptorsSimple(1, destHeapHandle,
		                              scene->AlbedoBuffer->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle,
		                              scene->RoughnessMetallicAoBuffer->
		                                     GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle,
		                              scene->EmissiveBuffer->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle,
		                              scene->WorldNormalBuffer->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle,
		                              scene->IrradianceTexture->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle,
		                              scene->WorldPositionBuffer->
		                                     GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle,
		                              scene->ShadowMapTexture->
		                                     GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_FLOAT),
		                              D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}


}
