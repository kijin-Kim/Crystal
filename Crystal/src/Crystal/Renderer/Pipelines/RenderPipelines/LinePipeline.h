#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/Resources/ConstantBuffer.h"

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

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT3 Color = {};
		};

	public:
		LinePipeline() = default;
		~LinePipeline() override = default;

		void PrepareRecord(const PipelineInputs* const pipelineInputs) override;
		

		STATIC_TYPE_IMPLE(LinePipeline)
	};
}
