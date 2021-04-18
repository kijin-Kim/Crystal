#pragma once
#include <wrl/client.h>
#include <vector>

#include "CommandQueue.h"
#include "Crystal/Core/WindowsWindow.h"
#include "Crystal/Resources/ShaderManager.h"

#include "Crystal/Resources/Texture.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/LightingStaticPipeline.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/CubemapPipeline.h"
#include "Crystal/Renderer/Pipelines/ComputePipelines/PanoToCubemapPipeline.h"
#include "Crystal/Renderer/Pipelines/ComputePipelines/DiffIrradSamplingPipeline.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Pipelines/RenderPipelines/LinePipeline.h"
#include "Crystal/Core/ApplicationUtility.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/Resources/DescriptorAllocator.h"


namespace Crystal {
	class PlayerController;
	class DescriptorAllocation;

	class Renderer final
	{
	public:
		void Init(WindowsWindow* window);
		Renderer(const Renderer&) = delete;
		static Renderer& Instance() { static Renderer instance; return instance; }

		void PrepareRender();
		void Render();

	
		std::shared_ptr<CommandQueue> GetCommandQueue() const { return m_CommandQueue; }


		void ChangeResolution(int width, int height);
		void ChangeDisplayMode();

		bool GetIsFullScreenMode() const { return m_bIsFullScreen; }
		void ActiveFullScreenMode(const bool bActive) { m_bIsFullScreen = bActive; }

		void RegisterLightComponent(std::weak_ptr<LightComponent> componentWeak);
		void RegisterRendererComponent(std::weak_ptr<PrimitiveComponent> componentWeak);
		
		

		int GetResolutionWidth() const { return m_ResWidth; }
		int GetResolutionHeight() const { return m_ResHeight; }

		std::shared_ptr<CameraComponent> GetCamera() { return ApplicationUtility::GetPlayerController().GetMainCamera().lock(); }

		DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorCount);
		void ReleaseStaleDescriptors();
	private:
		Renderer() = default;
		~Renderer() = default;

		void CreateDevice();
		void CreateDepthStencilView();

		void CreateRenderTargets();
		void DestroyRenderTargets();

		template <class T>
		std::unique_ptr<T> CreatePipeline(std::weak_ptr<Shader> shader, const std::string& name)
		{
			/*Pipeline¿ª ∏∏µÏ¥œ¥Ÿ.*/
			auto pipeline = std::make_unique<T>();
			pipeline->SetObjectOwner(shader, Pipeline::PipelineOwnerType::Owner_Shader);
			pipeline->SetObjectName(name);
			pipeline->OnCreate();

			return pipeline;
		}

	private:

		WindowsWindow* m_Window = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_SwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_D3d12CommandQueue = nullptr;

		HANDLE m_FenceEvent = nullptr;

		UINT m_RtvIndex = 0;

		std::shared_ptr<CommandQueue> m_CommandQueue = nullptr;

		int m_ResWidth = 1920;
		int m_ResHeight = 1080;
		bool m_bIsFullScreen = false;

		/*std::unique_ptr<Texture> m_RenderTargetTextures[2];*/
		/*std::unique_ptr<Texture> m_DepthStencilBufferTexture;*/

		std::weak_ptr<Texture> m_ColorBufferTextures[2];
		std::weak_ptr<Texture> m_DepthStencilBufferTexture;
		std::weak_ptr<Texture> m_BrightColorBuffer;
		std::weak_ptr<Texture> m_FloatingPointBuffer;

		//=== G-Buffers ==
		std::weak_ptr<Texture> m_AlbedoBuffer;
		std::weak_ptr<Texture> m_RoughnessMetallicAoBuffer;
		std::weak_ptr<Texture> m_EmissiveBuffer;
		std::weak_ptr<Texture> m_WorldNormalBuffer;
		std::weak_ptr<Texture> m_IrradianceBuffer;
		std::weak_ptr<Texture> m_WorldPositionBuffer;

		// TODO 
		std::shared_ptr<Texture> m_PanoTexture;
		std::shared_ptr<Texture> m_CubemapTexture;
		std::shared_ptr<Texture> m_IrradiancemapTexture;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ImGuiHeap = nullptr;

		std::unique_ptr<LightingStaticPipeline> m_LightingPipeline = nullptr;
		std::unique_ptr<LinePipeline> m_LinePipeline = nullptr;
		std::unique_ptr<CubemapPipeline> m_CubemapPipeline = nullptr;

		std::unique_ptr<PanoToCubemapPipeline> m_PanoToCubemapPipeline = nullptr;
		std::unique_ptr<DiffIrradSamplingPipeline> m_DiffIrradSamplingPipeline = nullptr;

		std::vector<std::unique_ptr<Pipeline>> m_Pipelines;
		std::vector<std::unique_ptr<LightPipeline>> m_LightPipelines;

		std::unique_ptr<DescriptorAllocator> m_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];


		using RenderGraph = std::vector<std::unique_ptr<Pipeline>>;

		RenderGraph m_RenderGraph;
	};
}