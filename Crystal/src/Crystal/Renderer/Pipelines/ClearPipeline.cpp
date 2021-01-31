#include "cspch.h"
#include "ClearPipeline.h"

namespace Crystal {


	void ClearPipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		Pipeline::Record(commandList);

		ClearPipelineInputs* clearPipelineInputs = (ClearPipelineInputs*)m_PipelineInputs;

		if (!clearPipelineInputs->bClearDepth && !clearPipelineInputs->bClearStencil)
		{
			//CS_WARN("Pipeline : %s는 필요없는 Pipeline입니다.", m_Name.c_str());
			return;
		}

		for (auto renderTarget : clearPipelineInputs->RenderTargets)
		{
			if (!renderTarget)
				continue;

			commandList->ClearRenderTargetView(renderTarget->GetRenderTargetView(), clearPipelineInputs->ClearColorValue.data(), 0, nullptr);
		}

		D3D12_CLEAR_FLAGS d3d12ClearFlags = {};
		if (clearPipelineInputs->bClearDepth)
			d3d12ClearFlags |= D3D12_CLEAR_FLAG_DEPTH;
		if(clearPipelineInputs->bClearStencil)
			d3d12ClearFlags |= D3D12_CLEAR_FLAG_STENCIL;

		commandList->ClearDepthStencilView(clearPipelineInputs->DepthStencilBuffer->GetDepthStencilView(), 
			d3d12ClearFlags, clearPipelineInputs->ClearDepthValue, clearPipelineInputs->ClearStencilValue, 0, nullptr);
	}

}