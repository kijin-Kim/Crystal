#pragma once
#include <wrl/client.h>
#include <vector>

#include "CommandQueue.h"
#include "Crystal/Core/Timer.h"
#include "Crystal/Core/WindowsWindow.h"
#include "Pipeline.h"
#include "Crystal/AssetManager/ShaderManager.h"

#include "Crystal/Gameplay/Components/MeshComponent.h"
#include "Crystal/AssetManager/ConstantBuffer.h"
#include "Crystal/AssetManager/Texture.h"

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
		void ChangeResolution(const char* formattedResolution);
		void ChangeDisplayMode();

		bool GetIsFullScreenMode() const { return m_bIsFullScreen; }
		void ActiveFullScreenMode(bool bActive) { m_bIsFullScreen = bActive; }

		/// SHOULD GET DRAWABLE //
		void RegisterMeshComponent(MeshComponent* meshComponent) { m_MeshComponents.push_back(meshComponent); }

	private:
		Renderer() = default;
		~Renderer();

		void createDevice();
		void createRenderTargetViewFromSwapChain();
		void createDepthStencilView();
		void createPipelineStates();
		void loadResources();
	private:
		WindowsWindow* m_Window = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_SwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_d3d12CommandQueue = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence = nullptr;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PBRPipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_CubemapPipelineState = nullptr;

		HANDLE m_FenceEvent = nullptr;
		UINT64 m_FenceValue = 0;

		UINT m_RtvIndex = 0;

		std::shared_ptr<CommandQueue> m_CommandQueue = nullptr;

		DirectX::XMFLOAT4X4 m_WorldMat = {};

		std::unique_ptr<RootSignature> m_RootSignature = nullptr;

		Timer timer;

		//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CommonDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CommonDescriptorHeap = nullptr;

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
		};

		PerObjectData m_PerObjectData = {};
		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection;
			DirectX::XMFLOAT4 LightPositionInWorld;
			DirectX::XMFLOAT4 CameraPositionInWorld;
		};
		PerFrameData m_PerFrameData = {};
		std::vector<MeshComponent*> m_MeshComponents;

		float m_ClearColor[3] = { 0.0f, 0.0f, 0.0f };

		int m_ResWidth = 1920;
		int m_ResHeight = 1080;

		const char* m_ResolutionItems[4] = { "1920x1080", "1366x768", "1024x768", "800x600" };
		int m_CurrentResolutionIndex = 0;

		bool m_bIsFullScreen = false;

		ConstantBuffer m_PerFrameBuffer;
		ConstantBuffer m_PerObjectBuffer;
		ConstantBuffer m_CubemapCbuffer;

		class World* m_World;

		std::unique_ptr<VertexBuffer> m_QuadVertexBuffer;
		std::unique_ptr<IndexBuffer> m_QuadIndexBuffer;

		std::unique_ptr<RootSignature> m_CubemapRootSignature = nullptr;

		PlayerController* m_PlayerController = nullptr;

		std::unique_ptr<Texture> m_DiffuseTexture;
		std::unique_ptr<Texture> m_RoughnessTexture;
		std::unique_ptr<Texture> m_MetalicTexture;
		std::unique_ptr<Texture> m_CubemapTexture;

		std::unique_ptr<Texture> m_ColorBufferTextures[2];
		std::unique_ptr<Texture> m_DepthBufferTexture;
	};
}