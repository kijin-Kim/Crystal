#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Material.h"

namespace Crystal {

	class GeometrySkeletalPipeline : public RenderPipeline
	{
	public:
		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection;
		};

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT4X4 Bones[300];
		};

		struct PerDrawData
		{
			DirectX::XMFLOAT3 AlbedoColor = Vector3::Black;
			int bToggleAlbedoTexture = false;
			int bToggleMetallicTexture = false;
			int bToggleRoughnessTexture = false;
			int bToggleNormalTexture = false;
			int bToggleIrradianceTexture = false;
			int bToggleEmissiveTexture = false;
			float RoughnessConstant = 0.0f;
			float MetallicConstant = 0.0f;
			DirectX::XMFLOAT3 EmissiveColor = Vector3::Black;
		};

	
		
	public:
		GeometrySkeletalPipeline() = default;
		~GeometrySkeletalPipeline() override = default;

		void OnCreate() override;
		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		
		STATIC_TYPE_IMPLE(GeometrySkeletalPipeline)

	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		Unique<Buffer> m_PerFrameConstantBuffer = nullptr;
		std::vector<Unique<Buffer>> m_PerObjectConstantBuffers;
		std::vector<Unique<Buffer>> m_PerDrawConstantBuffers;
		
	};
}
