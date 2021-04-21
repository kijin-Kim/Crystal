#pragma once
#include "LightPipeline.h"
#include "Crystal/Resources/Material.h"

namespace Crystal {
	class UnlitPipeline : public RenderPipeline
	{
	public:
		struct UnlitPipelineInputs : public RenderPipelineInputs
		{
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection;
		};

		struct Material : public MaterialBase
		{
			DirectX::XMFLOAT3 Color = {};
		};



	public:
		UnlitPipeline() = default;
		~UnlitPipeline() override = default;

		void Begin(const PipelineInputs* const pipelineInputs) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;

		struct PerInstanceData
		{
			DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();
			uint32_t MaterialIndex = 0;
			
			DirectX::XMFLOAT3 EmissiveColor = Vector3::Zero;
			
			uint32_t bToggleEmissivetexture = false;
		};
		



		struct InstanceBatch
		{
			std::vector<PerInstanceData> PerInstanceDatas;
			std::unique_ptr<Buffer> PerInstanceVertexBuffer = nullptr;
			std::array<NewMaterial*, 10> MaterialLookup;
			UINT64 DescriptorOffset = -1;
		};

		STATIC_TYPE_IMPLE(UnlitPipeline)

	private:
		std::map<Renderable*, InstanceBatch> m_InstanceBatches;
	};
	
}

