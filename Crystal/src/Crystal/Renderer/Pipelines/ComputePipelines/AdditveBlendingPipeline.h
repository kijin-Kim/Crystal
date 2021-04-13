#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {

	class AdditiveBlendingPipeline : public ComputePipeline
	{
	public:
		struct AdditiveBlendingPipelineInputs : public ComputePipelineInputs
		{
		};

		AdditiveBlendingPipeline() = default;
		~AdditiveBlendingPipeline() override {}

		void Begin(const PipelineInputs* const pipelineInputs) override;


		STATIC_TYPE_IMPLE(AdditiveBlendingPipeline)
	};



}
