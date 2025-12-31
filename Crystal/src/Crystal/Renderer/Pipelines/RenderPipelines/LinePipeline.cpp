#include "cspch.h"
#include "LinePipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/Components/PostProcessComponent.h"
#include "Crystal/GamePlay/Objects/Actors/PostProcessVolumeActor.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Resources/DescriptorAllocator.h"
#include "Crystal/Renderer/Scene.h"
#include "Crystal/Renderer/Pipelines/PipelineStateHelper.h"
//#include "Crystal/GamePlay/Components/AIComponent.h"

namespace Crystal {


	void LinePipeline::OnCreate()
	{
		auto device = Device::Instance().GetD3DDevice();


		CD3DX12_ROOT_PARAMETER1 rootParameters[1];
		rootParameters[0].InitAsConstants(16, 0);


		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr,
		                     D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		// CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(),
		                                 rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		// CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");



		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"MATROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"MATROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
		};


		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {inputLayout, _countof(inputLayout)};

		D3D12_RT_FORMAT_ARRAY rtvFormat = {};
		rtvFormat.NumRenderTargets = 1;
		rtvFormat.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		RenderTargetDescription renderTargetDescription(rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);

		PipelineStateDescription pipelineStateDescription(
			inputLayoutDesc,
			StateHelper::Opaque,
			StateHelper::DepthEnable,
			StateHelper::CullCounterClock,
			renderTargetDescription,
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE
		);


