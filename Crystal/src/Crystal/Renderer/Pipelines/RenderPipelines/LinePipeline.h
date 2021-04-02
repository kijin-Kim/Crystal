#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Crystal/Resources/ConstantBuffer.h"

namespace Crystal {
	class LinePipeline : public RenderPipeline
	{
	public:
		struct LinePipelineInputs : public RenderPipelineInputs
		{
			std::vector<CollisionComponent*>* CollisionComponents = nullptr;
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection;
		};

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT3 Color = {};
		};

	public:
		LinePipeline() = default;
		~LinePipeline() override = default;

		void OnCreate() override;

		void PrepareRecord(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
			const PipelineInputs* const pipelineInputs) override;
		
	private:

		std::unique_ptr<ConstantBuffer> m_PerFrameConstantBuffer = nullptr;
		PerFrameData m_PerFrameData = {};
		std::vector<std::unique_ptr<ConstantBuffer>> m_PerObjectConstantBuffers;
		PerObjectData m_PerObjectData = {};
	};
}
