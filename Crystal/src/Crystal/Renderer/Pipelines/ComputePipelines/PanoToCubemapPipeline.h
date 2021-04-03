#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {
	class PanoToCubemapPipeline : public ComputePipeline
	{
	public:

		struct PanoToCubemapPipelineInputs : public ComputePipelineInputs
		{
			Texture* SourceTexture = nullptr;
			Texture* DestinationTexture = nullptr;
		};

		PanoToCubemapPipeline() = default;
		~PanoToCubemapPipeline() override {}

		void PrepareRecord(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs) override;
		

		STATIC_TYPE_IMPLE(PanoToCubemapPipeline)
	private:
		unsigned int m_DestinationTextureWidth = 0;
		unsigned int m_DestinationTextureHeight = 0;
	};
}
