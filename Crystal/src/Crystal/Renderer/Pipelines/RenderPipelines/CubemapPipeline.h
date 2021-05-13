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

		void OnCreate() override;
		void Begin() override;

		STATIC_TYPE_IMPLE(CubemapPipeline)

	private:
		std::shared_ptr<StaticMeshComponent> m_StaticMeshComponent = nullptr;
	};
}
