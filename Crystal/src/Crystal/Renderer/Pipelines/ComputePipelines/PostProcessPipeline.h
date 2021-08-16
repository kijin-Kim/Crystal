#pragma once
#include "Crystal/GamePlay/Components/PostProcessComponent.h"
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {

	class PostProcessPipeline : public ComputePipeline
	{
	public:
		struct PerExecuteData
		{
			DirectX::XMFLOAT3 AlbedoColor = { 0.0f, 0.0f, 0.0f };
			float Opacity = 1.0f;
			DirectX::XMFLOAT3 AlbedoColorMultiplier = { 1.0f, 1.0f, 1.0f };
			float OpacityMultiplier = 1.0f;
			DirectX::XMFLOAT2 TexCoordMultiplier = { 1.0f, 1.0f };


			int bToggleAlbedoTexture = false;
			int bToggleOpacityTexture = false;
			int bToggleSceneColorMaskTexture = false;
		};

	public:
		PostProcessPipeline() = default;
		~PostProcessPipeline() override {}

		void Begin(const Shared<Scene>& scene) override;
		void End() override;

		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

		STATIC_TYPE_IMPLE(PostProcessPipeline)

	private:
		struct RenderData
		{
			Weak<PostProcessComponent> Component;
			SIZE_T DescriptorHeapOffsets;
		};

		std::multimap<float, RenderData> m_OrderSortedPostProcesses;
	};

}
