#include "cspch.h"
#include "LightingPipeline.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	LightingPipeline::LightingPipeline(const std::string& name) : Pipeline(name)
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 100;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


		CD3DX12_ROOT_PARAMETER1 rootParameter[3] = {};
		rootParameter[0].InitAsConstantBufferView(0);

		CD3DX12_DESCRIPTOR_RANGE1 globalRootParamRanges[1] = {};
		globalRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
		rootParameter[1].InitAsDescriptorTable(_countof(globalRootParamRanges), globalRootParamRanges);

		CD3DX12_DESCRIPTOR_RANGE1 perObjectRootParamRanges[2] = {};
		perObjectRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
		perObjectRootParamRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // VOLATILE로 설정해야 모든 Descriptor가 없어도 렌더링을 진행할 수 있음.
		rootParameter[2].InitAsDescriptorTable(_countof(perObjectRootParamRanges), perObjectRootParamRanges);

		CD3DX12_STATIC_SAMPLER_DESC StaticSamplerDescs[1] = {};
		StaticSamplerDescs[0].Init(0);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootsigDesc;
		rootsigDesc.Init_1_1(_countof(rootParameter), rootParameter, _countof(StaticSamplerDescs), StaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		hr = D3D12SerializeVersionedRootSignature(&rootsigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다 %s", rootSignatureErrorBlob->GetBufferPointer());
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_AnimatedRootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");


		D3D12_INPUT_ELEMENT_DESC animatedInputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 56, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

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
		} pipelineStateStream;

		////////////////////////////////////////////////////
		////////PIPELINE STATE//////////////////////////////
		////////////////////////////////////////////////////

		pipelineStateStream.RootSignature = m_AnimatedRootSignature.Get();
		pipelineStateStream.InputLayout = { animatedInputLayout, _countof(animatedInputLayout) };
		pipelineStateStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		auto& shaderManager = ShaderManager::Instance();
		auto& shaderDatablobs = shaderManager.GetShader("PBRShader")->GetRaw();
		pipelineStateStream.VS = { shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(), shaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
		pipelineStateStream.PS = { shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(), shaderDatablobs[ShaderType::Pixel]->GetBufferSize() };

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

		pipelineStateStream.DSV = CD3DX12_DEPTH_STENCIL_DESC(depthStencilDesc);
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineStateStream.RTVFormats = rtvFormat;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };

		hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_AnimatedPipelineState));
		CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");


		for (int i = 0; i < 10; i++)
			m_PerObjectConstantBuffers.push_back(std::make_unique<ConstantBuffer>((int)sizeof(m_PerObjectData)));
		m_PerFrameConstantBuffer = std::make_unique<ConstantBuffer>((int)sizeof(m_PerFrameData));

	}

	void LightingPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{

		Pipeline::Record(commandList);


		auto device = Renderer::Instance().GetDevice();


		LightingPipelineInputs* lightPipelineInputs = (LightingPipelineInputs*)m_PipelineInputs;

		m_PerFrameData.ViewProjection = Matrix4x4::Transpose(lightPipelineInputs->Camera->GetViewProjection());
		auto camPos = lightPipelineInputs->Camera->GetWorldPosition();
		m_PerFrameData.CameraPositionInWorld = DirectX::XMFLOAT4(camPos.x, camPos.y, camPos.z, 0.0f);
		m_PerFrameData.LightPositionInWorld = DirectX::XMFLOAT4(1000.0f, 1000.0f, 0.0f, 0.0f);
		m_PerObjectData.AlbedoColor = {};


		auto renderComponent = *lightPipelineInputs->RenderComponents;

		auto boneMatrices = ((Mesh*)(renderComponent[0]->GetRenderables()[0]))->GetBoneTransforms();
		// TODO : 최적화 매우매우매우매우 비효율적
		std::copy(boneMatrices.begin(), boneMatrices.end(), m_PerObjectData.Bones);


		// 아주 간단한 프로토 타입 디자인, 추 후에 매 패스(셰이더)마다 이 정보를 가지고 있게 할 것임.

		D3D12_CPU_DESCRIPTOR_HANDLE pbrInputCpuHandles[PBR_INPUT_COUNT] = {};

		pbrInputCpuHandles[IrradianceTexture] = lightPipelineInputs->IrradiancemapTexture->GetShaderResourceView();
		device->CopyDescriptorsSimple(1, m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), pbrInputCpuHandles[IrradianceTexture],
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/
		for (int i = 0; i < renderComponent.size(); i++)
		{

			m_PerObjectData.World = Matrix4x4::Transpose(renderComponent[i]->GetWorldTransform());
			m_PerObjectConstantBuffers[i]->SetData((void*)&m_PerObjectData);


			pbrInputCpuHandles[PerObjectConstants] = m_PerObjectConstantBuffers[i]->GetCPUDescriptorHandle();


			auto material = ((Mesh*)(renderComponent[i]->GetRenderables()[0]))->GetMaterial();

			if (material->HasTextureInput("AlbedoTexture"))
			{
				m_PerObjectData.bToggleAlbedoTexture = true;
				pbrInputCpuHandles[AlbedoTexture] = material->GetTextureInput("AlbedoTexture")->GetShaderResourceView();
			}
			else if (material->HasFloatInput("AlbedoColor"))
			{
				m_PerObjectData.bToggleAlbedoTexture = false;
				m_PerObjectData.AlbedoColor = material->GetFloatInput("AlbedoColor");
			}

			if (material->HasTextureInput("RoughnessTexture"))
			{
				m_PerObjectData.bToggleRoughnessTexture = true;
				pbrInputCpuHandles[RoughnessTexture] = material->GetTextureInput("RoughnessTexture")->GetShaderResourceView();
			}
			else if (material->HasFloatInput("RoughnessConstant"))
			{
				m_PerObjectData.bToggleRoughnessTexture = false;
				m_PerObjectData.RoughnessConstant = material->GetFloatInput("RoughnessConstant").x;
			}

			if (material->HasTextureInput("MetalicTexture"))
			{
				m_PerObjectData.bToggleMetalicTexture = true;
				pbrInputCpuHandles[MetalicTexture] = material->GetTextureInput("MetalicTexture")->GetShaderResourceView();
			}
			else if (material->HasFloatInput("MetalicConstant"))
			{
				m_PerObjectData.bToggleMetalicTexture = false;
				m_PerObjectData.MetalicConstant = material->GetFloatInput("MetalicConstant").x;
			}

			if (material->HasTextureInput("NormalTexture"))
			{
				m_PerObjectData.bToggleNormalTexture = true;
				pbrInputCpuHandles[NormalTexture] = material->GetTextureInput("NormalTexture")->GetShaderResourceView();
			}
			else
				m_PerObjectData.bToggleNormalTexture = false;

			D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (i * 5);


			for (int j = PerObjectConstants; j < 5; j++)
			{
				if (pbrInputCpuHandles[j].ptr)
				{
					device->CopyDescriptorsSimple(1, destHeapHandle, pbrInputCpuHandles[j],
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				}
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}

		}

		m_PerFrameConstantBuffer->SetData((void*)&m_PerFrameData);



		// CommandList를 Record 합니다.

		commandList->IASetPrimitiveTopology(lightPipelineInputs->PrimitiveTopology);
		commandList->RSSetViewports(1, &lightPipelineInputs->Camera->GetViewport());
		commandList->RSSetScissorRects(1, &lightPipelineInputs->Camera->GetScissorRect());
		commandList->OMSetRenderTargets(1, &lightPipelineInputs->RenderTargets[0]->GetRenderTargetView(), TRUE, &lightPipelineInputs->DepthStencilBuffer->GetDepthStencilView());

		
		commandList->SetPipelineState(m_AnimatedPipelineState.Get());
		commandList->SetGraphicsRootSignature(m_AnimatedRootSignature.Get());

		commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress());
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		commandList->SetGraphicsRootDescriptorTable(1, m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		for (int i = 0; i < renderComponent.size(); i++)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (i * 5);
			commandList->SetGraphicsRootDescriptorTable(2, handle);
			((Mesh*)renderComponent[i]->GetRenderables()[0])->Render(commandList);
		}


	}

}