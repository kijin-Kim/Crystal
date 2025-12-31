#pragma once
#include "Crystal/Renderer/InstancedStaticMesh.h"
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/Material.h"

namespace Crystal {
	class ForwardStaticPipeline final : public RenderPipeline
	{
	public:
		struct Light
		{
			ELightType LightType;
			float Intensity;
			float AttenuationRadius;
			bool bCastShadow = false;

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

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
		};




	public:
		ForwardStaticPipeline() = default;
		~ForwardStaticPipeline() override = default;


		void OnCreate() override;
		void Begin(const Shared<Scene>& scene) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;
		STATIC_TYPE_IMPLE(ForwardStaticPipeline)
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		InstancedStaticMesh m_InstancedStaticMesh;

		
		std::unordered_map<PipelineStateKey, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_PipelineStates;
		std::unordered_map<ID3D12PipelineState*, InstancedStaticMesh> m_InstancedStaticMeshes;

	};
}


