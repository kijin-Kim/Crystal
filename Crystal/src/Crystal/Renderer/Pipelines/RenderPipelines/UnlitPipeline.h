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
		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;


		

		STATIC_TYPE_IMPLE(UnlitPipeline)


	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		struct PerInstanceData
		{
			DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();
		};

		struct InstanceBatch
		{		
			uint64_t InstanceCount = 1;
			uint64_t TextureOffset = 0;
			std::vector<PerInstanceData> PerInstanceDatas;
			
		};

		struct DrawData
		{
			uint64_t InstanceCount = 1;
			uint64_t TextureOffset = 0;
			Unique<Buffer> InstanceVertexBuffer = nullptr;
		};
		
		std::unordered_map<Texture*, InstanceBatch> m_InstanceBatches;
		std::vector<DrawData> m_DrawDatas;


		Unique<Buffer> m_PerFrameConstantBuffer = nullptr;
		
	};
	
}

