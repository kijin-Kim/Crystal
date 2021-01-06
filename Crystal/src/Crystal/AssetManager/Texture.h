#pragma once
#include <unordered_map>

namespace Crystal {

	class Descriptor
	{
	public:
		Descriptor() { m_Handle.ptr = 0; }
		Descriptor(D3D12_CPU_DESCRIPTOR_HANDLE handle) : m_Handle(handle) {}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const { return m_Handle; }

		bool IsNull() const { return m_Handle.ptr == 0; }

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE m_Handle;

	};

	// Data that API Expose to user (handle of data)
	class Texture final
	{
	public:
		/*�������� �Ӽ����� �ް� ���� ���ҽ��� ����ϴ�.*/
		Texture(int width, int height, int depth, int mipLevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES initialStates);
		/*���Ϸκ��� �ؽ��ĸ� �ε��մϴ�.*/
		Texture(const std::string& filePath, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);
		/*���ҽ��� �ǳ׹޾� �並 ����ϴ�.*/
		Texture(ID3D12Resource* resource, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);
		~Texture() = default;

		void CreateShaderResourceView(DXGI_FORMAT format, D3D12_SRV_DIMENSION srvDimension);
		void CreateUnorderedAccessView() {}
		void CreateRenderTargetView(DXGI_FORMAT format, D3D12_RTV_DIMENSION rtvDimension);
		void CreateDepthStencilView(DXGI_FORMAT format, D3D12_DSV_DIMENSION dsvDimension);

		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const
		{
			if (m_ShaderResourceView.IsNull())
				CS_ASSERT(false, "Shader Resource View�� ���� �������ּ���");
			return m_ShaderResourceView.GetDescriptorHandle();
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const
		{
			if (m_RenderTargetView.IsNull())
				CS_ASSERT(false, "Render Target View�� ���� �������ּ���");
			return m_RenderTargetView.GetDescriptorHandle();
		}


		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const
		{
			if (m_DepthStencilView.IsNull())
				CS_ASSERT(false, "Depth Stencil View�� ���� �������ּ���");
			return m_DepthStencilView.GetDescriptorHandle();
		}

		ID3D12Resource* GetResource() { return m_Resource.Get(); }

	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		Descriptor m_ShaderResourceView = {};
		Descriptor m_UnorderedAccessView = {};
		Descriptor m_DepthStencilView = {};
		Descriptor m_RenderTargetView = {};
	};

	/*������ �۷ι� ��*/
	class DescriptorHeapManager final
	{
	public:
		static DescriptorHeapManager& Instance() { static DescriptorHeapManager instance; return instance; }

		Descriptor AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, UINT descriptorCount);
	private:
		DescriptorHeapManager();
		~DescriptorHeapManager() = default;
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CBVSRVUAVHeap = nullptr;
		UINT m_CBVSRVUAVHeapIncrementSize = 0;
		UINT m_CBVSRVUAVHeapDescriptorCount = 0;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap = nullptr;
		UINT m_RTVHeapIncrementSize = 0;
		UINT m_RTVHeapDescriptorCount = 0;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap = nullptr;
		UINT m_DSVHeapIncrementSize = 0;
		UINT m_DSVHeapDescriptorCount = 0;
	};
}
