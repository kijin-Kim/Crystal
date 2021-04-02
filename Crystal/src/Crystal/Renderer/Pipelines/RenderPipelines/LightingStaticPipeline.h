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

		struct StaticMeshPerObjectData
		{
			DirectX::XMFLOAT4X4 World;
		};

		struct SkeletalMeshPerObjectData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT4X4 Bones[200];
		};

		struct PerMaterialData
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
		
	private:
		PerFrameData m_PerFrameData = {};

		std::unique_ptr<ConstantBuffer> m_PerFrameConstantBuffer = nullptr;

		std::vector<std::unique_ptr<ConstantBuffer>> m_StaticMeshPerObjectConstantBuffers;
		std::vector<std::array<std::unique_ptr<ConstantBuffer>, 5>> m_StaticMeshPerMaterialConstantBufferLists;
		/*Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_StaticMeshDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_StaticMeshPipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_StaticMeshRootSignature = nullptr;*/

		std::vector<std::unique_ptr<ConstantBuffer>> m_SkeletalMeshPerObjectConstantBuffers;
		std::vector<std::array<std::unique_ptr<ConstantBuffer>, 5>> m_SkeletalMeshPerMaterialConstantBufferLists;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SkeletalMeshDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_SkeletalMeshPipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_SkeletalMeshRootSignature = nullptr;
	};
}
