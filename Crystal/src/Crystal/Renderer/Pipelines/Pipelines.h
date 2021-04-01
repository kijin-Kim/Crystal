#pragma once
#include <string>
#include <vector>

#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"
#include "Crystal/Resources/Texture.h"
#include "Crystal/Resources/Shader.h"

namespace Crystal {
	class Pipeline
	{
	public:
		struct PipelineInputs
		{
		};

	public:
		Pipeline(const std::string& name, std::weak_ptr<Shader> shader) : m_Name(name), m_Shader(shader) {}
		virtual ~Pipeline() = default;

		
		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs);

	protected:
		std::string m_Name;
		std::shared_ptr<Shader> m_Shader;
	};

	class RenderPipeline : public Pipeline
	{
	public:
		struct RenderPipelineInputs : public PipelineInputs
		{
			CameraComponent* Camera = nullptr;
		};
	public:
		RenderPipeline(const std::string& name, const std::shared_ptr<Shader>& shader) : Pipeline(name, shader) {}
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
		ComputePipeline(const std::string& name, const std::shared_ptr<Shader>& shader) : Pipeline(name, shader) {}
		~ComputePipeline() override = default;

		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, const PipelineInputs* const pipelineInputs) override { Pipeline::Record(commandList, pipelineInputs); }
	};
}
