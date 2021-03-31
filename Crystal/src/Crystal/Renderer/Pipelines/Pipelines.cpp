#include "cspch.h"
#include "Pipelines.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {
	void Pipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs)
	{
		/*CommandList를 녹화합니다*/
		//CS_DEBUG_INFO("Pipeline : %s Record", m_Name.c_str());
	}

	void RenderPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs)
	{
		Pipeline::Record(commandList, pipelineInputs);
	}
}