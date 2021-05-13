#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {
	class PanoToCubemapPipeline : public ComputePipeline
	{
	public:


		PanoToCubemapPipeline() = default;
		~PanoToCubemapPipeline() override {}

		void Begin() override;
		

		STATIC_TYPE_IMPLE(PanoToCubemapPipeline)
	private:
		unsigned int m_DestinationTextureWidth = 0;
		unsigned int m_DestinationTextureHeight = 0;
	};
}
