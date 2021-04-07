#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {

	class BlurPipeline : public ComputePipeline
	{
	public:
		struct BlurPipelineInputs : public ComputePipelineInputs
		{
		};

		BlurPipeline() = default;
		~BlurPipeline() override {}

		void PrepareRecord(const PipelineInputs* const pipelineInputs) override;


		STATIC_TYPE_IMPLE(BlurPipeline)
	};

}
