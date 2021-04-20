#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {
	class StaticMeshComponent;

	class TonemappingPipeline final : public RenderPipeline
	{
	public:
		struct TonemappingPipelineInputs : public RenderPipelineInputs
		{
		};


	public:
		TonemappingPipeline() = default;
		~TonemappingPipeline() override = default;

		void OnCreate() override;
		void Begin(const PipelineInputs* const pipelineInputs) override;
		

		STATIC_TYPE_IMPLE(TonemappingPipeline)

	private:
		std::shared_ptr<StaticMeshComponent> m_StaticMeshComponent = nullptr;
	};


}
