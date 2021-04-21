#pragma once
#include "LightPipeline.h"

namespace Crystal {
	class UnlitPipeline : public RenderPipeline
	{
	public:
		struct UnlitPipelineInputs : public RenderPipelineInputs
		{
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection;
		};

		struct PerInstanceData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT3 Color = {};
		};


		struct Material : public MaterialBase
		{
			DirectX::XMFLOAT3 Color = {};
		};



	public:
		UnlitPipeline() = default;
		~UnlitPipeline() override = default;

		void Begin(const PipelineInputs* const pipelineInputs) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;



		struct InstanceBatch
		{
			std::vector<PerInstanceData> PerInstanceDatas;
			std::unique_ptr<Buffer> PerInstanceVertexBuffer = nullptr;
		};

		STATIC_TYPE_IMPLE(UnlitPipeline)

	private:
		std::map<Renderable*, InstanceBatch> m_InstanceBatches;
	};
	
}

