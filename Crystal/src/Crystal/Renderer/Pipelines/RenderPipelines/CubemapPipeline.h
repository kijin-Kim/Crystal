#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Buffer.h"

namespace Crystal {
	class CubemapPipeline final : public RenderPipeline
	{
	public:
		struct CubemapPipelineInputs : public RenderPipelineInputs
		{
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 InverseViewProjection;
		};

		

	public:
		CubemapPipeline() = default;
		~CubemapPipeline() override = default;

		void Begin(const PipelineInputs* const pipelineInputs) override;

		STATIC_TYPE_IMPLE(CubemapPipeline)
	};
}
