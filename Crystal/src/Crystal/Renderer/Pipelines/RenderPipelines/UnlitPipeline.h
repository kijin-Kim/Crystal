#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Material.h"

namespace Crystal {
	class UnlitPipeline : public RenderPipeline
	{
	public:


		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 View;
			DirectX::XMFLOAT4X4 Projection;
		};



	public:
		UnlitPipeline() = default;
		~UnlitPipeline() override = default;

		void OnCreate() override;
		void Begin(const Shared<Scene>& scene) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;


		

		STATIC_TYPE_IMPLE(UnlitPipeline)


	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		struct PerDrawCallData
		{
			uint32_t HorizontalSubImageCount = 0;
			uint32_t VerticalSubImageCount = 0;
		};
		

		struct PerInstanceData
		{
			DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();
			uint32_t SubUVIndex = 0;

			int bToggleAlbedoTexture = false;
			int bToggleOpacityTexture = false;
			int bUseAlbedoTextureAlpha = false;
			DirectX::XMFLOAT3 AlbedoColor = Vector3::Cyan;
			float Opacity = 1.0f;
			float OpacityMultiplier = 1.0f;
		};


		struct InstanceBatch
		{		
			uint64_t InstanceCount = 1;
			uint64_t DescriptorHeapOffset = 0;
			std::vector<PerInstanceData> PerInstanceDatas;
			
		};

		struct DrawData
		{
			uint64_t InstanceCount = 1;
			uint64_t DescriptorHeapOffset = 0;
			Shared<Buffer> InstanceVertexBuffer = nullptr;
		};
		
		std::unordered_map<Material*, InstanceBatch> m_InstanceBatches;
		std::vector<DrawData> m_DrawDatas;


		Unique<Buffer> m_PerFrameConstantBuffer = nullptr;
		Unique<Buffer> m_PerDrawCallConstantBuffer = nullptr;
		
	};
	
}

