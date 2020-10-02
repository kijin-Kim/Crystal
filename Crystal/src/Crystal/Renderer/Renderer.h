#pragma once
#include <wrl/client.h>
#include <vector>

#include "Camera.h"
#include "CommandQueue.h"
#include "Crystal/Core/Timer.h"
#include "Crystal/Core/WindowsWindow.h"
#include "Pipeline.h"
#include "Crystal/AssetManager/ShaderManager.h"

#include "Crystal/Gameplay/Components/MeshComponent.h"
#include "Crystal/AssetManager/ConstantBufferManager.h"

namespace Crystal {

	class Renderer final
	{
	public:
		void Init(const std::shared_ptr<WindowsWindow>& window);
		Renderer(const Renderer&) = delete;
		static Renderer& Instance() { static Renderer instance; return instance; }

		void Render();

		ID3D12Device2* GetDevice() const { return m_Device.Get(); }
		IDXGIFactory4* GetFactory() const { return m_Factory.Get(); }
		std::shared_ptr<CommandQueue> GetCommandQueue() const { return m_CommandQueue; }

	

		/////////////////////////////////////////////////////////////////////////////////
		//TEMP//////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		void ChangeResolution(const char* formattedResolution);
		void ChangeDisplayMode();

		/// SHOULD GET DRAWABLE //
		void RegisterMesh(MeshComponent* mesh) { m_Meshes.push_back(mesh); }
		
	private:
		Renderer() {};
		~Renderer();

	private:
		std::shared_ptr<WindowsWindow> m_Window = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_SwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence = nullptr;
		HANDLE m_FenceEvent = nullptr;
		UINT64 m_FenceValue = 0;

		UINT m_RtvIndex = 0;

		std::shared_ptr<CommandQueue> m_CommandQueue = nullptr;

		std::vector<std::unique_ptr<RenderTarget>> m_RenderTargets;
		std::unique_ptr<DepthStencil> m_DepthStencil = nullptr;

		std::unique_ptr<GraphicsPipeline> m_GraphicsPipeline = nullptr;

		std::unique_ptr<Camera> m_Camera = nullptr;

		DirectX::XMFLOAT4X4 m_WorldMat = {};

		std::unique_ptr<RootSignature> m_RootSignature = nullptr;

		Timer timer;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CommonDescriptorHeap = nullptr;



		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
		}m_PerObjectData;

		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 View;
			DirectX::XMFLOAT4X4 Projection;
			DirectX::XMFLOAT4 LightPositionInWorld;
			DirectX::XMFLOAT4 CameraPositionInWorld;
		}m_PerFrameData;

		std::vector<MeshComponent*> m_Meshes;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ImGuiDescriptorHeap = nullptr;
		float m_ClearColor[3] = { 0.0f, 0.0f, 0.0f };
		
		int m_ResWidth = 1366;
		int m_ResHeight = 768;

		const char* m_ResolutionItems[4] = { "1920x1080", "1366x768", "1024x768", "800x600" };
		int m_CurrentResolutionIndex = 0;

		bool m_bIsFullScreen = false;

		ConstantBuffer m_PerFrameBuffer;
		ConstantBuffer m_PerObjectBuffer;
	};
}