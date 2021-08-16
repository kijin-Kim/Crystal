#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {
	class DiffIrradSamplingPipeline : public ComputePipeline
	{
	public:
		enum EDiffIrradSamplingDescriptorInputType
		{
			SourceTexture = 0,
			DestinationTexture = 1,
			DIFF_IRRAD_SAMPLING_INPUT_COUNT
		};


		DiffIrradSamplingPipeline() = default;
		~DiffIrradSamplingPipeline() override = default;

		void Begin(const Shared<Scene>& scene) override;


		STATIC_TYPE_IMPLE(DiffIrradSamplingPipeline)
		
	private:

		unsigned int m_DestinationTextureWidth = 0;
		unsigned int m_DestinationTextureHeight = 0;
	};
}
