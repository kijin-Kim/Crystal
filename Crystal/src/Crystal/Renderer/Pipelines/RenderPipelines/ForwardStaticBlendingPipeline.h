#pragma once
#include "Crystal/Types.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {
	class ForwardStaticBlendingPipeline final : public RenderPipeline
	{
	public:
		struct Light
		{
			ELightType LightType;
			float Intensity;
			float AttenuationRadius;
			bool bCastShadow;

			DirectX::XMFLOAT3 Direction = Vector3::Zero;
			float _pad2;

			DirectX::XMFLOAT3 Position = Vector3::Zero;
			float _pad3;

			DirectX::XMFLOAT3 Color = Vector3::White;
			float _pad4;
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection = Matrix4x4::Identity();
			DirectX::XMFLOAT4X4 LightViewProjection = Matrix4x4::Identity();
			DirectX::XMFLOAT3 CameraPositionInWorld = Vector3::Zero;
			float _pad1;
			Light Lights[100];
			uint32_t LightCount = 0;
			float _pad2[3]; // just in case
		};

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
		};

		struct PerDrawData
		{
			DirectX::XMFLOAT3 AlbedoColor = { 1.0f, 0.0f, 0.0f };
			float _pad1;
			DirectX::XMFLOAT3 EmissiveColor = { 1.0f, 0.0f, 0.0f };
			float _pad2;
			float RoughnessConstant = 0.0f;
			float MetallicConstant = 0.0f;
			DirectX::XMFLOAT2 TexCoordMultiplier = { 1.0f, 1.0f };
			
			int bShouldLit = true;
			float Opacity = 1.0f;
			float OpacityMultiplier = 1.0f;
			int bToggleAlbedoTexture = false;
			int bToggleMetallicTexture = false;
			int bToggleRoughnessTexture = false;
			int bToggleNormalTexture = false;
			int bToggleIrradianceTexture = false;
			int bToggleEmissiveTexture = false;
			int bToggleOpacityTexture = false;
		};




	public:
		ForwardStaticBlendingPipeline() = default;
		~ForwardStaticBlendingPipeline() override = default;


		void OnCreate() override;
		void Begin(const Shared<Scene>& scene) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;
		STATIC_TYPE_IMPLE(ForwardStaticBlendingPipeline)
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		std::unordered_map<PipelineStateKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_PipelineStates;
		

		Shared<Buffer> m_PerFrameConstantBuffer = nullptr;


		struct RenderData
		{
			Weak<StaticMeshComponent> Component;
			std::vector<SIZE_T> DescriptorHeapOffsets;
		};

		std::multimap<float, RenderData> m_AlphaSortedStaticMeshes;

		
	};
		
}
