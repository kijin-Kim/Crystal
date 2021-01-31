#include "cspch.h"
#include "Pipeline.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	static Pipeline::PipelineInputs m_CurrentPipelineInputs = {};

	void Pipeline::Begin(PipelineInputs* pipelineInputs)
	{
		if (!m_bEndIsCalled)
		{
			CS_FATAL(false, "Pipeline : %s�� End�� �θ��� �ʾҽ��ϴ�", m_Name.c_str());
			return;
		}
		m_bEndIsCalled = false;

		if (!pipelineInputs)
			CS_FATAL(false, "Pipeline : %s �� PipelineInput�� nullptr�Դϴ�.", m_Name.c_str());

		m_bBeginIsCalled = true;

		m_PipelineInputs = pipelineInputs;


		/*������ RenderState�� ���� commandList�� set�� �ؾ��ϴ����� �����ϴ� flag���� �����մϴ�.*/
		auto& globalRenderState = Renderer::Instance().GetGlobalRenderState();

		m_bShouldChangePrimitiveTopology =  globalRenderState.PrimitiveTopology == pipelineInputs->PrimitiveTopology;
		m_bShouldChangeViewPort = globalRenderState.Viewport == pipelineInputs->Camera->GetViewport();

		m_bShouldChangeScissorRects =
			globalRenderState.ScissorRect.left == pipelineInputs->Camera->GetScissorRect().left &&
			globalRenderState.ScissorRect.top == pipelineInputs->Camera->GetScissorRect().top &&
			globalRenderState.ScissorRect.right == pipelineInputs->Camera->GetScissorRect().right &&
			globalRenderState.ScissorRect.bottom == pipelineInputs->Camera->GetScissorRect().bottom;

		m_bShouldChangeRenderTargets = 
			globalRenderState.RenderTargets[0] == pipelineInputs->RenderTargets[0] &&
			globalRenderState.RenderTargets[1] == pipelineInputs->RenderTargets[1] &&
			globalRenderState.RenderTargets[2] == pipelineInputs->RenderTargets[2] &&
			globalRenderState.RenderTargets[3] == pipelineInputs->RenderTargets[3] &&
			globalRenderState.RenderTargets[4] == pipelineInputs->RenderTargets[4] &&
			globalRenderState.RenderTargets[5] == pipelineInputs->RenderTargets[5] &&
			globalRenderState.RenderTargets[6] == pipelineInputs->RenderTargets[6] &&
			globalRenderState.RenderTargets[7] == pipelineInputs->RenderTargets[7];

		m_bShouldChangeDepthStencilBuffer = globalRenderState.DepthStencilBuffer == pipelineInputs->DepthStencilBuffer;

	}

	void Pipeline::Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList)
	{
		if (!m_bBeginIsCalled)
		{
			CS_FATAL(false, "Pipeline : %s�� Record�� �θ������� Begin�� �θ��� �ʾҽ��ϴ�.", m_Name.c_str());
			return;
		}
		m_bBeginIsCalled = false;

		m_bRecordIsCalled = true;

		/*CommandList�� ��ȭ�մϴ�*/
		CS_DEBUG_INFO("Pipeline : %s Record", m_Name.c_str());
	}

	void Pipeline::End()
	{
		if (!m_bRecordIsCalled)
		{
			CS_FATAL(false, "Pipeline : %s�� End�� �θ������� Record�� �θ��� �ʾҽ��ϴ�.", m_Name.c_str());
			return;
		}
		m_bRecordIsCalled = false;

		m_bEndIsCalled = true;

		m_bShouldChangePrimitiveTopology = false;
		m_bShouldChangeViewPort = false;
		m_bShouldChangeScissorRects = false;
		m_bShouldChangeRenderTargets = false;
		m_bShouldChangeDepthStencilBuffer = false;

	}

}