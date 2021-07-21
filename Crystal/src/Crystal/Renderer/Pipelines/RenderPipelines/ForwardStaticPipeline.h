#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Material.h"

namespace Crystal {
	class ForwardStaticPipeline final : public RenderPipeline
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


		struct PerInstanceData
		{
			DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();
			DirectX::XMFLOAT3 AlbedoColor = Vector3::Zero;
			DirectX::XMFLOAT3 EmissiveColor = Vector3::Zero;
			float RoughnessConstant = 0.0f;
			float MetallicConstant = 0.0f;

			uint32_t bShouldLit = true;
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
			Material* Material = nullptr;
			UINT64 DescriptorOffset = -1;
		};

	public:
		ForwardStaticPipeline() = default;
		~ForwardStaticPipeline() override = default;


		void OnCreate() override;
		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;
		STATIC_TYPE_IMPLE(ForwardStaticPipeline)
	private:
		std::map<Renderable*, InstanceBatch> m_InstanceBatches;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		

		std::unordered_map<PipelineStateKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_PipelineStates;

	};
}
