#include "cspch.h"
#include "LightPassPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {


	void LightPassPipeline::OnCreate()
	{
		LightPipeline::OnCreate();

		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 20000;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


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


		auto shader = Cast<Shader>(GetOwner());

		pipelineStateStream.RootSignature = shader->GetRootSignature().GetData();
		auto inputLayout = shader->GetInputLayout();
		pipelineStateStream.InputLayout = { inputLayout.GetData(), inputLayout.GetCount() };
		pipelineStateStream.PrimitiveTopology = shader->GetPrimitiveTopologyType();


		auto& shaderDatablobs = shader->GetRaw();

		if (shader->HasShader(ShaderType::Vertex))
		{
			pipelineStateStream.VS = { shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
		}

		if (shader->HasShader(ShaderType::Hull))
		{
			pipelineStateStream.HS = { shaderDatablobs[ShaderType::Hull]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Hull]->GetBufferSize() };
		}

		if (shader->HasShader(ShaderType::Domain))
		{
			pipelineStateStream.DS = { shaderDatablobs[ShaderType::Domain]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Domain]->GetBufferSize() };
		}

		if (shader->HasShader(ShaderType::Geometry))
		{
			pipelineStateStream.DS = { shaderDatablobs[ShaderType::Geometry]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Geometry]->GetBufferSize() };
		}

		if (shader->HasShader(ShaderType::Pixel))
		{
			pipelineStateStream.PS = { shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Pixel]->GetBufferSize() };
		}

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = false;
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
		rtvFormat.NumRenderTargets = 2;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;

		pipelineStateStream.RTVFormats = rtvFormat;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };

		hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState));
		CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");


		auto& resourceManager = ResourceManager::Instance();
		
		m_StaticMeshComponent = std::make_shared<StaticMeshComponent>();
		m_StaticMeshComponent->SetRenderable(resourceManager.GetRenderable("2DPlaneQuadMesh"));

		m_Components.push_back(m_StaticMeshComponent);
		
	}

	void LightPassPipeline::Begin(const PipelineInputs* const pipelineInputs)
	{
		LightPipeline::Begin(pipelineInputs);


		PrepareConstantBuffers(sizeof(PerFrameData));

		auto input = (RenderPipelineInputs*)pipelineInputs;
		

		auto device = Device::Instance().GetD3DDevice();


		PerFrameData perFrameData = {};

		perFrameData.ViewProjection = Matrix4x4::Transpose(input->Camera->GetViewProjection());
		perFrameData.CameraPositionInWorld = input->Camera->GetWorldPosition();


		const int maxLightCount = 100;
		int lightCount = 0;
		for (const auto& weak : m_LocalLightComponents)
		{
			if (lightCount >= maxLightCount)
				break;

			auto localLightComponent = weak.lock();
			if (!localLightComponent)
				continue;

			auto lightPosition = perFrameData.Lights[lightCount].WorldPosition = localLightComponent->GetWorldPosition();

			perFrameData.Lights[lightCount].Color = localLightComponent->GetLightColor();
			perFrameData.Lights[lightCount].Intensity = localLightComponent->GetLightIntensity();


			lightCount++;
		}

		perFrameData.LightCount = lightCount;

		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		auto destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		device->CopyDescriptorsSimple(1, destHeapHandle, m_PerFrameConstantBuffer->GetConstantBufferView(), 
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		auto& resourceManager = ResourceManager::Instance();

		device->CopyDescriptorsSimple(1, destHeapHandle, resourceManager.GetTexture("AlbedoBuffer").lock()->GetShaderResourceView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle, resourceManager.GetTexture("RoughnessMetallicAOBuffer").lock()->GetShaderResourceView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle, resourceManager.GetTexture("EmissiveBuffer").lock()->GetShaderResourceView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle, resourceManager.GetTexture("WorldNormalBuffer").lock()->GetShaderResourceView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle, resourceManager.GetTexture("IrradianceBuffer").lock()->GetShaderResourceView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device->CopyDescriptorsSimple(1, destHeapHandle, resourceManager.GetTexture("WorldPositionBuffer").lock()->GetShaderResourceView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		

	}

}

