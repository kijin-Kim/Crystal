#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/ConstantBuffer.h"
#include "Crystal/Resources/Buffers.h"

namespace Crystal {
	class CubemapPipeline final : public RenderPipeline
	{
	public:
		struct CubemapPipelineInputs : public RenderPipelineInputs
		{
			Texture* CubemapTexture = nullptr;
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 InverseViewProjection;
		};

	public:
		CubemapPipeline() = default;
		~CubemapPipeline() override = default;

		void OnCreate() override;

		void PrepareRecord(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
			const PipelineInputs* const pipelineInputs) override;
	private:
		std::unique_ptr<ConstantBuffer> m_PerFrameConstantBuffer = nullptr;

		PerFrameData m_PerFrameData = {};
	};
}
