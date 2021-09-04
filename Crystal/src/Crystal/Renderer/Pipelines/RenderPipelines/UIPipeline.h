#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Material.h"

namespace Crystal {

	class UIPipeline : public RenderPipeline
	{
	public:
		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 View;
			DirectX::XMFLOAT4X4 Projection;
		};

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();
			DirectX::XMFLOAT3 AlbedoColor = Vector3::Cyan;
			int bToggleAlbedoTexture = false;
			DirectX::XMFLOAT3 TintColor = Vector3::White;
			float Opacity = 1.0f;
			float OpacityMultiplier = 1.0f;
			int bToggleOpacityTexture = false;
			int bUseAlbedoTextureAlpha = false;
		};


	public:
		UIPipeline() = default;
		~UIPipeline() override = default;

		void OnCreate() override;
		void Begin(const Shared<Scene>& scene) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;


		STATIC_TYPE_IMPLE(UIPipeline)


	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;


		struct RenderData
		{
			SIZE_T DescriptorHeapOffsets;
		};

		std::multimap<float, RenderData> m_SortedMeshes;
	};


}
