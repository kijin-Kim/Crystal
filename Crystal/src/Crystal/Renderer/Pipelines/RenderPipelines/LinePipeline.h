#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Buffer.h"

namespace Crystal {
	class LinePipeline : public RenderPipeline
	{
	public:
		struct LinePipelineInputs : public RenderPipelineInputs
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
		LinePipeline() = default;
		~LinePipeline() override = default;

		void Begin(const PipelineInputs* const pipelineInputs) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;
		

	
		struct InstanceBatch
		{
			std::vector<PerInstanceData> PerInstanceDatas;
			std::unique_ptr<Buffer> PerInstanceVertexBuffer = nullptr;
		};

		STATIC_TYPE_IMPLE(LinePipeline)
	
	private:
		std::map<Renderable*, InstanceBatch> m_InstanceBatches;
	};
}
