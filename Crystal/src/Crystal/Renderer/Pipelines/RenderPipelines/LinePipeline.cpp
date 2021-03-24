#include "cspch.h"
#include "LinePipeline.h"
#include "Crystal\Renderer\Renderer.h"

namespace Crystal {

	LinePipeline::LinePipeline(const std::string& name) : RenderPipeline(name)
	{
		auto device = Renderer::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.NumDescriptors = 100;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;

		HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		CS_FATAL(SUCCEEDED(hr), "Descriptor 힙을 생성하는데 실패하였습니다.");

		

		CD3DX12_ROOT_PARAMETER1 rootParameter[2] = {};
		rootParameter[0].InitAsConstantBufferView(0);

		CD3DX12_DESCRIPTOR_RANGE1 commonDescriptorHeapRanges[1] = {};
		commonDescriptorHeapRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
		rootParameter[1].InitAsDescriptorTable(_countof(commonDescriptorHeapRanges),
			commonDescriptorHeapRanges);

	
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Init_1_1(_countof(rootParameter), rootParameter, 0, nullptr, 
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		hr = D3D12SerializeVersionedRootSignature(&rootSigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(),
			rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");

		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
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
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;


		pipelineStateStream.RootSignature = m_RootSignature.Get();
		pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
		pipelineStateStream.PrimitiveTopology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		

		auto& shaderManager = ShaderManager::Instance();
		auto& ShaderDatablobs = shaderManager.GetShader("SimpleColorShader")->GetRaw();
		pipelineStateStream.VS = { ShaderDatablobs[ShaderType::Vertex]->GetBufferPointer(),
			ShaderDatablobs[ShaderType::Vertex]->GetBufferSize() };
		pipelineStateStream.PS = { ShaderDatablobs[ShaderType::Pixel]->GetBufferPointer(),
			ShaderDatablobs[ShaderType::Pixel]->GetBufferSize() };

		
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		D3D12_RT_FORMAT_ARRAY RtvFormat = {};
		RtvFormat.NumRenderTargets = 1;
		RtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineStateStream.RTVFormats = RtvFormat;
		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(pipelineStateStream), &pipelineStateStream };

		hr = device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState));
		CS_FATAL(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");


		m_PerFrameConstantBuffer = std::make_unique<ConstantBuffer>((int)sizeof(PerFrameData));
		

		const int maxObjectCount = 300;
		for (int i = 0; i < maxObjectCount; i++)
			m_PerObjectConstantBuffers.push_back(std::make_unique<ConstantBuffer>((int)sizeof(PerObjectData)));
	}

	void LinePipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, 
		const PipelineInputs* const pipelineInputs)
	{
		RenderPipeline::Record(commandList, pipelineInputs);

		auto device = Renderer::Instance().GetDevice();

		LinePipelineInputs* linePipelineInputs = (LinePipelineInputs*)pipelineInputs;
		auto& collisionComponents = *linePipelineInputs->CollisionComponents;


		m_PerFrameData.ViewProjection = Matrix4x4::Transpose(linePipelineInputs->Camera->GetViewProjection());
		m_PerFrameConstantBuffer->SetData((void*)&m_PerFrameData);

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		for (int i = 0; i < collisionComponents.size(); i++)
		{
			DirectX::XMFLOAT4X4 postTransform = Matrix4x4::Identity();

			const auto collisionComponentType = collisionComponents[i]->GetPrimitiveComponentType();
			switch (collisionComponentType)
			{
			case PrimitiveComponent::EPrimitiveComponentType::Ray:
			{
				auto lineComponent = (RayComponent*)collisionComponents[i];

				const auto origin = lineComponent->GetOrigin();
				const auto length = lineComponent->GetMaxDistance();
				const auto newDirection = lineComponent->GetDirection();

				const DirectX::XMFLOAT3 currentDirection = { 1.0f, 0.0f, 0.0f };
				
			
					
				const auto scaleMatrix = Matrix4x4::Scale(length);

				auto rotationAxis = Vector3::Normalize(Vector3::Cross(currentDirection, newDirection));
				if (Vector3::IsZero(rotationAxis))
				{
					rotationAxis = { 0.0f, 0.0f, -1.0f };
				}
				const auto rotationAngle = acosf(Vector3::Dot(currentDirection, newDirection));
				const auto quternion =
					Vector4::QuternionRotationAxis(rotationAxis, rotationAngle);
				const auto rotationMatrix = Matrix4x4::RotationQuaternion(quternion);

				const auto translationMatrix = Matrix4x4::Translation(origin);
				postTransform = Matrix4x4::Multiply(scaleMatrix, rotationMatrix);
				postTransform = Matrix4x4::Multiply(postTransform, translationMatrix);
			}
				break;
			case PrimitiveComponent::EPrimitiveComponentType::BoundingBox:
			{
				const auto extents = ((BoundingBoxComponent*)collisionComponents[i])->GetExtents();
				const auto center = ((BoundingBoxComponent*)collisionComponents[i])->GetCenter();

				postTransform = Matrix4x4::Scale(extents);
				postTransform = Matrix4x4::Multiply(postTransform, Matrix4x4::Translation(center));
			}
				break;
			case PrimitiveComponent::EPrimitiveComponentType::BoundingOrientedBox:
			{
				const auto extents = ((BoundingOrientedBoxComponent*)collisionComponents[i])->GetExtents();
				const auto center = ((BoundingOrientedBoxComponent*)collisionComponents[i])->GetCenter();
				const auto orientation = ((BoundingOrientedBoxComponent*)collisionComponents[i])->GetOrientation();

				postTransform = Matrix4x4::Scale(extents);
				postTransform = Matrix4x4::Multiply(postTransform, Matrix4x4::RotationQuaternion(orientation));
				postTransform = Matrix4x4::Multiply(postTransform, Matrix4x4::Translation(center));
			}
				break;
			case PrimitiveComponent::EPrimitiveComponentType::BoundingSphere:
			{
				const auto radius = ((BoundingSphereComponent*)collisionComponents[i])->GetRadius();
				const auto center = ((BoundingSphereComponent*)collisionComponents[i])->GetCenter();

				postTransform = Matrix4x4::Scale(radius);
				postTransform = Matrix4x4::Multiply(postTransform, Matrix4x4::Translation(center));
			}
				break;
			default:
				CS_FATAL(false, "잘못된 PrimitiveComponent Type입니다.");
				break;
			}
			
			DirectX::XMFLOAT4X4 postScaledWorld = Matrix4x4::Multiply(postTransform, collisionComponents[i]->GetWorldTransform());
			m_PerObjectData.World = Matrix4x4::Transpose(postScaledWorld);
			m_PerObjectData.Color = { 1.0f, 1.0f, 0.0f };
			m_PerObjectConstantBuffers[i]->SetData((void*)&m_PerObjectData);

			device->CopyDescriptorsSimple(1, cpuHandle, m_PerObjectConstantBuffers[i]->GetCPUDescriptorHandle(),
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}



		ID3D12DescriptorHeap* descriptorHeaps[] = { m_DescriptorHeap.Get() };
		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());
		commandList->SetGraphicsRootConstantBufferView(0, m_PerFrameConstantBuffer->GetGPUVirtualAddress());

		
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		for (int i = 0; i < collisionComponents.size(); i++)
		{
			commandList->SetGraphicsRootDescriptorTable(1, gpuHandle);
			auto* renderable = collisionComponents[i]->GetRenderable();
			for(int j = 0; j < renderable->GetVertexbufferCount(); j++)
				renderable->Render(commandList, j);

			gpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
	
	}

}