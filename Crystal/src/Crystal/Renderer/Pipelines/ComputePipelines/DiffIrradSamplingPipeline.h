#pragma once
#include "Crystal/Renderer/Pipelines/Pipelines.h"

namespace Crystal {
	class DiffIrradSamplingPipeline : public ComputePipeline
	{
	public:
		enum EDiffIrradSamplingDescriptorInputType
		{
			SourceTexture = 0,
			DestinationTexture = 1,
			DIFF_IRRAD_SAMPLING_INPUT_COUNT
		};

		struct DiffIrradSamplingPipelineInputs : public ComputePipelineInputs
		{
			Texture* SourceTexture = nullptr;
			Texture* DestinationTexture = nullptr;
		};

		DiffIrradSamplingPipeline(const std::string& name, const std::shared_ptr<Shader>& shader);
		~DiffIrradSamplingPipeline() override = default;

		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs) override;
		
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = nullptr;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

		unsigned int m_DestinationTextureWidth = 0;
		unsigned int m_DestinationTextureHeight = 0;
	};
}
