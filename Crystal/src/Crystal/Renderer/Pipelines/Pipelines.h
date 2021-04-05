#pragma once
#include <string>
#include <vector>

#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"
#include "Crystal/Resources/Texture.h"
#include "Crystal/Resources/Shader.h"
#include "Crystal/GamePlay/Components/PrimitiveComponent.h"

namespace Crystal {
	class Pipeline : public Object
	{
	public:
		enum PipelineOwnerType
		{
			Owner_Shader = 0
		};

		struct PipelineInputs
		{
		};

	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;

		void RegisterPipelineComponents(std::weak_ptr<PrimitiveComponent> component) { m_Components.push_back(component); }
	
		virtual void PrepareRecord(const PipelineInputs* const pipelineInputs);
		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) {}
		virtual void EndRecord() {}

		bool IsValidForThisPipeline(const std::shared_ptr<Material>& material);

		STATIC_TYPE_IMPLE(Pipeline)
	protected:
		std::string m_Name;
		std::vector<std::weak_ptr<PrimitiveComponent>> m_Components;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = nullptr;


	};

	class RenderPipeline : public Pipeline
	{
	public:
		struct RenderPipelineInputs : public PipelineInputs
		{
			CameraComponent* Camera = nullptr;
		};
	public:
		RenderPipeline() = default;
		~RenderPipeline() override = default;

		void OnCreate() override;

		void PrepareRecord(const PipelineInputs* const pipelineInputs) override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

		STATIC_TYPE_IMPLE(RenderPipeline)

	};

	class ComputePipeline : public Pipeline
	{
	public:
		struct ComputePipelineInputs : public PipelineInputs
		{
		};
	public:
		ComputePipeline() = default;
		~ComputePipeline() override = default;

		void OnCreate() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

		STATIC_TYPE_IMPLE(ComputePipeline)
	};
}
