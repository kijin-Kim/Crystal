#include "cspch.h"
#include "LightingPipeline.h"
#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Resources/ResourceManager.h"

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
			CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");

			CD3DX12_ROOT_PARAMETER1 rootParameter[4] = {}; // per frame
			rootParameter[0].InitAsConstantBufferView(0);

			CD3DX12_DESCRIPTOR_RANGE1 perFrameRootParameterRanges[1] = {}; // irradiance
			perFrameRootParameterRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			rootParameter[1].InitAsDescriptorTable(_countof(perFrameRootParameterRanges), perFrameRootParameterRanges);

			CD3DX12_DESCRIPTOR_RANGE1 perObjectRootParamRanges[1] = {};
			perObjectRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
			rootParameter[2].InitAsDescriptorTable(_countof(perObjectRootParamRanges), perObjectRootParamRanges);

			CD3DX12_DESCRIPTOR_RANGE1 perMeshRootParameterRanges[2] = {};
			perMeshRootParameterRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
			perMeshRootParameterRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // VOLATILE로 설정해야 모든 Descriptor가 없어도 렌더링을 진행할 수 있음.
			rootParameter[3].InitAsDescriptorTable(_countof(perMeshRootParameterRanges), perMeshRootParameterRanges);

			CD3DX12_STATIC_SAMPLER_DESC StaticSamplerDescs[1] = {};
			StaticSamplerDescs[0].Init(0);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootsigDesc;
			rootsigDesc.Init_1_1(_countof(rootParameter), rootParameter, _countof(StaticSamplerDescs), StaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
			hr = D3D12SerializeVersionedRootSignature(&rootsigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
			CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다 %s", rootSignatureErrorBlob->GetBufferPointer());
			hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_StaticMeshRootSignature));
			CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");


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
				CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
				CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			} pipelineStateStream;

			////////////////////////////////////////////////////
			////////PIPELINE STATE//////////////////////////////
			////////////////////////////////////////////////////

			pipelineStateStream.RootSignature = m_StaticMeshRootSignature.Get();
			pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
			pipelineStateStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


			auto& resourceManager = ResourceManager::Instance();
			auto& shaderDatablobs = resourceManager.GetShader("PBRShader_Static")->GetRaw();
			pipelineStateStream.VS = { shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(), 
				shaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
			pipelineStateStream.PS = { shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(), 
				shaderDatablobs[ShaderType::Pixel]->GetBufferSize() };


			pipelineStateStream.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			D3D12_RT_FORMAT_ARRAY rtvFormat = {};
			rtvFormat.NumRenderTargets = 1;
			rtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			pipelineStateStream.RTVFormats = rtvFormat;

			D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };

			hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_StaticMeshPipelineState));
			CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");


			// 임시로 10개의 오브젝트 까지 받을 수 있게함.
			const int maxObjectCount = 10;
			const int maxMaterialCount = 5;

			m_StaticMeshPerMaterialConstantBufferLists.resize(maxObjectCount);
			for (int i = 0; i < maxObjectCount; i++)
			{
				m_StaticMeshPerObjectConstantBuffers.push_back(std::make_unique<ConstantBuffer>((int)sizeof(StaticMeshPerObjectData)));
				for (int j = 0; j < maxMaterialCount; j++)
				{
					m_StaticMeshPerMaterialConstantBufferLists[i][j] 
						= std::make_unique<ConstantBuffer>((int)sizeof(StaticMeshPerObjectData));
				}
			}
		}




		// Create SkeletalMesh's PipelineState
		{
			D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
			descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			descriptorHeapDesc.NumDescriptors = 100;
			descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			descriptorHeapDesc.NodeMask = 0;

			HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_SkeletalMeshDescriptorHeap));
			CS_FATAL(SUCCEEDED(hr), "CBV_SRV힙을 생성하는데 실패하였습니다.");


			CD3DX12_ROOT_PARAMETER1 rootParameter[4] = {}; // per frame
			rootParameter[0].InitAsConstantBufferView(0);

			CD3DX12_DESCRIPTOR_RANGE1 globalRootParamRanges[1] = {}; // irradiance
			globalRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			rootParameter[1].InitAsDescriptorTable(_countof(globalRootParamRanges), globalRootParamRanges);

			CD3DX12_DESCRIPTOR_RANGE1 perObjectRootParamRanges[1] = {};
			perObjectRootParamRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
			rootParameter[2].InitAsDescriptorTable(_countof(perObjectRootParamRanges), perObjectRootParamRanges);

			CD3DX12_DESCRIPTOR_RANGE1 perMeshRootParameterRanges[2] = {};
			perMeshRootParameterRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
			perMeshRootParameterRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 1, 0, 
				D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE); // VOLATILE로 설정해야 모든 Descriptor가 없어도 렌더링을 진행할 수 있음.
			rootParameter[3].InitAsDescriptorTable(_countof(perMeshRootParameterRanges), perMeshRootParameterRanges);

			CD3DX12_STATIC_SAMPLER_DESC StaticSamplerDescs[1] = {};
			StaticSamplerDescs[0].Init(0);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootsigDesc;
			rootsigDesc.Init_1_1(_countof(rootParameter), rootParameter, _countof(StaticSamplerDescs), 
				StaticSamplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
			Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
			hr = D3D12SerializeVersionedRootSignature(&rootsigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
			CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다 %s", rootSignatureErrorBlob->GetBufferPointer());
			hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(), 
				rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_SkeletalMeshRootSignature));
			CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");


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
				CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
				CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			} pipelineStateStream;

			////////////////////////////////////////////////////
			////////PIPELINE STATE//////////////////////////////
			////////////////////////////////////////////////////

			pipelineStateStream.RootSignature = m_SkeletalMeshRootSignature.Get();
			pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
			pipelineStateStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


			auto& resourceManager = ResourceManager::Instance();
			auto& shaderDatablobs = resourceManager.GetShader("PBRShader_Skeletal")->GetRaw();
			pipelineStateStream.VS = { shaderDatablobs[ShaderType::Vertex]->GetBufferPointer(), 
				shaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
			pipelineStateStream.PS = { shaderDatablobs[ShaderType::Pixel]->GetBufferPointer(), 
				shaderDatablobs[ShaderType::Pixel]->GetBufferSize() };

			pipelineStateStream.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			D3D12_RT_FORMAT_ARRAY rtvFormat = {};
			rtvFormat.NumRenderTargets = 1;
			rtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			pipelineStateStream.RTVFormats = rtvFormat;

			D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };

			hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_SkeletalMeshPipelineState));
			CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");



			// 임시로 10개의 오브젝트 까지 받을 수 있게함.
			const int maxObjectCount = 10;
			const int maxMaterialCount = 5;

			m_SkeletalMeshPerMaterialConstantBufferLists.resize(maxObjectCount);
			for (int i = 0; i < maxObjectCount; i++)
			{
				m_SkeletalMeshPerObjectConstantBuffers.push_back(
					std::make_unique<ConstantBuffer>((int)sizeof(SkeletalMeshPerObjectData)));
				for (int j = 0; j < maxMaterialCount; j++)
				{
					m_SkeletalMeshPerMaterialConstantBufferLists[i][j] 
						= std::make_unique<ConstantBuffer>((int)sizeof(PerMaterialData));
				}
			}
		}

		m_PerFrameConstantBuffer = std::make_unique<ConstantBuffer>((int)sizeof(PerFrameData));
		m_PerMaterialConstantBuffer= std::make_unique<ConstantBuffer>((int)sizeof(PerMaterialData));

	}

	void LightingPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, 
		const PipelineInputs* const pipelineInputs)
	{

		RenderPipeline::Record(commandList, pipelineInputs);


		auto device = Renderer::Instance().GetDevice();


		LightingPipelineInputs* lightPipelineInputs = (LightingPipelineInputs*)pipelineInputs;

		m_PerFrameData.ViewProjection = Matrix4x4::Transpose(lightPipelineInputs->Camera->GetViewProjection());
		auto camPos = lightPipelineInputs->Camera->GetWorldPosition();
		m_PerFrameData.CameraPositionInWorld = DirectX::XMFLOAT4(camPos.x, camPos.y, camPos.z, 0.0f);
		m_PerFrameData.LightPositionInWorld[0] = DirectX::XMFLOAT4(20000.0f, 20000.0f, 0.0f, 0.0f);
		m_PerFrameData.LightPositionInWorld[1] = DirectX::XMFLOAT4(-20000.0f, 20000.0f, 0.0f, 0.0f);
		m_PerFrameConstantBuffer->SetData((void*)&m_PerFrameData);





		auto& skeletalMeshComponents = *lightPipelineInputs->SkeletalMeshComponents;
		auto& staticMeshComponents = *lightPipelineInputs->StaticMeshComponents;

		
		D3D12_CPU_DESCRIPTOR_HANDLE destHeapHandle = m_StaticMeshDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		
		D3D12_CPU_DESCRIPTOR_HANDLE irradianceTextureHandle = lightPipelineInputs->IrradiancemapTexture->GetShaderResourceView(); // Per Frame
		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);




		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/
		for (int i = 0; i < staticMeshComponents.size(); i++) // PerObject
		{
			
			StaticMeshPerObjectData staticMeshPerObjectData = {};

			staticMeshPerObjectData.World = Matrix4x4::Transpose(staticMeshComponents[i]->GetWorldTransform());
			m_StaticMeshPerObjectConstantBuffers[i]->SetData((void*)&staticMeshPerObjectData);

			D3D12_CPU_DESCRIPTOR_HANDLE perObjectConstantBufferHandle = m_StaticMeshPerObjectConstantBuffers[i]->GetCPUDescriptorHandle();
			device->CopyDescriptorsSimple(1, destHeapHandle, perObjectConstantBufferHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			


			
			auto staticMesh = std::static_pointer_cast<StaticMesh>(staticMeshComponents[i]->GetRenderable());

			auto materials = staticMeshComponents[i]->GetMaterials();
			for (int j = 0; j < staticMesh->GetVertexbufferCount(); j++)
			{
				if (!materials[j])
					break;

				D3D12_CPU_DESCRIPTOR_HANDLE albedoTextureHandle = {}; // PerMaterial
				D3D12_CPU_DESCRIPTOR_HANDLE metallicTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE roughnessTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE normalTextureHandle = {};

				PerMaterialData perMaterialData = {};


				if (materials[j]->HasTextureInput("AlbedoTexture"))
				{
					perMaterialData.bToggleAlbedoTexture = true;
					albedoTextureHandle = materials[j]->GetTextureInput("AlbedoTexture")->GetShaderResourceView();
				}
				else if (materials[j]->HasFloatInput("AlbedoColor"))
				{
					perMaterialData.bToggleAlbedoTexture = false;
					const DirectX::XMFLOAT4& floatInput = materials[j]->GetFloatInput("AlbedoColor");
					perMaterialData.AlbedoColor = { floatInput.x, floatInput.y, floatInput.z };
				}

				if (materials[j]->HasTextureInput("MetallicTexture"))
				{
					perMaterialData.bToggleMetallicTexture = true;
					metallicTextureHandle = materials[j]->GetTextureInput("MetallicTexture")->GetShaderResourceView();
				}
				else if (materials[j]->HasFloatInput("MetallicConstant"))
				{
					perMaterialData.bToggleMetallicTexture = false;
					perMaterialData.MetallicConstant = materials[j]->GetFloatInput("MetallicConstant").x;
				}

				if (materials[j]->HasTextureInput("RoughnessTexture"))
				{
					perMaterialData.bToggleRoughnessTexture = true;
					roughnessTextureHandle = materials[j]->GetTextureInput("RoughnessTexture")->GetShaderResourceView();
				}
				else if (materials[j]->HasFloatInput("RoughnessConstant"))
				{
					perMaterialData.bToggleRoughnessTexture = false;
					perMaterialData.RoughnessConstant = materials[j]->GetFloatInput("RoughnessConstant").x;
				}

				if (materials[j]->HasTextureInput("NormalTexture"))
				{
					perMaterialData.bToggleNormalTexture = true;
					normalTextureHandle = materials[j]->GetTextureInput("NormalTexture")->GetShaderResourceView();
				}
				else
					perMaterialData.bToggleNormalTexture = false;

				perMaterialData.bToggleIrradianceTexture = true;

				m_StaticMeshPerMaterialConstantBufferLists[i][j]->SetData((void*)&perMaterialData);
				device->CopyDescriptorsSimple(1, destHeapHandle, m_StaticMeshPerMaterialConstantBufferLists[i][j]->GetCPUDescriptorHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

				if(albedoTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, albedoTextureHandle,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if(metallicTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, metallicTextureHandle,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if(roughnessTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, roughnessTextureHandle,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if(normalTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, normalTextureHandle,D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


			}
			

		}


		destHeapHandle = m_SkeletalMeshDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(1, destHeapHandle, irradianceTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		/*메터리얼을 Shader Visible Descriptor Heap에 복사합니다.*/
		for (int i = 0; i < skeletalMeshComponents.size(); i++)
		{
			SkeletalMeshPerObjectData skeletalMeshPerObjectData = {};

			auto skeletalMesh = std::static_pointer_cast<SkeletalMesh>(skeletalMeshComponents[i]->GetRenderable());

			skeletalMeshPerObjectData.World = Matrix4x4::Transpose(skeletalMeshComponents[i]->GetWorldTransform());
			auto boneMatrices = skeletalMesh->GetBoneTransforms();
			std::copy(boneMatrices.begin(), boneMatrices.end(), skeletalMeshPerObjectData.Bones);			// TODO : 최적화 매우매우매우매우 비효율적
			m_SkeletalMeshPerObjectConstantBuffers[i]->SetData((void*)&skeletalMeshPerObjectData);


			D3D12_CPU_DESCRIPTOR_HANDLE perObjectConstantBufferHandle = m_SkeletalMeshPerObjectConstantBuffers[i]->GetCPUDescriptorHandle();
			device->CopyDescriptorsSimple(1, destHeapHandle, perObjectConstantBufferHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


			auto materials = skeletalMeshComponents[i]->GetMaterials();
			for (int j = 0; j < skeletalMesh->GetVertexbufferCount(); j++)
			{
				if (!materials[j])
					break;

				PerMaterialData perMaterialData = {};


				D3D12_CPU_DESCRIPTOR_HANDLE albedoTextureHandle = {}; // PerMaterial
				D3D12_CPU_DESCRIPTOR_HANDLE metallicTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE roughnessTextureHandle = {};
				D3D12_CPU_DESCRIPTOR_HANDLE normalTextureHandle = {};

				if (materials[j]->HasTextureInput("AlbedoTexture"))
				{
					perMaterialData.bToggleAlbedoTexture = true;
					albedoTextureHandle = materials[j]->GetTextureInput("AlbedoTexture")->GetShaderResourceView();
				}
				else if (materials[j]->HasFloatInput("AlbedoColor"))
				{
					perMaterialData.bToggleAlbedoTexture = false;
					const DirectX::XMFLOAT4& floatInput = materials[j]->GetFloatInput("AlbedoColor");
					perMaterialData.AlbedoColor = { floatInput.x, floatInput.y, floatInput.z };
				}

				if (materials[j]->HasTextureInput("MetallicTexture"))
				{
					perMaterialData.bToggleMetallicTexture = true;
					metallicTextureHandle = materials[j]->GetTextureInput("MetallicTexture")->GetShaderResourceView();
				}
				else if (materials[j]->HasFloatInput("MetallicConstant"))
				{
					perMaterialData.bToggleMetallicTexture = false;
					perMaterialData.MetallicConstant = materials[j]->GetFloatInput("MetallicConstant").x;
				}

				if (materials[j]->HasTextureInput("RoughnessTexture"))
				{
					perMaterialData.bToggleRoughnessTexture = true;
					roughnessTextureHandle = materials[j]->GetTextureInput("RoughnessTexture")->GetShaderResourceView();
				}
				else if (materials[j]->HasFloatInput("RoughnessConstant"))
				{
					perMaterialData.bToggleRoughnessTexture = false;
					perMaterialData.RoughnessConstant = materials[j]->GetFloatInput("RoughnessConstant").x;
				}

				if (materials[j]->HasTextureInput("NormalTexture"))
				{
					perMaterialData.bToggleNormalTexture = true;
					normalTextureHandle = materials[j]->GetTextureInput("NormalTexture")->GetShaderResourceView();
				}
				else
					perMaterialData.bToggleNormalTexture = false;

				perMaterialData.bToggleIrradianceTexture = true;

				m_StaticMeshPerMaterialConstantBufferLists[i][j]->SetData((void*)&perMaterialData);
				device->CopyDescriptorsSimple(1, destHeapHandle, m_StaticMeshPerMaterialConstantBufferLists[i][j]->GetCPUDescriptorHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


				if (albedoTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, albedoTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if (metallicTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, metallicTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if (roughnessTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, roughnessTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				if (normalTextureHandle.ptr)
					device->CopyDescriptorsSimple(1, destHeapHandle, normalTextureHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
				destHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


			}

			

		}


		commandList->SetPipelineState(m_StaticMeshPipelineState.Get());
		commandList->SetGraphicsRootSignature(m_StaticMeshRootSignature.Get());
		ID3D12DescriptorHeap* staticDescriptorHeaps[] = { m_StaticMeshDescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(staticDescriptorHeaps), staticDescriptorHeaps);
		commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress());


		D3D12_GPU_DESCRIPTOR_HANDLE staticMeshDescriptorHeapHandle = m_StaticMeshDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		commandList->SetGraphicsRootDescriptorTable(1, staticMeshDescriptorHeapHandle); // Irradiance
		staticMeshDescriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int i = 0; i < staticMeshComponents.size(); i++) 
		{
			commandList->SetGraphicsRootDescriptorTable(2, staticMeshDescriptorHeapHandle); // PerObject
			staticMeshDescriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			auto staticMesh = std::static_pointer_cast<StaticMesh>(staticMeshComponents[i]->GetRenderable());
			auto materials = staticMeshComponents[i]->GetMaterials();
			for (int j = 0; j < staticMesh->GetVertexbufferCount(); j++)
			{
				if (materials[j])
				{
					commandList->SetGraphicsRootDescriptorTable(3, staticMeshDescriptorHeapHandle); // PerMaterial
				}
				staticMeshDescriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;
				staticMesh->Render(commandList, j);
			}
		}

		




		commandList->SetPipelineState(m_SkeletalMeshPipelineState.Get());
		commandList->SetGraphicsRootSignature(m_SkeletalMeshRootSignature.Get());
		ID3D12DescriptorHeap* skeletalDescriptorHeaps[] = { m_SkeletalMeshDescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(skeletalDescriptorHeaps), skeletalDescriptorHeaps);
		commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress());
		
		D3D12_GPU_DESCRIPTOR_HANDLE skeletalMeshDescriptorHeapHandle = m_SkeletalMeshDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		commandList->SetGraphicsRootDescriptorTable(1, skeletalMeshDescriptorHeapHandle); // Irradiance
		skeletalMeshDescriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int i = 0; i < skeletalMeshComponents.size(); i++)
		{
			commandList->SetGraphicsRootDescriptorTable(2, skeletalMeshDescriptorHeapHandle); // PerObject
			skeletalMeshDescriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			auto skeletalMesh = std::static_pointer_cast<SkeletalMesh>(skeletalMeshComponents[i]->GetRenderable());
			auto materials = skeletalMeshComponents[i]->GetMaterials();
			for (int j = 0; j < skeletalMesh->GetVertexbufferCount(); j++)
			{
				if (materials[j])
				{
					commandList->SetGraphicsRootDescriptorTable(3, skeletalMeshDescriptorHeapHandle); // PerMaterial
				}
				skeletalMeshDescriptorHeapHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;
				skeletalMesh->Render(commandList, j);
			}
		}


	}

}