#include "cspch.h"
#include "LinePipeline.h"

#include "Crystal/Core/Device.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Resources/DescriptorAllocator.h"
#include "Crystal/Renderer/Scene.h"
//#include "Crystal/GamePlay/Components/AIComponent.h"

namespace Crystal {

	void LinePipeline::Begin()
	{
		RenderPipeline::Begin();

		PrepareConstantBuffers(sizeof(PerFrameData), sizeof(PerInstanceData));

		auto device = Device::Instance().GetD3DDevice();

		auto renderSystem = Cast<RenderSystem>(GetOuter());
		auto level = Cast<Level>(renderSystem->GetOuter());
		auto& scene = level->GetScene();

		auto camera = scene->Cameras[0].lock();



		PerFrameData perFrameData = {};
		perFrameData.ViewProjection = Matrix4x4::Transpose(camera->GetViewProjection());
		m_PerFrameConstantBuffer->SetData((void*)&perFrameData, 0, sizeof(perFrameData));

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		device->CopyDescriptorsSimple(1, cpuHandle, m_PerFrameConstantBuffer->AsConstantBufferView(),
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		cpuHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


		for (int i = 0; i < scene->BoundingOrientedBoxComponents.size(); i++)
		{
			auto component = scene->BoundingOrientedBoxComponents[i].lock();
			if (!component)
				continue;

			if(component->GetHideInGame())
			{
				continue;
			}

			PerInstanceData perInstanceData = {};
			perInstanceData.World = component->GetPostScaledTransform();


			auto& materials = component->GetMaterials();
			for (const auto& mat : materials)
			{
				auto material = mat.get();
				perInstanceData.Color = material->AlbedoColor;

			}


			Shared<Renderable> renderable = scene->LineBoxMesh;
			
			if(!renderable)
			{
				continue;
			}
			
			m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);
		}

		for (int i = 0; i < scene->BoundingBoxComponents.size(); i++)
		{
			auto component = scene->BoundingBoxComponents[i].lock();
			if (!component)
				continue;

			if (component->GetHideInGame())
			{
				continue;
			}

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

			if (component->GetHideInGame())
			{
				continue;
			}

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

			if (component->GetHideInGame())
			{
				continue;
			}

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


		
		for (int i = 0; i < scene->AIPerceptions.size(); i++)
		{
			auto component = Cast<AIPerceptionComponent>(scene->AIPerceptions[i]);
			if (!component)
				continue;

			if(component->GetIsHearingEnabled())
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
			}

			if(component->GetIsSightEnabled())
			{
				CalculateBoundingFrustumTransform(component->GetSightFrustum(), component->GetSightFrustumTransform());
			}
			
		}

		



		for (auto& pair : m_InstanceBatches)
		{
			auto& instanceBatches = pair.second;

			if (!instanceBatches.PerInstanceVertexBuffer)
			{
				instanceBatches.PerInstanceVertexBuffer = std::make_unique<Buffer>(nullptr, sizeof(PerInstanceData) *
					instanceBatches.PerInstanceDatas.size(), instanceBatches.PerInstanceDatas.size(), false, true);
			}

			instanceBatches.PerInstanceVertexBuffer->SetData((void*)instanceBatches.PerInstanceDatas.data(),
				0, sizeof(PerInstanceData) * instanceBatches.PerInstanceDatas.size());

		}

	}

	void LinePipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{

		Pipeline::Record(commandList);

		auto device = Device::Instance().GetD3DDevice();
		auto rootSignature = m_Shader->GetRootSignature();


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


		for (const auto& pair : m_InstanceBatches)
		{
			auto& renderable = pair.first;
			auto& instanceBatch = pair.second;
			auto& perInstanceVertexBuffer = instanceBatch.PerInstanceVertexBuffer;

			commandList->IASetVertexBuffers(1, 1, &perInstanceVertexBuffer->AsVertexBufferView(perInstanceVertexBuffer->GetSize() / perInstanceVertexBuffer->GetElementCount()));

			if(!renderable)
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

	void LinePipeline::CalculateBoundingFrustumTransform(const Collision::BoundingFrustum& frustum, const DirectX::XMFLOAT4X4& world)
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
		perInstanceData.Color = Vector3::Yellow;


		auto nearScale = Matrix4x4::Scale({ nearWidth, nearHeight, 1.0f });
		auto nearTranslation = Matrix4x4::Translation({ 0.0f, 0.0f, frustum.Near });
		perInstanceData.World = Matrix4x4::Multiply(Matrix4x4::Multiply(nearScale, nearTranslation), world);
		m_InstanceBatches[renderable.get()].PerInstanceDatas.push_back(perInstanceData);


		auto farScale = Matrix4x4::Scale({ farWidth, farHeight, 1.0f });
		auto farTranslation = Matrix4x4::Translation({ 0.0f, 0.0f, frustum.Far });
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
				rotationAxis = { 0.0f, 0.0f, -1.0f };
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
				rotationAxis = { 0.0f, 0.0f, -1.0f };
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
				rotationAxis = { 0.0f, 0.0f, -1.0f };
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
				rotationAxis = { 0.0f, 0.0f, -1.0f };
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
