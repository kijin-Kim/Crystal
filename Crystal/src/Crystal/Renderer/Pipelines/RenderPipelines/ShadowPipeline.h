#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {

	class ShadowPipeline : public RenderPipeline
	{
	public:
		ShadowPipeline() = default;
		~ShadowPipeline() override = default;


	
	private:
		/*void OnCreate() override;
		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;*/




		STATIC_TYPE_IMPLE(ShadowPipeline)
		
	};
	
}
