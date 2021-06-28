#pragma once
#include "Crystal/Math/Math.h"
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Buffer.h"
#include "Crystal/Resources/Material.h"
#include "Crystal/Resources/Renderable.h"

namespace Crystal {

	class ShadowMapStaticPipeline : public RenderPipeline
	{
	public:
		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection = Matrix4x4::Identity();
		};

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
		};


		struct PerInstanceData
		{
			DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();
		};

		struct InstanceBatch
		{
			std::vector<PerInstanceData> PerInstanceDatas;
			std::unique_ptr<Buffer> PerInstanceVertexBuffer = nullptr;
		};

	public:
		ShadowMapStaticPipeline() = default;
		~ShadowMapStaticPipeline() override = default;


		void OnCreate() override;
		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;
		STATIC_TYPE_IMPLE(ShadowMapStaticPipeline)
	private:
		std::map<Renderable*, InstanceBatch> m_InstanceBatches;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		
	};
	
}
