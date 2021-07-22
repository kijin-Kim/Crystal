#pragma once
#include "Crystal/Resources/Buffer.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"

#include <vector>

#include "Crystal/Renderer/InstancedStaticMesh.h"


namespace Crystal {
	
	

	

	
	class GeometryStaticPipeline final : public RenderPipeline
	{
	public:
		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection = Matrix4x4::Identity();
		};

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
		};



	public:
		GeometryStaticPipeline() = default;
		~GeometryStaticPipeline() override = default;


		void OnCreate() override;
		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;
		void End() override;
		STATIC_TYPE_IMPLE(GeometryStaticPipeline)
	private:
		InstancedStaticMesh m_InstancedStaticMesh;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;


		Shared<Buffer> m_PerFrameConstantBuffer = nullptr;
	};




}
