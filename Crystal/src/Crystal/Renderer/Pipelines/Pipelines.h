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
	struct Scene;

	class PrimitiveComponent;

	class Pipeline : public Object
	{

	public:
		Pipeline() = default;
		virtual ~Pipeline() = default;

		void RegisterPipelineComponents(std::weak_ptr<PrimitiveComponent> component);
	
		virtual void Begin();
		virtual void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) {}
		virtual void End() {}

		

		void PrepareConstantBuffers(int perFrameBufferSize = 0, int perObjectBufferSize = 0, int perDrawBufferSize = 0, int perDrawBufferCount = 0);

		void SetShader(const Shared<Shader>& shader) { m_Shader = shader; }

		const Shared<Scene>& GetScene();

		STATIC_TYPE_IMPLE(Pipeline)
	protected:
		std::vector<std::weak_ptr<PrimitiveComponent>> m_Components;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap = nullptr;


		std::unique_ptr<Buffer> m_PerFrameConstantBuffer = nullptr;
		std::vector<std::unique_ptr<Buffer>> m_PerObjectConstantBuffers;
		std::vector<std::unique_ptr<Buffer>> m_PerDrawConstantBuffers;


		Shared<Shader> m_Shader;


	};

	class RenderPipeline : public Pipeline
	{

	public:
		RenderPipeline() = default;
		~RenderPipeline() override = default;

		void OnCreate() override;

		void Begin() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

		STATIC_TYPE_IMPLE(RenderPipeline)

	};

	class ComputePipeline : public Pipeline
	{
	
	public:
		ComputePipeline() = default;
		~ComputePipeline() override = default;

		void OnCreate() override;
		void Record(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList) override;

		STATIC_TYPE_IMPLE(ComputePipeline)
	};
}
