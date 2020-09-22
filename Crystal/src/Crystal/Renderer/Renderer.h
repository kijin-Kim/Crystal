#pragma once
#include <wrl/client.h>
#include <vector>

#include "Camera.h"
#include "CommandQueue.h"
#include "Crystal/Core/Timer.h"
#include "Crystal/Core/WindowsWindow.h"
#include "Pipeline.h"
#include "ShaderLibrary.h"

#include "Model.h"

namespace Crystal {

	class Renderer final
	{
	public:
		void Init(const std::shared_ptr<WindowsWindow>& window);
		Renderer(const Renderer&) = delete;
		static Renderer& Get() { static Renderer instance; return instance; }

		void Render();

		ID3D12Device2* GetDevice() const { return m_Device.Get(); }
		IDXGIFactory4* GetFactory() const { return m_Factory.Get(); }
		std::shared_ptr<CommandQueue> GetCommandQueue() const { return m_CommandQueue; }
		const std::shared_ptr<ShaderLibrary> GetShaderLibrary() const { return m_ShaderLibrary; }
	private:
		Renderer() {};

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

		std::shared_ptr<ShaderLibrary> m_ShaderLibrary = std::make_shared<ShaderLibrary>();

		std::vector<std::unique_ptr<RenderTarget>> m_RenderTargets;
		std::unique_ptr<DepthStencil> m_DepthStencil = nullptr;

		std::unique_ptr<GraphicsPipeline> m_GraphicsPipeline = nullptr;

		std::unique_ptr<Camera> m_Camera = nullptr;

		DirectX::XMFLOAT4X4 m_WorldMat = {};

		std::unique_ptr<RootSignature> m_RootSignature = nullptr;

		Timer timer;

		//Microsoft::WRL::ComPtr<ID3D12Resource> m_TextureBuffer = nullptr;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_TextureBuffers;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CommonDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_TextureDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstantBufferResource = nullptr;


		struct ConstantBufferData
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT4X4 ViewProj;
			DirectX::XMFLOAT3 LightPositionInWorld;
			DirectX::XMFLOAT3 CameraPositionInWorld;
		} m_ConstantBufferData;


		Model* model;


	};
}