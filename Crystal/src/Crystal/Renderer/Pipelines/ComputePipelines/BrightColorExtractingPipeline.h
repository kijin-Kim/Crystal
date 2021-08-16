#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {

	class BrightColorExtractingPipeline : public ComputePipeline
	{
	public:
		BrightColorExtractingPipeline() = default;
		~BrightColorExtractingPipeline() override = default;

		void Begin(const Shared<Scene>& scene) override;

		STATIC_TYPE_IMPLE(BrightColorExtractingPipeline)
		
	};
	
}
