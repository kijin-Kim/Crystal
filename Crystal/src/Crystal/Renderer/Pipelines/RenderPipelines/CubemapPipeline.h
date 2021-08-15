#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Buffer.h"

namespace Crystal {
	class StaticMeshComponent;

	class CubemapPipeline final : public RenderPipeline
	{
	public:


		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 InverseViewProjection;
		};

		

	public:
		CubemapPipeline() = default;
		~CubemapPipeline() override = default;

		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

		STATIC_TYPE_IMPLE(CubemapPipeline)
	};
}
