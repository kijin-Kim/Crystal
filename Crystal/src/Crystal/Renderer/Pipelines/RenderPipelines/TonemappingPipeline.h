#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {

	class TonemappingPipeline final : public RenderPipeline
	{
	public:
		struct TonemappingPipelineInputs : public RenderPipelineInputs
		{
		};


	public:
		TonemappingPipeline() = default;
		~TonemappingPipeline() override = default;

		void OnCreate() override;
		void PrepareRecord(const PipelineInputs* const pipelineInputs) override;

		STATIC_TYPE_IMPLE(TonemappingPipeline)
	};


}