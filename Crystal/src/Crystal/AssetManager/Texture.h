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
		/*¿©±â¼­ ºä¸¦ ¸¸µì´Ï´Ù.*/

	public:
		Texture(const std::string& filePath);
		~Texture() = default;

		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const { return m_ShaderResourceView.GetDescriptorHandle(); }
	private:
		void createShaderResourceView(const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);


	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		Descriptor m_ShaderResourceView;
		Descriptor m_UnorderedAccessView;
		Descriptor m_DepthStencilView;
		Descriptor m_RenderTargetView;
		UINT m_Count;
	};

	/*°£´ÜÇÑ ±Û·Î¹ú Èü*/
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
