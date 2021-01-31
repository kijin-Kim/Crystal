#pragma once
#include "Pipeline.h"
#include <array>

namespace Crystal {

	class ClearPipeline final : public Pipeline
	{
	public:
		struct ClearPipelineInputs : public PipelineInputs
		{
			std::array<float, 4> ClearColorValue = { 0.0f, 0.0f, 0.0f, 0.0f };
			bool bClearDepth = true;
			float ClearDepthValue = 1.0f;
			bool bClearStencil = true;
			float ClearStencilValue = 0.0f;
		};

	public:
		ClearPipeline(const std::string& name) : Pipeline(name) {};
		virtual ~ClearPipeline() = default;

		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
	};

}