#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {

	class AdditiveBlendingPipeline : public ComputePipeline
	{
	public:


		AdditiveBlendingPipeline() = default;
		~AdditiveBlendingPipeline() override {}

		void Begin() override;


		STATIC_TYPE_IMPLE(AdditiveBlendingPipeline)
	};



}
