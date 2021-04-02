#include "cspch.h"
#include "Pipelines.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {
	void Pipeline::PrepareRecord(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs)
	{
		/*CommandList를 녹화합니다*/
		//CS_DEBUG_INFO("Pipeline : %s Record", m_Name.c_str());
	}

	void RenderPipeline::OnCreate()
	{
		Pipeline::OnCreate();

		auto device = Renderer::Instance().GetDevice();

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


		auto shader = Cast<Shader>(GetObjectOwner(Pipeline::PipelineOwnerType::Owner_Shader));

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
		depthStencilDesc.DepthEnable = true;
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
	}

	void RenderPipeline::PrepareRecord(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs)
	{
		Pipeline::PrepareRecord(commandList, pipelineInputs);
	}

	void RenderPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto device = Renderer::Instance().GetDevice();
		auto shader = Cast<Shader>(GetObjectOwner(Pipeline::PipelineOwnerType::Owner_Shader));
		auto rootSignature = shader->GetRootSignature();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(rootSignature.GetData());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		int rootParameterIndex = 0;

		if (rootSignature.HasPerFrameParameter())
		{
			commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerFrameParameterIndex(), descriptorHeapHandle);
			descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
				* rootSignature.GetPerFrameDescriptorCount();

		}

		for (const auto& component : m_Components)
		{
			auto meshComponent = component.lock();
			if (!meshComponent)
				continue;

			if (rootSignature.HasPerObjectParameter())
			{
				commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerObjectParameterIndex(), descriptorHeapHandle); // PerObject
				descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
					* rootSignature.GetPerObjectDescriptorCount();
			}


			auto renderable = meshComponent->GetRenderable();
			auto materials = meshComponent->GetMaterials();
			for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
			{
				if (materials[j])
				{
					if (rootSignature.GetPerDrawDescriptorCount())
					{
						commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerDrawParameterIndex(), descriptorHeapHandle); // PerMaterial
						descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
							* rootSignature.GetPerDrawDescriptorCount();
					}

				}
				renderable->Render(commandList, j);
			}
		}
	}

}