		pipelineStateDescription.CreatePipelineState(m_RootSignature, m_Shader, m_PipelineState);
	}

	void LinePipeline::Begin(const Shared<Scene>& scene)
	{
		auto& worldConfig = GetWorldConfig();
		if(!worldConfig.bShowDebugAI && !worldConfig.bShowDebugCollision)
		{
			return;
		}
		
		RenderPipeline::Begin(scene);

		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerInstanceData));



		if(worldConfig.bShowDebugCollision)
		{
			for (int i = 0; i < scene->BoundingOrientedBoxComponents.size(); i++)
			{
				auto component = scene->BoundingOrientedBoxComponents[i].lock();
				if (!component)
					continue;

			

				PerInstanceData perInstanceData = {};
				perInstanceData.World = component->GetPostScaledTransform();


				auto& materials = component->GetMaterials();
				for (const auto& mat : materials)
				{
					auto material = mat.get();
					perInstanceData.Color = material->AlbedoColor;
				}


				Shared<Renderable> renderable = scene->LineBoxMesh;

				if (!renderable)
				{
					continue;
				}

				m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);
			}

			for (int i = 0; i < scene->BoundingSphereComponents.size(); i++)
			{
				auto component = scene->BoundingSphereComponents[i].lock();
				if (!component)
					continue;

				PerInstanceData perInstanceData = {};
				perInstanceData.World = component->GetPostScaledTransform();


				auto& materials = component->GetMaterials();
				for (const auto& mat : materials)
				{
					auto material = mat.get();
					perInstanceData.Color = material->AlbedoColor;
				}


				Shared<Renderable> renderable = scene->LineSphereMesh;

				if (!renderable)
				{
					continue;
				}

				m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);
			}


			for (int i = 0; i < scene->RayComponents.size(); i++)
			{
				auto component = scene->RayComponents[i].lock();
				if (!component)
					continue;



				PerInstanceData perInstanceData = {};
				perInstanceData.World = component->GetPostScaledTransform();


				auto& materials = component->GetMaterials();
				for (const auto& mat : materials)
				{
					auto material = mat.get();
					perInstanceData.Color = material->AlbedoColor;
				}


				Shared<Renderable> renderable = scene->LineMesh;

				if (!renderable)
				{
					continue;
				}

				m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);
			}

			for (int i = 0; i < scene->BoundingFrustumComponents.size(); i++)
			{
				auto component = Cast<BoundingFrustumComponent>(scene->BoundingFrustumComponents[i]);
				if (!component)
					continue;

				CalculateBoundingFrustumTransform(component->GetBoundingFrustum(), component->GetWorldTransform());
			}

			for(int i = 0; i < scene->PostProcesses.size(); i++)
			{
				auto component = scene->PostProcesses[i].lock();
				if (!component)
					continue;


				auto volumeActor = Cast<PostProcessVolumeActor>(component->GetOuter());
				
				PerInstanceData perInstanceData = {};
				perInstanceData.World = volumeActor->GetPostScaledTransform();
				perInstanceData.Color = { 0.5f, 0.5f, 0.5f };


				Shared<Renderable> renderable;

				auto type = volumeActor->GetVolumeType();
				switch (type)
				{
				case EVolumeType::VT_Box:
					renderable = scene->LineBoxMesh;
					break;
				case EVolumeType::VT_Sphere:
					renderable = scene->LineSphereMesh;
					break;
				default: ;
				}

				if (!renderable)
				{
					continue;
				}

				m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);
				
			}
		}


		
		if(worldConfig.bShowDebugAI)
		{
			for (int i = 0; i < scene->AIPerceptions.size(); i++)
			{
				auto component = Cast<AIPerceptionComponent>(scene->AIPerceptions[i]);
				if (!component)
					continue;

				if (component->GetIsHearingEnabled())
				{
					PerInstanceData perInstanceData = {};
					perInstanceData.World = component->GetHearingSphereTransform();
					perInstanceData.Color = Vector3::Green;

					Shared<Renderable> renderable = scene->LineSphereMesh;

					if (!renderable)
					{
						continue;
					}

					m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);



					for (auto& noise : component->GetPerceptedNoiseStimuli())
					{
						perInstanceData.World = Matrix4x4::Multiply(Matrix4x4::Scale(noise.MaxRange), Matrix4x4::Translation(noise.Position));
						perInstanceData.Color = Vector3::Blue;
						m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);
					}
				}

				if (component->GetIsSightEnabled())
				{
					CalculateBoundingFrustumTransform(component->GetSightFrustum(), component->GetSightFrustumTransform());
				}
			}
		}

		
		for (auto& pair : m_InstanceBatches)
		{
			auto& instanceBatches = pair.second;

			if (!instanceBatches.PerInstanceVertexBuffer)
			{
				instanceBatches.PerInstanceVertexBuffer = std::make_unique<Buffer>(nullptr, sizeof(PerInstanceData) *
				                                                                   instanceBatches.PerInstanceDatas.size(),
				                                                                   instanceBatches.PerInstanceDatas.size(), false, true);
			}

			instanceBatches.PerInstanceVertexBuffer->SetData((void*)instanceBatches.PerInstanceDatas.data(),
			                                                 0, sizeof(PerInstanceData) * instanceBatches.PerInstanceDatas.size());
		}
	}

	void LinePipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		
		commandList->SetPipelineState(m_PipelineState.Get());
		commandList->SetGraphicsRootSignature(m_RootSignature.Get());


		auto& scene = GetScene();
		auto camera = scene->Cameras[0].lock();
		commandList->SetGraphicsRoot32BitConstants(0, 16, &Matrix4x4::Transpose(camera->GetViewProjection()), 0);


		for (const auto& pair : m_InstanceBatches)
		{
			auto& renderable = pair.first;
			auto& instanceBatch = pair.second;
			auto& perInstanceVertexBuffer = instanceBatch.PerInstanceVertexBuffer;

			commandList->IASetVertexBuffers(
				1, 1, &perInstanceVertexBuffer->AsVertexBufferView(perInstanceVertexBuffer->GetSize() / perInstanceVertexBuffer->GetElementCount()));

			if (!renderable)
			{
				continue;
			}


			//여기서부터 Texture2D Array Per Instance
			for (int j = 0; j < renderable->GetVertexbufferCount(); j++)
			{
				renderable->Render(commandList, j, perInstanceVertexBuffer->GetSize() / sizeof(PerInstanceData));
			}
		}
	}

	void LinePipeline::End()
	{
		m_InstanceBatches.clear();
	}

	void LinePipeline::CalculateBoundingFrustumTransform(const DirectX::BoundingFrustum& frustum, const DirectX::XMFLOAT4X4& world)
	{
		DirectX::XMFLOAT3 corners[8];
		frustum.GetCorners(corners);

		auto scene = GetScene();

		Shared<Renderable> renderable = scene->LineQuadMesh;
		if (!renderable)
		{
			return;
		}

		float nearWidth = fabs(corners[0].x - corners[2].x);
		float nearHeight = fabs(corners[0].y - corners[2].y);

		float farWidth = fabs(corners[4].x - corners[6].x);
		float farHeight = fabs(corners[4].y - corners[6].y);


		PerInstanceData perInstanceData = {};
		perInstanceData.Color = Vector3::Green;


		auto nearScale = Matrix4x4::Scale({nearWidth, nearHeight, 1.0f});
		auto nearTranslation = Matrix4x4::Translation({0.0f, 0.0f, frustum.Near});
		perInstanceData.World = Matrix4x4::Multiply(Matrix4x4::Multiply(nearScale, nearTranslation), world);
		m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);


		auto farScale = Matrix4x4::Scale({farWidth, farHeight, 1.0f});
		auto farTranslation = Matrix4x4::Translation({0.0f, 0.0f, frustum.Far});
		perInstanceData.World = Matrix4x4::Multiply(Matrix4x4::Multiply(farScale, farTranslation), world);
		m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);


		renderable = scene->LineMesh;
		if (!renderable)
		{
			return;
		}

		{
			auto farNearCornerSub = Vector3::Subtract(corners[4], corners[0]);
			const auto scaleMatrix = Matrix4x4::Scale(Vector3::Length(farNearCornerSub));


			auto direction = Vector3::Normalize(farNearCornerSub);
			auto rotationAxis = Vector3::Normalize(Vector3::Cross(Vector3::UnitX, direction));
			if (Vector3::IsZero(rotationAxis))
			{
				rotationAxis = {0.0f, 0.0f, -1.0f};
			}
			const auto rotationAngle = acosf(Vector3::Dot(Vector3::UnitX, direction));
			const auto quaternion = Vector4::QuaternionRotationAxis(rotationAxis, rotationAngle);
			const auto rotationMatrix = Matrix4x4::RotationQuaternion(quaternion);
			DirectX::XMFLOAT4X4 scaleRotation = Matrix4x4::Multiply(scaleMatrix, rotationMatrix);


			scaleRotation = Matrix4x4::Multiply(scaleRotation, Matrix4x4::Translation(corners[0]));
			perInstanceData.World = Matrix4x4::Multiply(scaleRotation, world);
			m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);
		}

		{
			auto farNearCornerSub = Vector3::Subtract(corners[5], corners[1]);
			const auto scaleMatrix = Matrix4x4::Scale(Vector3::Length(farNearCornerSub));


			auto direction = Vector3::Normalize(farNearCornerSub);
			auto rotationAxis = Vector3::Normalize(Vector3::Cross(Vector3::UnitX, direction));
			if (Vector3::IsZero(rotationAxis))
			{
				rotationAxis = {0.0f, 0.0f, -1.0f};
			}
			const auto rotationAngle = acosf(Vector3::Dot(Vector3::UnitX, direction));
			const auto quaternion = Vector4::QuaternionRotationAxis(rotationAxis, rotationAngle);
			const auto rotationMatrix = Matrix4x4::RotationQuaternion(quaternion);
			DirectX::XMFLOAT4X4 scaleRotation = Matrix4x4::Multiply(scaleMatrix, rotationMatrix);


			scaleRotation = Matrix4x4::Multiply(scaleRotation, Matrix4x4::Translation(corners[1]));
			perInstanceData.World = Matrix4x4::Multiply(scaleRotation, world);
			m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);
		}

		{
			auto farNearCornerSub = Vector3::Subtract(corners[6], corners[2]);
			const auto scaleMatrix = Matrix4x4::Scale(Vector3::Length(farNearCornerSub));


			auto direction = Vector3::Normalize(farNearCornerSub);
			auto rotationAxis = Vector3::Normalize(Vector3::Cross(Vector3::UnitX, direction));
			if (Vector3::IsZero(rotationAxis))
			{
				rotationAxis = {0.0f, 0.0f, -1.0f};
			}
			const auto rotationAngle = acosf(Vector3::Dot(Vector3::UnitX, direction));
			const auto quaternion = Vector4::QuaternionRotationAxis(rotationAxis, rotationAngle);
			const auto rotationMatrix = Matrix4x4::RotationQuaternion(quaternion);
			DirectX::XMFLOAT4X4 scaleRotation = Matrix4x4::Multiply(scaleMatrix, rotationMatrix);


			scaleRotation = Matrix4x4::Multiply(scaleRotation, Matrix4x4::Translation(corners[2]));
			perInstanceData.World = Matrix4x4::Multiply(scaleRotation, world);
			m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);
		}

		{
			auto farNearCornerSub = Vector3::Subtract(corners[7], corners[3]);
			const auto scaleMatrix = Matrix4x4::Scale(Vector3::Length(farNearCornerSub));


			auto direction = Vector3::Normalize(farNearCornerSub);
			auto rotationAxis = Vector3::Normalize(Vector3::Cross(Vector3::UnitX, direction));
			if (Vector3::IsZero(rotationAxis))
			{
				rotationAxis = {0.0f, 0.0f, -1.0f};
			}
			const auto rotationAngle = acosf(Vector3::Dot(Vector3::UnitX, direction));
			const auto quaternion = Vector4::QuaternionRotationAxis(rotationAxis, rotationAngle);
			const auto rotationMatrix = Matrix4x4::RotationQuaternion(quaternion);
			DirectX::XMFLOAT4X4 scaleRotation = Matrix4x4::Multiply(scaleMatrix, rotationMatrix);


			scaleRotation = Matrix4x4::Multiply(scaleRotation, Matrix4x4::Translation(corners[3]));
			perInstanceData.World = Matrix4x4::Multiply(scaleRotation, world);
			m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);
		}
	}
}
