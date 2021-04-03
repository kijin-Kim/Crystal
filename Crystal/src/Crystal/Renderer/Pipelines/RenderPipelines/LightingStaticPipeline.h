#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/ConstantBuffer.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"

namespace Crystal {
	class LightingStaticPipeline final : public RenderPipeline
	{
	public:
		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection;
			DirectX::XMFLOAT4 CameraPositionInWorld;
			DirectX::XMFLOAT4 LightPositionInWorld[2];
		};

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
		};

		struct PerDrawData
		{
			DirectX::XMFLOAT3 AlbedoColor = { 1.0f, 0.0f, 0.0f };
			int bToggleAlbedoTexture = false;
			int bToggleMetallicTexture = false;
			int bToggleRoughnessTexture = false;
			int bToggleNormalTexture = false;
			int bToggleIrradianceTexture = false;
			float RoughnessConstant = 0.0f;
			float MetallicConstant = 0.0f;
		};

		struct LightingPipelineInputs : public RenderPipelineInputs
		{
			std::vector<SkeletalMeshComponent*>* SkeletalMeshComponents = nullptr;
			std::vector<StaticMeshComponent*>* StaticMeshComponents = nullptr;
			Texture* IrradiancemapTexture = nullptr;
		};

	public:
		LightingStaticPipeline() = default;
		~LightingStaticPipeline() override = default;

		void OnCreate() override;

		void PrepareRecord(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList,
			const PipelineInputs* const pipelineInputs) override;
		STATIC_TYPE_IMPLE(LightingStaticPipeline)
	private:
		PerFrameData m_PerFrameData = {};

		std::vector<std::unique_ptr<ConstantBuffer>> m_PerObjectConstantBuffers;
		std::vector<std::array<std::unique_ptr<ConstantBuffer>, 5>> m_PerDrawConstantBufferLists;

		std::unique_ptr<ConstantBuffer> m_PerFrameConstantBuffer = nullptr;
	};
}
