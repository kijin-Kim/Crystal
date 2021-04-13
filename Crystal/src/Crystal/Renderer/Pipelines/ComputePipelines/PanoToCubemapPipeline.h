#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {
	class PanoToCubemapPipeline : public ComputePipeline
	{
	public:
		struct PanoToCubemapPipelineInputs : public ComputePipelineInputs
		{
		};

		PanoToCubemapPipeline() = default;
		~PanoToCubemapPipeline() override {}

		void Begin(const PipelineInputs* const pipelineInputs) override;
		

		STATIC_TYPE_IMPLE(PanoToCubemapPipeline)
	private:
		unsigned int m_DestinationTextureWidth = 0;
		unsigned int m_DestinationTextureHeight = 0;
	};
}
