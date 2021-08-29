#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Buffer.h"

namespace Crystal {
	class StaticMeshComponent;

	class CubemapPipeline final : public RenderPipeline
	{
	public:


		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 InverseViewProjection;
			DirectX::XMFLOAT4 AdditiveColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			float RedLevel = 1.0f;
			float GreenLevel = 1.0f;
			float BlueLevel = 1.0f;
			float Power = 0.8f;
			float Saturation = 1.1f;
			float NearStarDesaturation = 0.0f;
			float FarStarDesaturation = 0.0f;
		};

		

	public:
		CubemapPipeline() = default;
		~CubemapPipeline() override = default;

		void OnCreate() override;
		void Begin(const Shared<Scene>& scene) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

		STATIC_TYPE_IMPLE(CubemapPipeline)


	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
	};
}
