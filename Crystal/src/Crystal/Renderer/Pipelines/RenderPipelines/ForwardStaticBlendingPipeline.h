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
			DirectX::XMFLOAT3 Direction = Vector3::Zero;
			float _pad1;

			DirectX::XMFLOAT3 Color = Vector3::White;
			float _pad2;

			float Intensity;
			float _pad3[3];
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
			
			int bShouldLit = true;
			float Opacity = 1.0f;
			int bToggleAlbedoTexture = false;
			int bToggleMetallicTexture = false;
			int bToggleRoughnessTexture = false;
			int bToggleNormalTexture = false;
			int bToggleIrradianceTexture = false;
			int bToggleEmissiveTexture = false;
		};




	public:
		ForwardStaticBlendingPipeline() = default;
		~ForwardStaticBlendingPipeline() override = default;


		void OnCreate() override;
		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;
		STATIC_TYPE_IMPLE(ForwardStaticBlendingPipeline)
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		std::unordered_map<PipelineStateKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_PipelineStates;
		

		Shared<Buffer> m_PerFrameConstantBuffer = nullptr;

		std::map<float, Weak<StaticMeshComponent>> m_AlphaSortedStaticMeshes;
	};
		
}
