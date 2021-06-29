#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"


namespace Crystal {

	class ShadowMapSkeletalPipeline : public RenderPipeline
	{
	public:

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT4X4 Bones[300];
		};

	public:
		ShadowMapSkeletalPipeline() = default;
		~ShadowMapSkeletalPipeline() override = default;


		void OnCreate() override;
		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		STATIC_TYPE_IMPLE(ShadowMapSkeletalPipeline)
	
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;


	};

}
