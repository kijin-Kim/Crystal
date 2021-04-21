#pragma once
#include <string>
#include <vector>

#include "Crystal/GamePlay/Components/Component.h"
#include "Crystal/Renderer/CommandQueue.h"
#include "Crystal/GamePlay/Components/CameraComponent.h"
#include "Crystal/Resources/Texture.h"
#include "Crystal/Resources/Shader.h"
#include "Crystal/Resources/Buffer.h"

namespace Crystal {

	class PrimitiveComponent;
	class Material;

	class Pipeline : public Object
	{
	public:
		struct MaterialBase : public Object
		{
			enum MaterialOwnerType
			{
				Owner_Shader = 0
			};
			
			virtual bool UsingSameTextures(MaterialBase* material) { return false; }

			STATIC_TYPE_IMPLE(MaterialBase)
		};


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

		void RegisterPipelineComponents(std::weak_ptr<PrimitiveComponent> component);
	
		virtual void Begin(const PipelineInputs* const pipelineInputs);
		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) {}
		virtual void End() {}

		bool IsValidForThisPipelineNew(MaterialBase* material);

		void PrepareConstantBuffers(int perFrameBufferSize = 0, int perObjectBufferSize = 0, int perDrawBufferSize = 0, int perDrawBufferCount = 0);

		STATIC_TYPE_IMPLE(Pipeline)
	protected:
		std::string m_Name;
		std::vector<std::weak_ptr<PrimitiveComponent>> m_Components;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = nullptr;


		std::unique_ptr<Buffer> m_PerFrameConstantBuffer = nullptr;
		std::vector<std::unique_ptr<Buffer>> m_PerObjectConstantBuffers;
		std::vector<std::unique_ptr<Buffer>> m_PerDrawConstantBuffers;


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

		void Begin(const PipelineInputs* const pipelineInputs) override;
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
