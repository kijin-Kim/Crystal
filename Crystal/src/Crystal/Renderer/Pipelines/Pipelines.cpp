#include "cspch.h"
#include "Pipelines.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Resources/Buffer.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Resources/Material.h"

namespace Crystal {

	void Pipeline::RegisterPipelineComponents(std::weak_ptr<PrimitiveComponent> component)
	{
		m_Components.push_back(component);
	}

	void Pipeline::Begin()
	{
		/*CommandList를 녹화합니다*/
		//CS_DEBUG_INFO("Pipeline : %s Record", m_Name.c_str());
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

	const Shared<Scene>& Pipeline::GetScene()
	{
		auto renderSystem = Cast<RenderSystem>(GetOuter());
		auto level = Cast<Level>(renderSystem->GetOuter());
		return level->GetScene();
	}

	void RenderPipeline::OnCreate()
	{
		Pipeline::OnCreate();

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


		pipelineStateStream.RootSignature = m_Shader->GetRootSignature().GetData();
		auto inputLayout = m_Shader->GetInputLayout();
		pipelineStateStream.InputLayout = {inputLayout.GetData(), inputLayout.GetCount()};
		pipelineStateStream.PrimitiveTopology = m_Shader->GetPrimitiveTopologyType();


		auto& shaderDatablobs = m_Shader->GetRaw();

		if (m_Shader->HasShader(ShaderType::Vertex))
		{
			pipelineStateStream.VS = {
				shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Vertex]->GetBufferSize()
			};
		}

		if (m_Shader->HasShader(ShaderType::Hull))
		{
			pipelineStateStream.HS = {
				shaderDatablobs[ShaderType::Hull]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Hull]->GetBufferSize()
			};
		}

		if (m_Shader->HasShader(ShaderType::Domain))
		{
			pipelineStateStream.DS = {
				shaderDatablobs[ShaderType::Domain]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Domain]->GetBufferSize()
			};
		}

		if (m_Shader->HasShader(ShaderType::Geometry))
		{
			pipelineStateStream.DS = {
				shaderDatablobs[ShaderType::Geometry]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Geometry]->GetBufferSize()
			};
		}

		if (m_Shader->HasShader(ShaderType::Pixel))
		{
			pipelineStateStream.PS = {
				shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(),
				shaderDatablobs[ShaderType::Pixel]->GetBufferSize()
			};
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
		rtvFormat.NumRenderTargets = 6;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[1] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[2] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[3] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[4] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvFormat.RTFormats[5] = DXGI_FORMAT_R16G16B16A16_FLOAT;

		pipelineStateStream.RTVFormats = rtvFormat;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {sizeof(pipelineStateStream), &pipelineStateStream};

		hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState));
		CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");
	}

	void RenderPipeline::Begin()
	{
		Pipeline::Begin();
	}

	void RenderPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		auto device = Device::Instance().GetD3DDevice();
		auto rootSignature = m_Shader->GetRootSignature();


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(rootSignature.GetData());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = {m_DescriptorHeap.Get()};
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);

		D3D12_GPU_DESCRIPTOR_HANDLE descriptorHeapHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();

		int rootParameterIndex = 0;

		if (rootSignature.HasPerFrameParameter())
		{
			commandList->
				SetGraphicsRootDescriptorTable(rootSignature.GetPerFrameParameterIndex(), descriptorHeapHandle);
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
				commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerObjectParameterIndex(),
				                                            descriptorHeapHandle); // PerObject
				descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
					* rootSignature.GetPerObjectDescriptorCount();
			}


			auto renderable = meshComponent->GetRenderable();
			auto& materials = meshComponent->GetMaterials();

			if (!renderable)
			{
				continue;
			}

			for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
			{
				if (j < materials.size() && materials[j])
				{
					if (rootSignature.GetPerExecuteDescriptorCount())
					{
						commandList->SetGraphicsRootDescriptorTable(rootSignature.GetPerExecuteParameterIndex(),
						                                            descriptorHeapHandle); // PerMaterial
						descriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(
								D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
							* rootSignature.GetPerExecuteDescriptorCount();
					}
				}
				renderable->Render(commandList, j);
			}
		}
	}

	void ComputePipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 10;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


		auto& shaderDatablobs = m_Shader->GetRaw();
		D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStateDesc = {};
		computePipelineStateDesc.pRootSignature = m_Shader->GetRootSignature().GetData();
		computePipelineStateDesc.CS = {
			shaderDatablobs[ShaderType::Compute]->GetBufferPointer(),
			shaderDatablobs[ShaderType::Compute]->GetBufferSize()
		};

		hr = device->CreateComputePipelineState(&computePipelineStateDesc, IID_PPV_ARGS(&m_PipelineState));
		CS_FATAL(SUCCEEDED(hr), "PipelineState를 생성하는데 실패하였습니다.");
	}

	void ComputePipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);


		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetComputeRootSignature(m_Shader->GetRootSignature().GetData());
		ID3D12DescriptorHeap* irradianceSamplingHeaps[] = {m_DescriptorHeap.Get()};
		commandList->SetDescriptorHeaps(_countof(irradianceSamplingHeaps), irradianceSamplingHeaps);
		commandList->SetComputeRootDescriptorTable(0, m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		auto dispatchThreadGroupCounts = m_Shader->GetDispatchThreadGroupCounts();
		commandList->Dispatch(dispatchThreadGroupCounts.x, dispatchThreadGroupCounts.y, dispatchThreadGroupCounts.z);
	}

}
