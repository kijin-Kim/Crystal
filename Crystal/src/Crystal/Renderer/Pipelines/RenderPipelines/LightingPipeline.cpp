#include "cspch.h"
#include "LightingPipeline.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	LightingPipeline::LightingPipeline(const std::string& name) : RenderPipeline(name)
	{
		auto device = Renderer::Instance().GetDevice();

		// Create StaticMesh's PipelineState
		{
			D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
			descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			descriptorHeapDesc.NumDescriptors = 100;
			descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			descriptorHeapDesc.NodeMask = 0;

			HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_StaticMeshDescriptorHeap));
			CS_FATAL(SUCCEEDED(hr), "CBV_SRV���� �����ϴµ� �����Ͽ����ϴ�.");

			CD3DX12_ROOT_PARAMETER1 rootParameter[3] = {}; // per frame
			rootParameter[0].InitAsConstantBufferView(0);

			CD3DX12_DESCRIPTOR_RANGE1 globalRootParamRanges[1] = {}; // irradiance
			globalRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			rootParameter[1].InitAsDescriptorTable(_countof(globalRootParamRanges), globalRootParamRanges);

			CD3DX12_DESCRIPTOR_RANGE1 perObjectRootParamRanges[2] = {};
			perObjectRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
			perObjectRootParamRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // VOLATILE�� �����ؾ� ��� Descriptor�� ��� �������� ������ �� ����.
			rootParameter[2].InitAsDescriptorTable(_countof(perObjectRootParamRanges), perObjectRootParamRanges);

			CD3DX12_STATIC_SAMPLER_DESC StaticSamplerDescs[1] = {};
			StaticSamplerDescs[0].Init(0);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootsigDesc;
			rootsigDesc.Init_1_1(_countof(rootParameter), rootParameter, _countof(StaticSamplerDescs), StaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
			hr = D3D12SerializeVersionedRootSignature(&rootsigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
			CS_FATAL(SUCCEEDED(hr), "Root Signature�� �ø���ȭ�ϴµ� �����Ͽ����ϴ� %s", rootSignatureErrorBlob->GetBufferPointer());
			hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_StaticMeshRootSignature));
			CS_FATAL(SUCCEEDED(hr), "Root Signature�� �����ϴµ� �����Ͽ����ϴ�");


			D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
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

			pipelineStateStream.RootSignature = m_StaticMeshRootSignature.Get();
			pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
			pipelineStateStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

			auto& shaderManager = ShaderManager::Instance();
			auto& shaderDatablobs = shaderManager.GetShader("PBRShader_Static")->GetRaw();
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

			hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_StaticMeshPipelineState));
			CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object�� �����ϴµ� �����Ͽ����ϴ�");


			// �ӽ÷� 10���� ������Ʈ ���� ���� �� �ְ���.
			for (int i = 0; i < 10; i++)
				m_StaticMeshPerObjectConstantBuffers.push_back(std::make_unique<ConstantBuffer>((int)sizeof(m_StaticMeshPerObjectData)));
		}




		// Create SkeletalMesh's PipelineState
		{
			D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
			descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			descriptorHeapDesc.NumDescriptors = 100;
			descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			descriptorHeapDesc.NodeMask = 0;

			HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_SkeletalMeshDescriptorHeap));
			CS_FATAL(SUCCEEDED(hr), "CBV_SRV���� �����ϴµ� �����Ͽ����ϴ�.");


			CD3DX12_ROOT_PARAMETER1 rootParameter[3] = {};
			rootParameter[0].InitAsConstantBufferView(0);

			CD3DX12_DESCRIPTOR_RANGE1 globalRootParamRanges[1] = {};
			globalRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			rootParameter[1].InitAsDescriptorTable(_countof(globalRootParamRanges), globalRootParamRanges);

			CD3DX12_DESCRIPTOR_RANGE1 perObjectRootParamRanges[2] = {};
			perObjectRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
			perObjectRootParamRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // VOLATILE�� �����ؾ� ��� Descriptor�� ��� �������� ������ �� ����.
			rootParameter[2].InitAsDescriptorTable(_countof(perObjectRootParamRanges), perObjectRootParamRanges);

			CD3DX12_STATIC_SAMPLER_DESC StaticSamplerDescs[1] = {};
			StaticSamplerDescs[0].Init(0);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootsigDesc;
			rootsigDesc.Init_1_1(_countof(rootParameter), rootParameter, _countof(StaticSamplerDescs), StaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
			hr = D3D12SerializeVersionedRootSignature(&rootsigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
			CS_FATAL(SUCCEEDED(hr), "Root Signature�� �ø���ȭ�ϴµ� �����Ͽ����ϴ� %s", rootSignatureErrorBlob->GetBufferPointer());
			hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_SkeletalMeshRootSignature));
			CS_FATAL(SUCCEEDED(hr), "Root Signature�� �����ϴµ� �����Ͽ����ϴ�");


			D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
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

			pipelineStateStream.RootSignature = m_SkeletalMeshRootSignature.Get();
			pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
			pipelineStateStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

			auto& shaderManager = ShaderManager::Instance();
			auto& shaderDatablobs = shaderManager.GetShader("PBRShader_Skeletal")->GetRaw();
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

			hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_SkeletalMeshPipelineState));
			CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object�� �����ϴµ� �����Ͽ����ϴ�");



			// �ӽ÷� 10���� ������Ʈ ���� ���� �� �ְ���.
			for (int i = 0; i < 10; i++)
				m_SkeletalMeshPerObjectConstantBuffers.push_back(std::make_unique<ConstantBuffer>((int)sizeof(m_SkeletalMeshPerObjectData)));

			m_PerFrameConstantBuffer = std::make_unique<ConstantBuffer>((int)sizeof(m_PerFrameData));
		}


	}

	void LightingPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs)
	{

		RenderPipeline::Record(commandList, pipelineInputs);


		auto device = Renderer::Instance().GetDevice();


		LightingPipelineInputs* lightPipelineInputs = (LightingPipelineInputs*)pipelineInputs;

		m_PerFrameData.ViewProjection = Matrix4x4::Transpose(lightPipelineInputs->Camera->GetViewProjection());
		auto camPos = lightPipelineInputs->Camera->GetWorldPosition();
		m_PerFrameData.CameraPositionInWorld = DirectX::XMFLOAT4(camPos.x, camPos.y, camPos.z, 0.0f);
		m_PerFrameData.LightPositionInWorld = DirectX::XMFLOAT4(1000.0f, 1000.0f, 0.0f, 0.0f);
		m_PerFrameConstantBuffer->SetData((void*)&m_PerFrameData);




		m_SkeletalMeshPerObjectData.AlbedoColor = {};


		auto skeletalMeshComponents = *lightPipelineInputs->SkeletalMeshComponents;
		auto staticMeshComponents = *lightPipelineInputs->StaticMeshComponents;

		


		// ���� ������ ������ Ÿ�� ������, �� �Ŀ� �� �н�(���̴�)���� �� ������ ������ �ְ� �� ����.

		D3D12_CPU_DESCRIPTOR_HANDLE pbrInputCpuHandles[PBR_INPUT_COUNT] = {};

		pbrInputCpuHandles[IrradianceTexture] = lightPipelineInputs->IrradiancemapTexture->GetShaderResourceView();
		device->CopyDescriptorsSimple(1, m_StaticMeshDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), pbrInputCpuHandles[IrradianceTexture],
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		device->CopyDescriptorsSimple(1, m_SkeletalMeshDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), pbrInputCpuHandles[IrradianceTexture],
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		/*���͸����� Shader Visible Descriptor Heap�� �����մϴ�.*/
		for (int i = 0; i < staticMeshComponents.size(); i++)
		{
			m_StaticMeshPerObjectData.World = Matrix4x4::Transpose(staticMeshComponents[i]->GetWorldTransform());
			m_StaticMeshPerObjectConstantBuffers[i]->SetData((void*)&m_StaticMeshPerObjectData);

			pbrInputCpuHandles[PerObjectConstants] = m_StaticMeshPerObjectConstantBuffers[i]->GetCPUDescriptorHandle();


			const StaticMesh* staticMesh = (StaticMesh*)staticMeshComponents[i]->GetRenderable();
			auto material = staticMesh->GetMaterial();

			if (material->HasTextureInput("AlbedoTexture"))
			{
				m_StaticMeshPerObjectData.bToggleAlbedoTexture = true;
				pbrInputCpuHandles[AlbedoTexture] = material->GetTextureInput("AlbedoTexture")->GetShaderResourceView();
			}
			else if (material->HasFloatInput("AlbedoColor"))
			{
				m_StaticMeshPerObjectData.bToggleAlbedoTexture = false;
				m_StaticMeshPerObjectData.AlbedoColor = material->GetFloatInput("AlbedoColor");
			}

			if (material->HasTextureInput("RoughnessTexture"))
			{
				m_StaticMeshPerObjectData.bToggleRoughnessTexture = true;
				pbrInputCpuHandles[RoughnessTexture] = material->GetTextureInput("RoughnessTexture")->GetShaderResourceView();
			}
			else if (material->HasFloatInput("RoughnessConstant"))
			{
				m_StaticMeshPerObjectData.bToggleRoughnessTexture = false;
				m_StaticMeshPerObjectData.RoughnessConstant = material->GetFloatInput("RoughnessConstant").x;
			}

			if (material->HasTextureInput("MetalicTexture"))
			{
				m_StaticMeshPerObjectData.bToggleMetalicTexture = true;
				pbrInputCpuHandles[MetalicTexture] = material->GetTextureInput("MetalicTexture")->GetShaderResourceView();
			}
			else if (material->HasFloatInput("MetalicConstant"))
			{
				m_StaticMeshPerObjectData.bToggleMetalicTexture = false;
				m_StaticMeshPerObjectData.MetalicConstant = material->GetFloatInput("MetalicConstant").x;
			}

			if (material->HasTextureInput("NormalTexture"))
			{
				m_StaticMeshPerObjectData.bToggleNormalTexture = true;
				pbrInputCpuHandles[NormalTexture] = material->GetTextureInput("NormalTexture")->GetShaderResourceView();
			}
			else
				m_StaticMeshPerObjectData.bToggleNormalTexture = false;

			m_StaticMeshPerObjectData.bToggleIrradianceTexture = true;

			D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_StaticMeshDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (i * 5);


			for (int j = PerObjectConstants; j < PBR_INPUT_COUNT; j++)
			{
				if (pbrInputCpuHandles[j].ptr)
				{
					device->CopyDescriptorsSimple(1, destHeapHandle, pbrInputCpuHandles[j],
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

					pbrInputCpuHandles[j] = {};
				}
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}

		}




		/*���͸����� Shader Visible Descriptor Heap�� �����մϴ�.*/
		for (int i = 0; i < skeletalMeshComponents.size(); i++)
		{
			SkeletalMesh* skeletalMesh = (SkeletalMesh*)skeletalMeshComponents[i]->GetRenderable();

			m_SkeletalMeshPerObjectData.World = Matrix4x4::Transpose(skeletalMeshComponents[i]->GetWorldTransform());
			auto boneMatrices = skeletalMesh->GetBoneTransforms();
			// TODO : ����ȭ �ſ�ſ�ſ�ſ� ��ȿ����
			std::copy(boneMatrices.begin(), boneMatrices.end(), m_SkeletalMeshPerObjectData.Bones);

			
			m_SkeletalMeshPerObjectConstantBuffers[i]->SetData((void*)&m_SkeletalMeshPerObjectData);


			pbrInputCpuHandles[PerObjectConstants] = m_SkeletalMeshPerObjectConstantBuffers[i]->GetCPUDescriptorHandle();


			auto material = skeletalMesh->GetMaterial();

			if (material->HasTextureInput("AlbedoTexture"))
			{
				m_SkeletalMeshPerObjectData.bToggleAlbedoTexture = true;
				pbrInputCpuHandles[AlbedoTexture] = material->GetTextureInput("AlbedoTexture")->GetShaderResourceView();
			}
			else if (material->HasFloatInput("AlbedoColor"))
			{
				m_SkeletalMeshPerObjectData.bToggleAlbedoTexture = false;
				m_SkeletalMeshPerObjectData.AlbedoColor = material->GetFloatInput("AlbedoColor");
			}

			if (material->HasTextureInput("RoughnessTexture"))
			{
				m_SkeletalMeshPerObjectData.bToggleRoughnessTexture = true;
				pbrInputCpuHandles[RoughnessTexture] = material->GetTextureInput("RoughnessTexture")->GetShaderResourceView();
			}
			else if (material->HasFloatInput("RoughnessConstant"))
			{
				m_SkeletalMeshPerObjectData.bToggleRoughnessTexture = false;
				m_SkeletalMeshPerObjectData.RoughnessConstant = material->GetFloatInput("RoughnessConstant").x;
			}

			if (material->HasTextureInput("MetalicTexture"))
			{
				m_SkeletalMeshPerObjectData.bToggleMetalicTexture = true;
				pbrInputCpuHandles[MetalicTexture] = material->GetTextureInput("MetalicTexture")->GetShaderResourceView();
			}
			else if (material->HasFloatInput("MetalicConstant"))
			{
				m_SkeletalMeshPerObjectData.bToggleMetalicTexture = false;
				m_SkeletalMeshPerObjectData.MetalicConstant = material->GetFloatInput("MetalicConstant").x;
			}

			if (material->HasTextureInput("NormalTexture"))
			{
				m_SkeletalMeshPerObjectData.bToggleNormalTexture = true;
				pbrInputCpuHandles[NormalTexture] = material->GetTextureInput("NormalTexture")->GetShaderResourceView();
			}
			else
				m_SkeletalMeshPerObjectData.bToggleNormalTexture = false;

			m_SkeletalMeshPerObjectData.bToggleIrradianceTexture = true;

			D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_SkeletalMeshDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (i * 5);


			for (int j = PerObjectConstants; j < PBR_INPUT_COUNT; j++)
			{
				if (pbrInputCpuHandles[j].ptr)
				{
					device->CopyDescriptorsSimple(1, destHeapHandle, pbrInputCpuHandles[j],
						D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

					pbrInputCpuHandles[j] = {};
				}
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}

		}


		commandList->SetPipelineState(m_StaticMeshPipelineState.Get());
		commandList->SetGraphicsRootSignature(m_StaticMeshRootSignature.Get());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = { m_StaticMeshDescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);
		commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(1, m_StaticMeshDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		for (int i = 0; i < staticMeshComponents.size(); i++)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle = m_StaticMeshDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (i * 5);
			commandList->SetGraphicsRootDescriptorTable(2, handle);

			StaticMesh* staticMesh = (StaticMesh*)staticMeshComponents[i]->GetRenderable();
			staticMesh->Render(commandList);
		}

		




		commandList->SetPipelineState(m_SkeletalMeshPipelineState.Get());
		commandList->SetGraphicsRootSignature(m_SkeletalMeshRootSignature.Get());
		ID3D12DescriptorHeap* skeletalDescriptorHeaps[] = { m_SkeletalMeshDescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(skeletalDescriptorHeaps), skeletalDescriptorHeaps);
		commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(1, m_SkeletalMeshDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		for (int i = 0; i < skeletalMeshComponents.size(); i++)
		{
			D3D12_GPU_DESCRIPTOR_HANDLE handle = m_SkeletalMeshDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
			handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			handle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * (i * 5);
			commandList->SetGraphicsRootDescriptorTable(2, handle);

			SkeletalMesh* skeletalMesh = (SkeletalMesh*)skeletalMeshComponents[i]->GetRenderable();
			skeletalMesh->Render(commandList);
		}


	}

}