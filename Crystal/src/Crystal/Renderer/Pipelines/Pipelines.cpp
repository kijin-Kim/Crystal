#include "cspch.h"
#include "Pipelines.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/Buffer.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/Resources/Material.h"

namespace Crystal {

	void Pipeline::RegisterPipelineComponents(std::weak_ptr<PrimitiveComponent> component)
	{
		m_Components.push_back(component);
	}

	void Pipeline::PrepareRecord(const PipelineInputs* const pipelineInputs)
	{
		/*CommandList를 녹화합니다*/
		//CS_DEBUG_INFO("Pipeline : %s Record", m_Name.c_str());
	}

	bool Pipeline::IsValidForThisPipeline(const std::shared_ptr<Material>& material)
	{
		if (!material)
			return false;

		auto thisPipelineShader = Cast<Shader>(GetObjectOwner(Pipeline::PipelineOwnerType::Owner_Shader));
		auto inputMaterialShader = Cast<Shader>(material->GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));

		return thisPipelineShader == inputMaterialShader;
	}

	bool Pipeline::IsValidForThisPipelineNew(const std::unique_ptr<MaterialBase>& material)
	{
		if (!material)
			return false;

		auto thisPipelineShader = Cast<Shader>(GetObjectOwner(Pipeline::PipelineOwnerType::Owner_Shader));
		auto inputMaterialShader = Cast<Shader>(material->GetObjectOwner(Pipeline::MaterialBase::MaterialOwnerType::Owner_Shader));

		return thisPipelineShader == inputMaterialShader;
	}

	void Pipeline::PrepareConstantBuffers(int perFrameBufferSize /*= 0*/, int perObjectBufferSize /*= 0*/,
		int perDrawBufferSize /*= 0*/, int perDrawBufferCount /*= 0*/)
	{
		if (perFrameBufferSize && !m_PerFrameConstantBuffer)
		{
			m_PerFrameConstantBuffer = std::make_unique<Buffer>(nullptr, perFrameBufferSize, 0, true, true);
			m_PerFrameConstantBuffer->CreateConstantBuffer();
		}


		if (perObjectBufferSize)
		{
			const int newRequireBufferCount = m_Components.size() - m_PerObjectConstantBuffers.size();
			if (m_Components.size() > m_PerObjectConstantBuffers.size())
			{
				for (int i = 0; i < newRequireBufferCount; i++)
				{
					auto buffer = std::make_unique<Buffer>(nullptr, perObjectBufferSize, 0, true, true);
					buffer->CreateConstantBuffer();
					m_PerObjectConstantBuffers.push_back(std::move(buffer));

					if (perDrawBufferSize)
					{
						for (int j = 0; j < perDrawBufferCount; j++)
						{
							auto buffer = std::make_unique<Buffer>(nullptr, perDrawBufferSize, 0, true, true);
							buffer->CreateConstantBuffer();
							m_PerDrawConstantBuffers.push_back(std::move(buffer));
						}
					}	
				}
			}
		}
		
	}

	void RenderPipeline::OnCreate()
	{
		Pipeline::OnCreate();

		auto device = Renderer::Instance().GetDevice();

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
		rtvFormat.NumRenderTargets = 2;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;

		pipelineStateStream.RTVFormats = rtvFormat;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };

		hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState));
		CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");
	}

	void RenderPipeline::PrepareRecord(const PipelineInputs* const pipelineInputs)
	{
		Pipeline::PrepareRecord(pipelineInputs);
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


			auto renderable = meshComponent->GetRenderable().lock();
			auto materials = meshComponent->GetMaterialsOld();
			for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
			{
				if (materials[j])
				{
					if (rootSignature.GetPerExecuteDescriptorCount())
					{
						commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerExecuteParameterIndex(), descriptorHeapHandle); // PerMaterial
						descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
							* rootSignature.GetPerExecuteDescriptorCount();
					}

				}
				renderable->Render(commandList, j);
			}
		}
	}

	void ComputePipeline::OnCreate()
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 10;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


		auto shader = Cast<Shader>(GetObjectOwner(Pipeline::PipelineOwnerType::Owner_Shader));
		auto& shaderDatablobs = shader->GetRaw();
		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc = {};
		computePipelineStateDesc.pRootSignature = shader->GetRootSignature().GetData();
		computePipelineStateDesc.CS = { shaderDatablobs[ShaderType::Compute]->GetBufferPointer(), shaderDatablobs[ShaderType::Compute]->GetBufferSize() };

		hr = device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&m_PipelineState));
		CS_FATAL(SUCCEEDED(hr), "PipelineState를 생성하는데 실패하였습니다.");
	}

	void ComputePipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto shader = Cast<Shader>(GetObjectOwner(Pipeline::PipelineOwnerType::Owner_Shader));

		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetComputeRootSignature(shader->GetRootSignature().GetData());
		ID3D12DescriptorHeap* irradianceSamplingHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(irradianceSamplingHeaps), irradianceSamplingHeaps);
		commandList->SetComputeRootDescriptorTable(0, m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		auto dispatchThreadGroupCounts = shader->GetDispatchThreadGroupCounts();
		commandList->Dispatch(dispatchThreadGroupCounts.x, dispatchThreadGroupCounts.y, dispatchThreadGroupCounts.z);
	}

}