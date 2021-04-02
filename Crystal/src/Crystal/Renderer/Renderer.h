#pragma once
#include <wrl/client.h>
#include <vector>

#include "CommandQueue.h"
#include "Crystal/Core/WindowsWindow.h"
#include "Crystal/Resources/ShaderManager.h"

#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/Resources/Texture.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/LightingStaticPipeline.h"
#include "Crystal/Renderer/Pipelines/RenderPipelines/CubemapPipeline.h"
#include "Crystal/Renderer/Pipelines/ComputePipelines/PanoToCubemapPipeline.h"
#include "Crystal/Renderer/Pipelines/ComputePipelines/DiffIrradSamplingPipeline.h"
#include "Crystal/GamePlay/Components/CollisionComponent.h"
#include "Pipelines/RenderPipelines/LinePipeline.h"

namespace Crystal {
	class PlayerController;

	class Renderer final
	{
	public:
		void Init(WindowsWindow* window);
		Renderer(const Renderer&) = delete;
		static Renderer& Instance() { static Renderer instance; return instance; }

		void Render();

		ID3D12Device2* GetDevice() const { return m_Device.Get(); }
		IDXGIFactory4* GetFactory() const { return m_Factory.Get(); }
		std::shared_ptr<CommandQueue> GetCommandQueue() const { return m_CommandQueue; }

		/////////////////////////////////////////////////////////////////////////////////
		//TEMP//////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		void ChangeResolution(int width, int height);
		void ChangeDisplayMode();

		bool GetIsFullScreenMode() const { return m_bIsFullScreen; }
		void ActiveFullScreenMode(const bool bActive) { m_bIsFullScreen = bActive; }

		void RegisterRendererComponent(std::weak_ptr<PrimitiveComponent> componentWeak);
		

		int GetResolutionWidth() const { return m_ResWidth; }
		int GetResolutionHeight() const { return m_ResHeight; }

	private:
		Renderer() = default;
		~Renderer() = default;

		void CreateDevice();
		void CreateRenderTargetViewFromSwapChain();
		void CreateDepthStencilView();
	private:

		WindowsWindow* m_Window = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_SwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_D3d12CommandQueue = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence = nullptr;

		HANDLE m_FenceEvent = nullptr;

		UINT m_RtvIndex = 0;

		std::shared_ptr<CommandQueue> m_CommandQueue = nullptr;

		std::vector<StaticMeshComponent*> m_StaticMeshComponents;
		std::vector<SkeletalMeshComponent*> m_SkeletalMeshComponents;
		std::vector<CollisionComponent*> m_CollisionComponents;

		int m_ResWidth = 1920;
		int m_ResHeight = 1080;
		bool m_bIsFullScreen = false;

		std::unique_ptr<Texture> m_RenderTargetTextures[2];
		std::unique_ptr<Texture> m_DepthStencilBufferTexture;

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

		std::vector<std::unique_ptr<RenderPipeline>> m_MainColorPipelines;
	};
}