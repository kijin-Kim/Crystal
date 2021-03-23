#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {

	class PanoToCubemapPipeline : public ComputePipeline
	{
	public:
		enum EPanoToCubemapDescriptorInputType
		{
			SourceTexture = 0,
			DestinationTexture = 1,
			PANO_TO_CUBEMAP_INPUT_COUNT
		};


		struct PanoToCubemapPipelineInputs : public ComputePipelineInputs
		{
			Texture* SourceTexture = nullptr;
			Texture* DestinationTexture = nullptr;
		};

		PanoToCubemapPipeline(const std::string& name);
		virtual ~PanoToCubemapPipeline() {}

		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs) override;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = nullptr;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		unsigned int m_DestinationTextureWidth = 0;
		unsigned int m_DestinationTextureHeight = 0;

	};
}
