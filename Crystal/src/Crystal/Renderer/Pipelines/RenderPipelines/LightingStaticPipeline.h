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

			DirectX::XMFLOAT3 CameraPositionInWorld = Vector3::Zero;
			float _pad1;

			Light Lights[20];

			uint32_t LightCount = 0;
			float _pad2[3]; // just in case
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



			bool CanBeInstancedTogether(MaterialBase* material) override
			{
				auto other = (Material*)material;

				if (!material)
					return false;

				bool result = true;
				if (!AlbedoTexture.expired() && !other->AlbedoTexture.expired())
				{
					result &= AlbedoTexture.lock() == other->AlbedoTexture.lock();
					if (!result)
						return false;
				}
				else if((AlbedoTexture.expired() != other->AlbedoTexture.expired()))
				{
					return false;
				}

				if (!MetallicTexture.expired() && !other->MetallicTexture.expired())
				{
					result &= MetallicTexture.lock() == other->MetallicTexture.lock();
					if (!result)
						return false;
				}
				else if ((MetallicTexture.expired() != other->MetallicTexture.expired()))
				{
					return false;
				}



				if (!RoughnessTexture.expired() && !other->RoughnessTexture.expired())
				{
					result &= RoughnessTexture.lock() == other->RoughnessTexture.lock();
					if (!result)
						return false;
				}
				else if ((RoughnessTexture.expired() != other->RoughnessTexture.expired()))
				{
					return false;
				}


				if (!NormalTexture.expired() && !other->NormalTexture.expired())
				{
					result &= NormalTexture.lock() == other->NormalTexture.lock();
					if (!result)
						return false;
				}
				else if ((NormalTexture.expired() != other->NormalTexture.expired()))
				{
					return false;
				}

				if (!EmissiveTexture.expired() && !other->EmissiveTexture.expired())
				{
					result &= EmissiveTexture.lock() == other->EmissiveTexture.lock();
					if (!result)
						return false;
				}
				else if ((EmissiveTexture.expired() != other->EmissiveTexture.expired()))
				{
					return false;
				}

				return result;
			}

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

	public:
		LightingStaticPipeline() = default;
		~LightingStaticPipeline() override = default;

		void PrepareRecord(const PipelineInputs* const pipelineInputs) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		STATIC_TYPE_IMPLE(LightingStaticPipeline)
	private:
		std::map <std::shared_ptr<Renderable>, std::vector<PerInstanceData>> m_PerInstanceData;
		std::map<std::shared_ptr<Renderable>, std::unique_ptr<Buffer>> m_PerInstanceVertexBuffers; // should vary size
		std::map<std::shared_ptr<Renderable>, std::array<MaterialBase*, 10>> m_MaterialCache;
		std::map<std::shared_ptr<Renderable>, UINT64> m_RenderableDescriptorHandleOffset;
		
		
	};
}
