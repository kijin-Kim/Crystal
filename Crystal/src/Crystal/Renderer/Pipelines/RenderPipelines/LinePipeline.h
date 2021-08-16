#pragma once
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Buffer.h"



namespace Crystal {

	namespace Collision {
		class BoundingFrustum;
	}

	class BoundingFrustumComponent;
	
	class LinePipeline : public RenderPipeline
	{
	public:

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection;
		};

		struct PerInstanceData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT3 Color = {};
		};



	public:
		LinePipeline() = default;
		~LinePipeline() override = default;


		void OnCreate() override;
		void Begin(const Shared<Scene>& scene) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;

		

		STATIC_TYPE_IMPLE(LinePipeline)
	
	private:
		void CalculateBoundingFrustumTransform(const Collision::BoundingFrustum& frustum, const DirectX::XMFLOAT4X4& world);
		
	private:

		struct InstanceBatch
		{
			std::vector<PerInstanceData> PerInstanceDatas;
			std::unique_ptr<Buffer> PerInstanceVertexBuffer = nullptr;
		};
		std::map<Renderable*, InstanceBatch> m_InstanceBatches;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
	};
}
