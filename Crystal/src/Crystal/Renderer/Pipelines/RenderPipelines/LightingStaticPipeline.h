#pragma once
#include "Crystal/Resources/ConstantBuffer.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "LightPipeline.h"

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

			int LightCount = 0;
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
			int bToggleAlbedoTexture = false;
			int bToggleMetallicTexture = false;
			int bToggleRoughnessTexture = false;
			int bToggleNormalTexture = false;
			int bToggleIrradianceTexture = false;
			int bToggleEmissiveTexture = false;
			float RoughnessConstant = 0.0f;
			float MetallicConstant = 0.0f;
		};

		struct LightingPipelineInputs : public RenderPipelineInputs
		{
			Texture* IrradiancemapTexture = nullptr;
		};

	public:
		LightingStaticPipeline() = default;
		~LightingStaticPipeline() override = default;

		void PrepareRecord(const PipelineInputs* const pipelineInputs) override;
		STATIC_TYPE_IMPLE(LightingStaticPipeline)
	};
}
