#pragma once
#include <wrl/client.h>
#include <vector>

#include "CommandQueue.h"
#include "Crystal/Core/WindowsWindow.h"
#include "Crystal/Resources/ShaderManager.h"

#include "Crystal/GamePlay/Components/MeshComponent.h"
#include "Crystal/Resources/Texture.h"
#include "Crystal/Renderer/Pipelines/LightingPipeline.h"
#include "Crystal/Renderer/Pipelines/CubemapPipeline.h"
#include "Crystal/Renderer/Pipelines/ClearPipeline.h"

namespace Crystal {
	class PlayerController;

	class Renderer final
	{
	public:
		/*현재 파이프라인에 바인드되어있는 global state입니다.*/
		//이거 RENDER COMMAND LIST 당 있어야할듯
		struct GlobalRenderState
		{
			D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology = {};
			D3D12_VIEWPORT Viewport = {};
			D3D12_RECT ScissorRect = {};
			Texture* RenderTargets[8] = {};
			Texture* DepthStencilBuffer = nullptr;
		};


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
		void ActiveFullScreenMode(const bool bActive) { m_bIsFullScreen = bActive; }

		/// SHOULD GET DRAWABLE //
		void RegisterMeshComponent(RenderComponent* meshComponent) { m_RenderComponents.push_back(meshComponent); }

		const GlobalRenderState& GetGlobalRenderState() const { return m_GlobalRenderState; }

	private:
		Renderer() = default;
		~Renderer() = default;
		
		void CreateDevice();
		void CreateRenderTargetViewFromSwapChain();
		void CreateDepthStencilView();
		void CreatePipelineStates();
		void CreateComputePipelineStates();
		void LoadResources();
	private:
		WindowsWindow* m_Window = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_SwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_D3d12CommandQueue = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence = nullptr;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PbrPipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PbrAnimatedPipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_CubemapPipelineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_EquiToCubePipeline = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_DiffuseIrradianceSamplingPipeline = nullptr;

		HANDLE m_FenceEvent = nullptr;

		UINT m_RtvIndex = 0;

		std::shared_ptr<CommandQueue> m_CommandQueue = nullptr;

		DirectX::XMFLOAT4X4 m_WorldMat = Matrix4x4::Identity();

		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_NormalRootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_CubemapRootSignature  = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_EquiToCubeRootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_DiffuseIrradianceSamplingRootSignature = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CommonDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CubemapDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_EquiToCubeDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DiffuseIrradianceSamplingDescriptorHeap = nullptr;

		struct PerObjectData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT4X4 Bones[100];
			DirectX::XMFLOAT4 AlbedoColor = { 1.0f, 1.0f, 1.0f, 0.0f };
			int bToggleAlbedoTexture = false;
			int bToggleMetalicTexture = false;
			int bToggleRoughnessTexture = false;
			int bToggleNormalTexture = false;
			int bToggleIrradianceTexture = false;
			float RoughnessConstant = 0.0f;
			float MetalicConstant = 0.0f;
		};
		PerObjectData m_PerObjectData = {};
		struct PerFrameData
		{
			DirectX::XMFLOAT4X4 ViewProjection;
			DirectX::XMFLOAT4 LightPositionInWorld;
			DirectX::XMFLOAT4 CameraPositionInWorld;
		};
		PerFrameData m_PerFrameData = {};

		struct PerFrameDataCubemap
		{
			DirectX::XMFLOAT4X4 InverseViewProjection;
		};
		PerFrameDataCubemap m_PerFrameDataCubemap = {};

		std::vector<RenderComponent*> m_RenderComponents;

		float m_ClearColor[3] = { 0.0f, 0.0f, 0.0f };

		int m_ResWidth = 800;
		int m_ResHeight = 600;

		const char* m_ResolutionItems[4] = { "1920x1080", "1366x768", "1024x768", "800x600" };
		int m_CurrentResolutionIndex = 3;

		bool m_bIsFullScreen = false;

		std::unique_ptr<VertexBuffer> m_QuadVertexBuffer;
		std::unique_ptr<IndexBuffer> m_QuadIndexBuffer;

		std::unique_ptr<Texture> m_RenderTargetTextures[2];
		std::unique_ptr<Texture> m_DepthStencilBufferTexture;
		std::unique_ptr<Texture> m_EquirectangularTexture;
		std::unique_ptr<Texture> m_CubemapTexture;
		std::unique_ptr<Texture> m_IrradiancemapTexture;
		

		Microsoft::WRL::ComPtr<ID3D12Resource> textureUploadBuffer = nullptr;


		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_ImGuiHeap = nullptr;


		std::unique_ptr<LightingPipeline> m_LightingPipeline = nullptr;
		std::unique_ptr<CubemapPipeline> m_CubemapPipeline = nullptr;
		std::unique_ptr<ClearPipeline> m_ClearPipeline = nullptr;



		GlobalRenderState m_GlobalRenderState = {};
		
	};
}