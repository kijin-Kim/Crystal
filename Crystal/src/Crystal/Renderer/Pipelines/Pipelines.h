#pragma once
#include <string>
#include <vector>

#include "Crystal/GamePlay/Components/Component.h"
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
		};

	public:
		RenderPipeline(const std::string& name) : Pipeline(name) {}
		~RenderPipeline() override = default;

		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs) override;
	};

	class ComputePipeline : public Pipeline
	{
	public:
		struct ComputePipelineInputs : public PipelineInputs
		{
		};

	public:
		ComputePipeline(const std::string& name) : Pipeline(name) {}
		~ComputePipeline() override = default;

		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs) override { Pipeline::Record(commandList, pipelineInputs); }
	};

}
