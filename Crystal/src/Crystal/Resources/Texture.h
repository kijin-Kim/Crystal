#pragma once
#include <unordered_map>
#include "Crystal/Core/Logger.h"
#include "DescriptorAllocation.h"

namespace Crystal {
	class Texture
	{
	public:
		/*여러가지 속성들을 받고 직접 리소스를 만듭니다.*/
		Texture(int width, int height, int depth, int mipLevels, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS resourceFlags,
		        D3D12_RESOURCE_STATES initialStates);
		/*파일로부터 텍스쳐를 로드합니다.*/
		Texture(const std::string& filePath, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);
		/*리소스를 건네받아 뷰를 만듭니다.*/
		Texture(ID3D12Resource* resource, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE);
		virtual ~Texture() = default;

		DescriptorAllocation CreateShaderResourceView(D3D12_SRV_DIMENSION srvDimension);
		DescriptorAllocation CreateUnorderedAccessView(D3D12_UAV_DIMENSION uavDimension);
		DescriptorAllocation CreateRenderTargetView(D3D12_RTV_DIMENSION rtvDimension);
		DescriptorAllocation CreateDepthStencilView(D3D12_DSV_DIMENSION dsvDimension);


		D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView(D3D12_SRV_DIMENSION srvDimension);
		D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(D3D12_UAV_DIMENSION uavDimension);
		D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView(D3D12_RTV_DIMENSION rtvDimension);
		D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView(D3D12_DSV_DIMENSION dsvDimension);


		ID3D12Resource* GetResource() const { return m_Resource.Get(); }

	private:
	
		
		
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		
		std::unordered_map<D3D12_SRV_DIMENSION, DescriptorAllocation> m_ShaderResourceViews;
		std::unordered_map<D3D12_UAV_DIMENSION, DescriptorAllocation> m_UnorderedAccessViews;
		std::unordered_map<D3D12_RTV_DIMENSION, DescriptorAllocation> m_RenderTargetViews;
		std::unordered_map<D3D12_DSV_DIMENSION, DescriptorAllocation> m_DepthStencilViews;

		
		
	};
}
