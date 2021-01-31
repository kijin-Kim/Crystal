#pragma once
#include <string>
#include <vector>

#include "Crystal/GamePlay/Components/BaseComponents.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"
#include "Crystal/Resources/Texture.h"

namespace Crystal {

	class Pipeline
	{
	public:
		struct PipelineInputs
		{
			std::vector<RenderComponent*>* RenderComponents = nullptr;
			CameraComponent* Camera = nullptr;
			Texture* RenderTargets[8] = {};
			Texture* DepthStencilBuffer = nullptr;
			D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		};

	public:
		Pipeline(const std::string& name) : m_Name(name) {};
		virtual ~Pipeline() = default;

		virtual void Begin(PipelineInputs* pipelineInputs);
		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList);
		virtual void End();

	protected:
		bool m_bShouldChangePrimitiveTopology = false;
		bool m_bShouldChangeViewPort = false;
		bool m_bShouldChangeScissorRects = false;
		bool m_bShouldChangeRenderTargets = false;
		bool m_bShouldChangeDepthStencilBuffer  = false;
		PipelineInputs* m_PipelineInputs = nullptr;
	private:
		std::string m_Name;

		bool m_bBeginIsCalled = false;
		bool m_bRecordIsCalled = false;
		bool m_bEndIsCalled = true;

	};

}
