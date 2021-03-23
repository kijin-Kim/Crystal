#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"
#include "Crystal/Resources/ConstantBuffer.h"
#include "Crystal/Resources/Buffers.h"

namespace Crystal {

	class CubemapPipeline final : public RenderPipeline
	{
	public:

		enum ECubemapDescriptorInputType
		{
			CubemapTexture = 0,
			CUBEMAP_INPUT_COUNT
		};

		struct CubemapPipelineInputs : public RenderPipelineInputs
		{
			Texture* CubemapTexture = nullptr;
		};

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 InverseViewProjection;
		};
		
	public:
		CubemapPipeline(const std::string& name);
		virtual ~CubemapPipeline() = default;

		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, 
			const PipelineInputs* const pipelineInputs) override;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = nullptr;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		std::unique_ptr<ConstantBuffer> m_PerFrameConstantBuffer = nullptr;

		std::unique_ptr<VertexBuffer> m_QuadVertexBuffer = nullptr;
		std::unique_ptr<IndexBuffer> m_QuadIndexBuffer = nullptr;

		PerFrameData m_PerFrameData = {};
	};
}
