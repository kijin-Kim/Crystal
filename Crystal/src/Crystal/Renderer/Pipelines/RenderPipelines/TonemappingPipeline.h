#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {
	class StaticMeshComponent;

	class TonemappingPipeline final : public RenderPipeline
	{

	public:
		TonemappingPipeline() = default;
		~TonemappingPipeline() override = default;

		void OnCreate() override;
		void Begin(const Shared<Scene>& scene) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

		
		STATIC_TYPE_IMPLE(TonemappingPipeline)
	};


}
