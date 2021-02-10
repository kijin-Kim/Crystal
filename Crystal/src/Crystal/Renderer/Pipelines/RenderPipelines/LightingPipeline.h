#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/ConstantBuffer.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"

namespace Crystal {

	class LightingPipeline final : public RenderPipeline
	{
	public:
		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT4X4 Bones[100];
			DirectX::XMFLOAT4 AlbedoColor = { 1.0f, 1.0f, 1.0f, 0.0f };
			int bToggleAlbedoTexture = false;
			int bToggleMetalicTexture = false;
			int bToggleRoughnessTexture = false;
			int bToggleNormalTexture = false;
			int bToggleIrradianceTexture = false;
			float RoughnessConstant = 0.0f;
			float MetalicConstant = 0.0f;
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection;
			DirectX::XMFLOAT4 LightPositionInWorld;
			DirectX::XMFLOAT4 CameraPositionInWorld;
		};

		enum EPBRDescriptorInputType
		{
			IrradianceTexture = 0,
			PerObjectConstants,
			AlbedoTexture,
			RoughnessTexture,
			MetalicTexture,
			NormalTexture,
			PBR_INPUT_COUNT
		};

		struct LightingPipelineInputs : public RenderPipelineInputs
		{
			std::vector<SkeletalMeshComponent*>* SkeletalMeshComponents = nullptr;
			Texture* IrradiancemapTexture = nullptr;
		};


	public:
		LightingPipeline(const std::string& name);
		virtual ~LightingPipeline() = default;

		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs) override;

	private:
		PerObjectData m_PerObjectData = {};
		PerFrameData m_PerFrameData = {};

		std::unique_ptr<ConstantBuffer> m_PerFrameConstantBuffer = nullptr;
		std::vector<std::unique_ptr<ConstantBuffer>> m_PerObjectConstantBuffers;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = nullptr;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_AnimatedPipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_AnimatedRootSignature = nullptr;

	};
}
