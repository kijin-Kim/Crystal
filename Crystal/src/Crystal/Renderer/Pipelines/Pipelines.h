#pragma once
#include <string>
#include <vector>

#include "Crystal/GamePlay/Components/BaseComponents.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"
#include "Crystal/Resources/Texture.h"

namespace Crystal {

	class Pipeline
	{
	public:
		struct PipelineInputs
		{
		};

	public:
		Pipeline(const std::string& name) : m_Name(name) {}
		virtual ~Pipeline() = default;

		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs);

	protected:
		std::string m_Name;
	};

	class RenderPipeline : public Pipeline
	{
	public:
		struct RenderPipelineInputs : public PipelineInputs
		{
			CameraComponent* Camera = nullptr;
			Texture* RenderTargets[8] = {};
			Texture* DepthStencilBuffer = nullptr;
			D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		};

	public:
		RenderPipeline(const std::string& name) : Pipeline(name) {}
		virtual ~RenderPipeline() = default;

		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs) override;
	};

	class ComputePipeline : public Pipeline
	{
	public:
		struct ComputePipelineInputs : public PipelineInputs
		{
		};

	public:
		ComputePipeline(const std::string& name) : Pipeline(name) {}
		virtual ~ComputePipeline() = default;

		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs) override { Pipeline::Record(commandList, pipelineInputs); }
	};

}
