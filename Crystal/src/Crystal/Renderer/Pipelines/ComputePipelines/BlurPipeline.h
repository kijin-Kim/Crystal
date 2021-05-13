#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {

	class BlurPipeline : public ComputePipeline
	{
	public:
	
		BlurPipeline() = default;
		~BlurPipeline() override {}

		void Begin() override;


		STATIC_TYPE_IMPLE(BlurPipeline)
	};

}
