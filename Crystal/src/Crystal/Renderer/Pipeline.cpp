#include "cspch.h"
#include "Pipeline.h"
#include "Renderer.h"

namespace Crystal {

	GraphicsPipeline::GraphicsPipeline(const D3D12_PIPELINE_STATE_STREAM_DESC* desc)
	{
		auto device = Renderer::Get().GetDevice();
		HRESULT hr = device->CreatePipelineState(desc, IID_PPV_ARGS(&m_PipelineState));
		CS_ASSERT(SUCCEEDED(hr), "Graphics Pipeline State Object를 생성하는데 실패하였습니다");
	}

}