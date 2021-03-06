#include "cspch.h"
#include "TonemappingPipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {



	void TonemappingPipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 500;
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



		pipelineStateStream.RootSignature = m_Shader->GetRootSignature().GetData();
		auto inputLayout = m_Shader->GetInputLayout();
		pipelineStateStream.InputLayout = { inputLayout.GetData(), inputLayout.GetCount() };
		pipelineStateStream.PrimitiveTopology = m_Shader->GetPrimitiveTopologyType();


		auto& shaderDatablobs = m_Shader->GetRaw();

		if (m_Shader->HasShader(ShaderType::Vertex))
		{
			pipelineStateStream.VS = { shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
		}

		if (m_Shader->HasShader(ShaderType::Hull))
		{
			pipelineStateStream.HS = { shaderDatablobs[ShaderType::Hull]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Hull]->GetBufferSize() };
		}

		if (m_Shader->HasShader(ShaderType::Domain))
		{
			pipelineStateStream.DS = { shaderDatablobs[ShaderType::Domain]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Domain]->GetBufferSize() };
		}

		if (m_Shader->HasShader(ShaderType::Geometry))
		{
			pipelineStateStream.DS = { shaderDatablobs[ShaderType::Geometry]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Geometry]->GetBufferSize() };
		}

		if (m_Shader->HasShader(ShaderType::Pixel))
		{
			pipelineStateStream.PS = { shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Pixel]->GetBufferSize() };
		}

		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = false; // NO depth test just overwrite
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
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipelineStateStream.RTVFormats = rtvFormat;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };

		hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState));
		CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");


		
		m_StaticMeshComponent = std::make_shared<StaticMeshComponent>();
		m_StaticMeshComponent->SetRenderable(CreateShared<PlaneQuad2D>());
		
		m_Components.push_back(m_StaticMeshComponent);
		
		
	}

	void TonemappingPipeline::Begin()
	{
		RenderPipeline::Begin();


		auto device = Device::Instance().GetD3DDevice();

		auto heapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

		auto renderSystem = Cast<RenderSystem>(GetOuter());
		auto level = Cast<Level>(renderSystem->GetOuter());
		auto& scene = level->GetScene();

		

		device->CopyDescriptorsSimple(1, heapHandle,
			scene->FloatingPointBuffer->GetShaderResourceView(D3D12_SRV_DIMENSION_TEXTURE2D), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}


	
}



