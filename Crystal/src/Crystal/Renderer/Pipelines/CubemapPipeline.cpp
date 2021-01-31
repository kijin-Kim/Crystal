#include "cspch.h"
#include "CubemapPipeline.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ConstantBuffer.h"

namespace Crystal {

	CubemapPipeline::CubemapPipeline(const std::string& name) : Pipeline(name)
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 100;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");

		CD3DX12_DESCRIPTOR_RANGE1 cubemapCommonDescriptorHeapRanges[1] = {};
		cubemapCommonDescriptorHeapRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER1 cubemapRootParameter[2] = {};
		cubemapRootParameter[0].InitAsConstantBufferView(0);
		cubemapRootParameter[1].InitAsDescriptorTable(_countof(cubemapCommonDescriptorHeapRanges), cubemapCommonDescriptorHeapRanges);

		CD3DX12_STATIC_SAMPLER_DESC cubemapStaticSamplerDescs[1] = {};
		cubemapStaticSamplerDescs[0].Init(0);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC cubemapRootsigDesc(_countof(cubemapRootParameter), cubemapRootParameter, _countof(cubemapStaticSamplerDescs), cubemapStaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		hr = D3D12SerializeVersionedRootSignature(&cubemapRootsigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");

		D3D12_INPUT_ELEMENT_DESC cubemapInputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		////////////////////////////////////////////////////
		////////PIPELINE STATE//////////////////////////////
		////////////////////////////////////////////////////


		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopology;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DSV;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} cubemapPipelineStream;


		cubemapPipelineStream.RootSignature = m_RootSignature.Get();
		cubemapPipelineStream.InputLayout = { cubemapInputLayout, _countof(cubemapInputLayout) };
		cubemapPipelineStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		auto& shaderManager = ShaderManager::Instance();
		auto& cubemapShaderDatablobs = shaderManager.GetShader("CubemapShader")->GetRaw();
		cubemapPipelineStream.VS = { cubemapShaderDatablobs[ShaderType::Vertex]->GetBufferPointer(), cubemapShaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
		cubemapPipelineStream.PS = { cubemapShaderDatablobs[ShaderType::Pixel]->GetBufferPointer(), cubemapShaderDatablobs[ShaderType::Pixel]->GetBufferSize() };

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

		cubemapPipelineStream.DSV = CD3DX12_DEPTH_STENCIL_DESC(depthStencilDesc);
		cubemapPipelineStream.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		D3D12_RT_FORMAT_ARRAY cubemapRtvFormat = {};
		cubemapRtvFormat.NumRenderTargets = 1;
		cubemapRtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		cubemapPipelineStream.RTVFormats = cubemapRtvFormat;
		D3D12_PIPELINE_STATE_STREAM_DESC cubemapPipelineStreamDesc = { sizeof(cubemapPipelineStream), &cubemapPipelineStream };

		hr = device->CreatePipelineState(&cubemapPipelineStreamDesc, IID_PPV_ARGS(&m_PipelineState));
		CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");

		float quadVertices[] = {
			-1.0f, -1.0f,
			-1.0f, +1.0f,
			+1.0f, +1.0f,
			+1.0f, -1.0f
		};

		uint32_t quadIndices[] = {
			0, 1, 2,
			0, 2, 3
		};

		m_QuadVertexBuffer = std::make_unique<VertexBuffer>((void*)quadVertices, (UINT)(sizeof(float) * 2), (UINT)_countof(quadVertices));
		m_QuadIndexBuffer = std::make_unique<IndexBuffer>((void*)quadIndices, (UINT)(sizeof(uint32_t) * _countof(quadIndices)), (UINT)(_countof(quadIndices)));

		m_PerFrameConstantBuffer = std::make_unique<ConstantBuffer>((int)sizeof(m_PerFrameData));

	}

	void CubemapPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		CubemapPipelineInputs* cubemapPipelineInputs = (CubemapPipelineInputs*)m_PipelineInputs;


		auto viewProj = cubemapPipelineInputs->Camera->GetViewProjection();
		viewProj._41 = 0.0f; viewProj._42 = 0.0f; viewProj._43 = 0.0f;
		m_PerFrameData.InverseViewProjection = Matrix4x4::Transpose(Matrix4x4::Inverse(viewProj));
		m_PerFrameConstantBuffer->SetData((void*)&m_PerFrameData);

		D3D12_CPU_DESCRIPTOR_HANDLE cubemapDescriptorInputs[CUBEMAP_INPUT_COUNT] = {};
		cubemapDescriptorInputs[CubemapTexture] = cubemapPipelineInputs->CubemapTexture->GetShaderResourceView();


		auto device = Renderer::Instance().GetDevice();
		for (int i = 0; i < CUBEMAP_INPUT_COUNT; i++)
		{
			device->CopyDescriptorsSimple(1, m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), cubemapDescriptorInputs[i],
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}


		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->RSSetViewports(1, &cubemapPipelineInputs->Camera->GetViewport());
		commandList->RSSetScissorRects(1, &cubemapPipelineInputs->Camera->GetScissorRect());
		commandList->OMSetRenderTargets(1, &cubemapPipelineInputs->RenderTargets[0]->GetRenderTargetView(), TRUE, &cubemapPipelineInputs->DepthStencilBuffer->GetDepthStencilView());

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(1, m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		commandList->IASetVertexBuffers(0, 1, &m_QuadVertexBuffer->GetView());
		commandList->IASetIndexBuffer(&m_QuadIndexBuffer->GetView());
		commandList->DrawIndexedInstanced(m_QuadIndexBuffer->GetCount(), 1, 0, 0, 0);
	}

}