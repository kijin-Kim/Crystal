#include "Crystal/Resources/ConstantBuffer.h"
#include "LightPipeline.h"

namespace Crystal {
	
	class LightingSkeletalPipeline final : public LightPipeline
	{
	public:
		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection;
			DirectX::XMFLOAT4 CameraPositionInWorld;
			DirectX::XMFLOAT4 LightPositionInWorld[20];
			int LightCount = 2;
		};

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT4X4 Bones[300];
		};

		struct PerDrawData
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

	public:
		LightingSkeletalPipeline() = default;
		~LightingSkeletalPipeline() override = default;


		void OnCreate() override;
		void PrepareRecord(const PipelineInputs* const pipelineInputs) override;
		STATIC_TYPE_IMPLE(LightingSkeletalPipeline)
	private:
		std::unique_ptr<ConstantBuffer> m_PerFrameConstantBuffer = nullptr;
		std::vector<std::unique_ptr<ConstantBuffer>> m_PerObjectConstantBuffers;
		std::vector<std::array<std::unique_ptr<ConstantBuffer>, 5>> m_PerDrawConstantBufferLists;

		PerFrameData m_PerFrameData = {};

	};



}