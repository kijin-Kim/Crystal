#pragma once
#include <d3d12.h>
#include <memory>

#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Shader.h"

namespace Crystal {

	class GraphicsPipeline final
	{
	public:
		GraphicsPipeline(const D3D12_PIPELINE_STATE_STREAM_DESC* desc);

		ID3D12PipelineState* GetRaw() const { return m_PipelineState.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
	};
}
