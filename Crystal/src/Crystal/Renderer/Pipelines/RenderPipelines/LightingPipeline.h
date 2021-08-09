#pragma once
#include "Crystal/GamePlay/Components/MeshComponents.h"

namespace Crystal {

	class LightingPipeline : public RenderPipeline
	{
	public:

		struct Light
		{
			ELightType LightType = LT_Directional;
			float Intensity;
			float AttenuationRadius;
			bool bCastShadow;

			DirectX::XMFLOAT3 Direction = Vector3::Zero;
			float _pad2;

			DirectX::XMFLOAT3 Position = Vector3::Zero;
			float _pad3;

			DirectX::XMFLOAT3 Color = Vector3::White;
			float _pad4;
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection = Matrix4x4::Identity();
			DirectX::XMFLOAT4X4 LightViewProjection = Matrix4x4::Identity();
			DirectX::XMFLOAT3 CameraPositionInWorld = Vector3::Zero;
			float _pad1;

			Light Lights[100];

			uint32_t LightCount = 0;
			float _pad2[3]; // just in case
		};


	public:
		LightingPipeline() = default;
		~LightingPipeline() override = default;


		void OnCreate() override;
		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
	
	private:
		std::shared_ptr<StaticMeshComponent> m_StaticMeshComponent;
	};


}
