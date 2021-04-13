#pragma once
#include "Crystal/Resources/Buffer.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "LightPipeline.h"
#include <vector>


namespace Crystal {
	class LightingStaticPipeline final : public LightPipeline
	{
	public:

		struct Light
		{
			DirectX::XMFLOAT3 WorldPosition = Vector3::Zero;
			float _pad1;

			DirectX::XMFLOAT3 Color = Vector3::White;
			float _pad2;

			float Intensity;
			float _pad3[3];
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection = Matrix4x4::Identity();
		};

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
		};

		struct PerDrawData
		{
			DirectX::XMFLOAT3 AlbedoColor = Vector3::Zero;
			float _pad1;
			DirectX::XMFLOAT3 EmissiveColor = Vector3::Zero;
			float _pad2;
			uint32_t bToggleAlbedoTexture = false;
			uint32_t bToggleMetallicTexture = false;
			uint32_t bToggleRoughnessTexture = false;
			uint32_t bToggleNormalTexture = false;
			uint32_t bToggleIrradianceTexture = false;
			uint32_t bToggleEmissiveTexture = false;
			float RoughnessConstant = 0.0f;
			float MetallicConstant = 0.0f;
		};

		struct Material : public Pipeline::MaterialBase
		{
			std::weak_ptr<Texture> AlbedoTexture;
			DirectX::XMFLOAT3 AlbedoColor = Vector3::Zero;

			std::weak_ptr<Texture> MetallicTexture;
			float MetallicConstant = 0.0f;

			std::weak_ptr<Texture> RoughnessTexture;
			float RoughnessConstant = 0.0f;

			std::weak_ptr<Texture> NormalTexture;

			std::weak_ptr<Texture> EmissiveTexture;
			DirectX::XMFLOAT3 EmissiveColor = Vector3::Zero;



			bool UsingSameTextures(MaterialBase* material) override;

			STATIC_TYPE_IMPLE(LightingStaticPipeline::Material)
		};

		struct LightingPipelineInputs : public RenderPipelineInputs
		{
			Texture* IrradiancemapTexture = nullptr;
		};
		
		struct PerInstanceData
		{
			DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();
			uint32_t MaterialIndex = 0;
			DirectX::XMFLOAT3 AlbedoColor = Vector3::Zero;
			DirectX::XMFLOAT3 EmissiveColor = Vector3::Zero;
			float RoughnessConstant = 0.0f;
			float MetallicConstant = 0.0f;

			uint32_t bToggleAlbedoTexture = false;
			uint32_t bToggleMetallicTexture = false;
			uint32_t bToggleRoughnessTexture = false;
			uint32_t bToggleNormalTexture = false;
			uint32_t bToggleIrradianceTexture = false;
			uint32_t bToggleEmissivetexture = false;
		};

		struct InstanceBatch
		{
			std::vector<PerInstanceData> PerInstanceDatas;
			std::unique_ptr<Buffer> PerInstanceVertexBuffer = nullptr;
			std::array<MaterialBase*, 10> MaterialLookup;
			UINT64 DescriptorOffset = -1;
		};

	public:
		LightingStaticPipeline() = default;
		~LightingStaticPipeline() override = default;

		void Begin(const PipelineInputs* const pipelineInputs) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;
		STATIC_TYPE_IMPLE(LightingStaticPipeline)
	private:
		std::map<Renderable*, InstanceBatch> m_InstanceBatches;
	};
}
